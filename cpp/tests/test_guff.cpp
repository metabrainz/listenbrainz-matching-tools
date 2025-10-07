#include <gtest/gtest.h>
#include "lb_matching_tools/guff.hpp"

using namespace lb_matching_tools;

TEST(GuffTest, GuffWordsNotEmpty) {
    EXPECT_FALSE(GUFF_PAREN_WORDS.empty());
    EXPECT_GT(GUFF_PAREN_WORDS.size(), 50);  // Should have many guff words
}

TEST(GuffTest, ContainsExpectedWords) {
    // Check for some known guff words
    const auto& guff = GUFF_PAREN_WORDS;
    
    EXPECT_NE(std::find(guff.begin(), guff.end(), "remaster"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "acoustic"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "live"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "remix"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "demo"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "instrumental"), guff.end());
}

TEST(GuffTest, AllWordsLowerCase) {
    // All guff words should be lowercase for proper matching
    for (const auto& word : GUFF_PAREN_WORDS) {
        std::string lower_word = word;
        std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
        EXPECT_EQ(word, lower_word) << "Guff word '" << word << "' should be lowercase";
    }
}