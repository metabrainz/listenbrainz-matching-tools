#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <cstring>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

namespace lb_matching_tools {

// Guff words - defined inline for header-only
inline const std::vector<std::string> GUFF_PAREN_WORDS = {
    "a cappella", "acoustic", "bonus", "censored", "clean", "club", "clubmix", 
    "composition", "cut", "dance", "demo", "dialogue", "dirty", "edit", "excerpt", 
    "explicit", "extended", "instrumental", "interlude", "intro", "karaoke", "live", 
    "long", "main", "maxi", "megamix", "mix", "mono", "official", "orchestral", 
    "original", "outro", "outtake", "outtakes", "piano", "quadraphonic", "radio", 
    "rap", "re-edit", "reedit", "refix", "rehearsal", "reinterpreted", "released", 
    "release", "remake", "remastered", "remaster", "master", "remix", "remixed", 
    "remode", "reprise", "rework", "reworked", "rmx", "session", "short", "single", 
    "skit", "stereo", "studio", "take", "takes", "tape", "track", "tryout", 
    "uncensored", "unknown", "unplugged", "untitled", "version", "ver", "video", 
    "vocal", "vs", "with", "without"
};

/**
 * @brief Result of a regex match operation
 */
struct MatchResult {
    bool matched;
    std::string title;
    std::string enclosed;
    std::string feat;
    std::string artists;
    std::string dash;
    std::string comma;
    
    MatchResult() : matched(false) {}
};

/**
 * @brief RAII wrapper for PCRE2 compiled regex patterns
 */
class CompiledRegex {
public:
    explicit CompiledRegex(const std::string& pattern, bool case_insensitive = true, bool utf8 = true) 
        : code_(nullptr), match_data_(nullptr) {
        
        int errornumber;
        PCRE2_SIZE erroroffset;
        
        // Set up compile options
        uint32_t options = 0;
        if (case_insensitive) options |= PCRE2_CASELESS;
        if (utf8) options |= PCRE2_UTF;
        
        // Compile the pattern
        code_ = pcre2_compile(
            reinterpret_cast<PCRE2_SPTR>(pattern.c_str()),
            PCRE2_ZERO_TERMINATED,
            options,
            &errornumber,
            &erroroffset,
            nullptr
        );
        
        if (code_ == nullptr) {
            // Get error message
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
            error_message_ = "PCRE2 compilation failed at offset " + 
                            std::to_string(erroroffset) + ": " + 
                            reinterpret_cast<char*>(buffer);
            return;
        }
        
        // Create match data
        match_data_ = pcre2_match_data_create_from_pattern(code_, nullptr);
        if (match_data_ == nullptr) {
            error_message_ = "Failed to create PCRE2 match data";
            pcre2_code_free(code_);
            code_ = nullptr;
        }
    }

    ~CompiledRegex() {
        cleanup();
    }
    
    // Non-copyable, movable
    CompiledRegex(const CompiledRegex&) = delete;
    CompiledRegex& operator=(const CompiledRegex&) = delete;
    
    CompiledRegex(CompiledRegex&& other) noexcept 
        : code_(other.code_), match_data_(other.match_data_), 
          error_message_(std::move(other.error_message_)) {
        other.code_ = nullptr;
        other.match_data_ = nullptr;
    }

    CompiledRegex& operator=(CompiledRegex&& other) noexcept {
        if (this != &other) {
            cleanup();
            code_ = other.code_;
            match_data_ = other.match_data_;
            error_message_ = std::move(other.error_message_);
            other.code_ = nullptr;
            other.match_data_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Match the pattern against input text
     * @param text Input text to match against
     * @return MatchResult with captured groups
     */
    MatchResult match(std::string_view text) const {
        MatchResult result;
        
        if (!is_valid()) {
            return result;
        }
        
        // Perform the match
        int rc = pcre2_match(
            code_,
            reinterpret_cast<PCRE2_SPTR>(text.data()),
            text.length(),
            0,      // start offset
            0,      // options
            match_data_,
            nullptr // match context
        );
        
        if (rc < 0) {
            // No match or error
            return result;
        }
        
        result.matched = true;
        
        // Extract named groups if they exist
        auto extract_group = [&](const char* name) -> std::string {
            PCRE2_UCHAR* buffer;
            PCRE2_SIZE bufflen;
            int ret = pcre2_substring_get_byname(match_data_, 
                                               reinterpret_cast<PCRE2_SPTR>(name), 
                                               &buffer, &bufflen);
            if (ret >= 0) {
                std::string result(reinterpret_cast<char*>(buffer), bufflen);
                pcre2_substring_free(buffer);
                return result;
            }
            return "";
        };
        
        // Extract the named groups we're interested in
        result.title = extract_group("title");
        result.enclosed = extract_group("enclosed");
        result.feat = extract_group("feat");
        result.artists = extract_group("artists");
        result.dash = extract_group("dash");
        result.comma = extract_group("comma");
        
        // If no named groups, try positional groups
        if (result.title.empty() && rc > 1) {
            PCRE2_UCHAR* buffer;
            PCRE2_SIZE bufflen;
            int ret = pcre2_substring_get_bynumber(match_data_, 1, &buffer, &bufflen);
            if (ret >= 0) {
                result.title = std::string(reinterpret_cast<char*>(buffer), bufflen);
                pcre2_substring_free(buffer);
            }
        }
        
        return result;
    }
    
    /**
     * @brief Check if pattern compilation was successful
     */
    bool is_valid() const { return code_ != nullptr; }
    
    /**
     * @brief Get compilation error message if any
     */
    const std::string& error_message() const { return error_message_; }
    
    /**
     * @brief Get the internal PCRE2 code pointer (for advanced operations)
     */
    pcre2_real_code_8* get_code() const { return code_; }

private:
    pcre2_real_code_8* code_;
    mutable pcre2_real_match_data_8* match_data_;
    std::string error_message_;
    
    void cleanup() {
        if (match_data_) {
            pcre2_match_data_free(match_data_);
            match_data_ = nullptr;
        }
        if (code_) {
            pcre2_code_free(code_);
            code_ = nullptr;
        }
    }
};

/**
 * @brief C++ port of the Python MetadataCleaner class
 * 
 * Provides methods for cleaning music metadata (recording and artist names)
 * by removing common "guff" (unnecessary metadata like remaster info, features, etc.)
 * while preserving essential information for matching purposes.
 */
class MetadataCleaner {
public:
    /**
     * @brief Construct a MetadataCleaner with specified script preference
     * @param preferred_script Unicode script name (e.g., "Latin", "Cyrillic")
     */
    explicit MetadataCleaner(const std::string& preferred_script = "Latin")
        : foreign_script_expression_("", false, true),  // Will be initialized below
          year_expression_(R"((20[0-9]{2}|19[0-9]{2}))", true, true),
          letter_expression_(R"(\p{L})", false, true) {
        
        initialize_recording_patterns();
        initialize_artist_patterns(); 
        initialize_foreign_script_pattern(preferred_script);
    }
    
    /**
     * @brief Clean a recording (track) name
     * @param text Input recording name
     * @return Cleaned recording name (may be unchanged if no cleaning needed)
     */
    std::string clean_recording(std::string_view text) {
        std::string cleaned;
        
        for (size_t i = 0; i < recording_expressions_.size(); ++i) {
            MatchResult match = recording_expressions_[i].match(text);
            
            if (match.matched) {
                bool is_guff_text = false;
                
                // Handle different patterns
                if (i == 0) {  // Pattern with enclosed content
                    if (!match.enclosed.empty()) {
                        // Remove surrounding brackets/parens for guff check
                        std::string paren_content = match.enclosed;
                        if (paren_content.length() >= 2) {
                            paren_content = paren_content.substr(1, paren_content.length() - 2);
                        }
                        
                        is_guff_text = is_paren_text_likely_guff(paren_content);
                        if (is_guff_text) {
                            cleaned = match.title;
                        } else {
                            continue;  // Keep looking for other patterns
                        }
                    }
                } else {
                    cleaned = match.title;
                }
                
                // Special check for hyphen split pattern (pattern index 2)
                if (i == 2) {
                    if (!hyphen_split_check(text, cleaned)) {
                        cleaned = std::string(text);
                    }
                }
                
                // Check parentheses balance
                if (!paren_checker(cleaned)) {
                    cleaned = std::string(text);
                }
                
                break;
            }
        }
        
        if (cleaned.empty()) {
            cleaned = std::string(text);
        }
        
        return drop_foreign_chars(cleaned);
    }
    
    /**
     * @brief Clean an artist name
     * @param text Input artist name  
     * @return Cleaned artist name (may be unchanged if no cleaning needed)
     */
    std::string clean_artist(std::string_view text) {
        // First try recording patterns
        std::string cleaned = clean_recording(text);
        if (cleaned != text) {
            return cleaned;
        }
        
        // Then try artist-specific patterns
        for (const auto& expr : artist_expressions_) {
            MatchResult match = expr.match(text);
            if (match.matched && !match.title.empty()) {
                return match.title;
            }
        }
        
        return std::string(text);
    }
    
    /**
     * @brief Remove characters from non-preferred Unicode scripts
     * @param text Input text
     * @return Text with foreign script characters removed, or original if no letters remain
     */
    std::string drop_foreign_chars(std::string_view text) {
        if (!foreign_script_expression_.is_valid()) {
            return std::string(text);
        }
        
        // Use PCRE2 substitution to remove foreign script characters
        PCRE2_SIZE output_length = text.length() * 2; // Start with enough space
        std::vector<PCRE2_UCHAR> output_buffer(output_length);
        
        int result = pcre2_substitute(
            foreign_script_expression_.get_code(),
            reinterpret_cast<PCRE2_SPTR>(text.data()),
            text.length(),
            0,                              // start offset
            PCRE2_SUBSTITUTE_GLOBAL,        // options - replace all matches
            nullptr,                        // match data (can be null for substitute)
            nullptr,                        // match context
            reinterpret_cast<PCRE2_SPTR>(""), // replacement string (empty)
            0,                              // replacement length (0 for empty string)
            output_buffer.data(),
            &output_length
        );
        
        if (result >= 0) {
            // Successfully substituted
            std::string remaining_text(reinterpret_cast<char*>(output_buffer.data()), output_length);
            
            // Trim whitespace
            remaining_text.erase(0, remaining_text.find_first_not_of(" \t\n\r\f\v"));
            remaining_text.erase(remaining_text.find_last_not_of(" \t\n\r\f\v") + 1);
            
            // Only return the remaining text if it still contains at least one letter
            MatchResult letter_match = letter_expression_.match(remaining_text);
            if (letter_match.matched && !remaining_text.empty()) {
                return remaining_text;
            }
        }
        
        // Fallback to original text if substitution failed or no letters remain
        return std::string(text);
    }

private:
    // Regex patterns for recording name cleaning
    std::vector<CompiledRegex> recording_expressions_;
    
    // Regex patterns for artist name cleaning  
    std::vector<CompiledRegex> artist_expressions_;
    
    // Pattern to match foreign script characters
    CompiledRegex foreign_script_expression_;
    
    // Pattern to match years (for guff detection)
    CompiledRegex year_expression_;
    
    // Pattern to match Unicode letters
    CompiledRegex letter_expression_;
    
    // String constants for symbol detection
    static inline const std::string SYMBOLS = "1234567890!@#$%^&*()-=_+[]{};\"|;'\\<>?/.,~`";
    
    /**
     * @brief Check if text in parentheses is likely "guff" (metadata noise)
     * @param paren_text Text found inside parentheses/brackets (without the brackets)
     * @return true if the text appears to be guff, false if it's likely meaningful
     */
    bool is_paren_text_likely_guff(std::string_view paren_text) {
        std::string text_lower;
        text_lower.reserve(paren_text.size());
        
        // Convert to lowercase
        std::transform(paren_text.begin(), paren_text.end(), 
                       std::back_inserter(text_lower),
                       [](char c) { return std::tolower(c); });
        
        size_t before_len = text_lower.length();
        
        // Remove guff words from text
        for (const auto& guff : GUFF_PAREN_WORDS) {
            size_t pos = 0;
            while ((pos = text_lower.find(guff, pos)) != std::string::npos) {
                text_lower.erase(pos, guff.length());
            }
        }
        
        // Remove years using the year regex pattern
        if (year_expression_.is_valid()) {
            PCRE2_SIZE output_length = text_lower.length() * 2;
            std::vector<PCRE2_UCHAR> output_buffer(output_length);
            
            int result = pcre2_substitute(
                year_expression_.get_code(),
                reinterpret_cast<PCRE2_SPTR>(text_lower.data()),
                text_lower.length(),
                0,                              // start offset
                PCRE2_SUBSTITUTE_GLOBAL,        // options - replace all matches
                nullptr,                        // match data 
                nullptr,                        // match context
                reinterpret_cast<PCRE2_SPTR>(""), // replacement string (empty)
                0,                              // replacement length (0 for empty string)
                output_buffer.data(),
                &output_length
            );
            
            if (result >= 0) {
                text_lower = std::string(reinterpret_cast<char*>(output_buffer.data()), output_length);
            }
        }
        
        size_t replaced = before_len - text_lower.length();
        
        // Calculate the number of "characters" vs "symbols" for what is leftover
        size_t chars = 0;
        size_t guff_chars = replaced;
        
        for (char ch : text_lower) {
            if (SYMBOLS.find(ch) != std::string::npos) {
                guff_chars++;
            }
        }
        
        // Count Unicode letters using the letter regex (handles Greek, etc.)
        if (letter_expression_.is_valid()) {
            PCRE2_SIZE offset = 0;
            while (offset < text_lower.length()) {
                pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(letter_expression_.get_code(), nullptr);
                
                int result = pcre2_match(
                    letter_expression_.get_code(),
                    reinterpret_cast<PCRE2_SPTR>(text_lower.data()),
                    text_lower.length(),
                    offset,
                    0,
                    match_data,
                    nullptr
                );
                
                if (result > 0) {
                    chars++;
                    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
                    offset = ovector[1]; // Move past this match
                } else {
                    pcre2_match_data_free(match_data);
                    break;
                }
                pcre2_match_data_free(match_data);
            }
        }
        
        return guff_chars > chars;
    }
    
    /**
     * @brief Check if cleaned text has properly balanced parentheses/brackets
     * @param text Text to check
     * @return true if all brackets are properly balanced
     */
    bool paren_checker(std::string_view text) {
        auto count_char = [text](char c) {
            return std::count(text.begin(), text.end(), c);
        };
        
        return (count_char('(') == count_char(')')) &&
               (count_char('<') == count_char('>')) &&
               (count_char('[') == count_char(']')) &&
               (count_char('{') == count_char('}'));
    }
    
    /**
     * @brief Check if hyphen-split result is reasonable (not too short)
     * @param original Original text
     * @param cleaned Cleaned text after hyphen split
     * @return true if cleaned text is reasonable length
     */
    bool hyphen_split_check(std::string_view original, std::string_view cleaned) {
        return cleaned.length() >= original.length() / 3;
    }
    
    /**
     * @brief Initialize regex patterns for recording name cleaning
     */
    void initialize_recording_patterns() {
        // Pattern 1: This track is crap (2018 remaster)
        // Matches text followed by content in parentheses, brackets, braces, or angle brackets
        recording_expressions_.emplace_back(
            R"((?P<title>.+?)\s+(?P<enclosed>\(.+\)|\[.+\]|\{.+\}|<.+>)$)",
            true, true
        );
        
        // Pattern 2: Featuring patterns
        // Tabula Rasa (feat. Lorraine Weiss)
        // TO STAY ALIVE [Feat. SkullyOSkully] 
        // For The Love feat. Amy True
        // For The Love ft. Amy True
        // For The Love ft Amy True
        recording_expressions_.emplace_back(
            R"((?P<title>.+?)\s+?(?P<feat>[\[\(]?(?:feat(?:uring)?|ft)\b\.?)\s*?(?P<artists>.+)\s*)",
            true, true
        );
        
        // Pattern 3: Dash separators
        // Don't Give up - 2001 remaster
        // Uses Unicode dash characters: U+2010-U+2014 (hyphens and dashes)
        // Also includes tilde ~, forward slash /, and regular hyphen -
        recording_expressions_.emplace_back(
            R"((?P<title>.+?)(?:\s+?[\x{2010}\x{2012}\x{2013}\x{2014}~/-])(?![^(]*\))(?P<dash>.*))",
            true, true
        );
    }
    
    /**
     * @brief Initialize regex patterns for artist name cleaning  
     */
    void initialize_artist_patterns() {
        // Pattern 1: Comma separators
        // Amy Winehouse, Weird Al Yankovic
        artist_expressions_.emplace_back(
            R"((?P<title>.+?)(?:\s*?,)(?P<comma>.*))",
            true, true
        );
        
        // Pattern 2: "with" and "&" separators  
        // Wilhelm Schneider Clauss with Heimersdorfer Kinderchor
        // Wilhelm Schneider Clauss & Heimersdorfer Kinderchor
        artist_expressions_.emplace_back(
            R"((?P<title>.+?)(?:\s+?(&|with))(?P<dash>.*))",
            true, true
        );
    }
    
    /**
     * @brief Create foreign script regex based on preferred script
     * @param preferred_script Name of preferred Unicode script
     */
    void initialize_foreign_script_pattern(const std::string& preferred_script) {
        // Create a pattern that matches characters NOT in Common or preferred script
        // This is equivalent to Python's regex: r"[^\p{Script=Common}\p{Script=" + preferred_script + r"}]+"
        std::string pattern = R"([^\p{Common}\p{)" + preferred_script + R"(}]+)";
        foreign_script_expression_ = CompiledRegex(pattern, false, true);
    }
};

} // namespace lb_matching_tools