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

#include <jpcre2.hpp>

// Type aliases for JPCRE2
using jpc = jpcre2::select<char>;

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
 * @brief JPCRE2-based wrapper for compiled regex patterns
 */
class CompiledRegex {
public:
    // Default constructor for member variables
    CompiledRegex() : valid_(false) {}
    
    explicit CompiledRegex(const std::string& pattern, bool case_insensitive = true, bool utf8 = true) {
        try {
            // Set up compile options using string modifiers
            std::string modifiers;
            if (case_insensitive) modifiers += "i";
            if (utf8) modifiers += "u"; // UTF-8 mode
            
            // Create and compile the regex
            regex_.setPattern(pattern);
            if (!modifiers.empty()) {
                regex_.addModifier(modifiers);
            }
            regex_.compile();
            
            valid_ = true;
        } catch (const std::exception& e) {
            error_message_ = std::string("JPCRE2 compilation failed: ") + e.what();
            valid_ = false;
        }
    }

    ~CompiledRegex() = default;
    
    // Non-copyable, movable
    CompiledRegex(const CompiledRegex&) = delete;
    CompiledRegex& operator=(const CompiledRegex&) = delete;
    
    CompiledRegex(CompiledRegex&&) = default;
    CompiledRegex& operator=(CompiledRegex&&) = default;
    
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
        
        try {
            // Create match object and perform the match
            jpc::RegexMatch rm;
            rm.setRegexObject(&regex_);
            std::string text_str(text); // JPCRE2 needs std::string
            
            // Set up vectors to capture substrings
            jpc::VecNum vec_num;
            jpc::VecNas vec_nas;
            rm.setNumberedSubstringVector(&vec_num)
              .setNamedSubstringVector(&vec_nas)
              .setSubject(text_str);
            
            size_t match_count = rm.match(); // Perform the match
            
            if (match_count > 0) {
                result.matched = true;
                
                // Extract named groups if they exist  
                auto extract_group = [&](const std::string& name) -> std::string {
                    // vec_nas is a vector of maps, check each map for the named group
                    for (const auto& group_map : vec_nas) {
                        auto it = group_map.find(name);
                        if (it != group_map.end()) {
                            return it->second;
                        }
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
                if (result.title.empty() && !vec_num.empty() && vec_num[0].size() > 1) {
                    result.title = vec_num[0][1]; // vec_num[0][1] is the first capture group
                }
            }
        } catch (const std::exception&) {
            // Match failed
        }
        
        return result;
    }
    
    /**
     * @brief Check if pattern compilation was successful
     */
    bool is_valid() const { return valid_; }
    
    /**
     * @brief Get compilation error message if any
     */
    const std::string& error_message() const { return error_message_; }
    
    /**
     * @brief Get the internal JPCRE2 regex object (for advanced operations)
     */
    const jpc::Regex& get_regex() const { return regex_; }
    
    /**
     * @brief Substitute all matches with replacement text
     */
    std::string substitute(const std::string& text, const std::string& replacement = "") const {
        if (!is_valid()) {
            return text;
        }
        
        try {
            jpc::RegexReplace rr;
            rr.setRegexObject(&regex_)
              .setReplaceWith(replacement)
              .setSubject(text);
              
            return rr.replace();
        } catch (const std::exception&) {
            return text;
        }
    }

private:
    jpc::Regex regex_;
    bool valid_ = false;
    std::string error_message_;
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
        
        // Use JPCRE2 substitution to remove foreign script characters
        std::string text_str(text);
        std::string remaining_text = foreign_script_expression_.substitute(text_str, "");
        
        // Trim whitespace
        remaining_text.erase(0, remaining_text.find_first_not_of(" \t\n\r\f\v"));
        remaining_text.erase(remaining_text.find_last_not_of(" \t\n\r\f\v") + 1);
        
        // Only return the remaining text if it contains substantial content
        // and is significantly shorter than the original (meaning foreign chars were actually removed)
        MatchResult letter_match = letter_expression_.match(remaining_text);
        if (letter_match.matched && !remaining_text.empty()) {
            // If the cleaned text is significantly shorter, it means foreign chars were removed
            // Use cleaned version only if meaningful content remains and some removal occurred
            size_t original_len = text_str.length();
            size_t remaining_len = remaining_text.length();
            
            if (original_len > 0 && remaining_len > 0) {
                // If more than 40% was removed, likely foreign script removal - use cleaned version
                // If less than 40% was removed, likely mixed or primarily foreign - keep original
                double removal_ratio = (double)(original_len - remaining_len) / original_len;
                if (removal_ratio >= 0.4) {
                    return remaining_text;
                }
            }
        }
        
        // Fallback to original text if substitution removed too much content or no letters remain
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
            text_lower = year_expression_.substitute(text_lower, "");
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
            try {
                jpc::RegexMatch rm;
                rm.setRegexObject(&letter_expression_.get_regex())
                  .setSubject(text_lower)
                  .setFindAll(); // Find all matches
                
                // Count matches (each match represents a letter)
                size_t match_count = rm.match();
                chars = match_count;
            } catch (const std::exception&) {
                // Fallback: count any characters that might be letters
                for (char ch : text_lower) {
                    if (std::isalpha(ch)) {
                        chars++;
                    }
                }
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