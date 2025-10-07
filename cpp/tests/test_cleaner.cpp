#include <gtest/gtest.h>
#include "lb_matching_tools/cleaner.hpp"

using namespace lb_matching_tools;

class CleanerTest : public ::testing::Test {
protected:
    void SetUp() override {
        cleaner = std::make_unique<MetadataCleaner>();
    }
    
    std::unique_ptr<MetadataCleaner> cleaner;
};

// Artist test cases ported from Python
class ArtistParameterizedTest : public CleanerTest, 
                                public ::testing::WithParamInterface<std::pair<std::string, std::string>> {
};

TEST_P(ArtistParameterizedTest, CleanArtistCases) {
    const auto& [input, expected] = GetParam();
    std::string result = cleaner->clean_artist(input);
    EXPECT_EQ(result, expected) << "Input: '" << input << "'";
}

INSTANTIATE_TEST_SUITE_P(ArtistTests, ArtistParameterizedTest, 
    ::testing::Values(
        std::make_pair("Amy Winehouse, Weird Al", "Amy Winehouse"),
        std::make_pair("((( Punchline Paradise )))", "((( Punchline Paradise )))"),
        std::make_pair("Dan No Ura 壇ノ浦の戦い", "Dan No Ura"),
        std::make_pair("4 Dan No Ura 壇ノ浦の戦い", "4 Dan No Ura"),
        std::make_pair("Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena", "Babylumalotoroony and the Jerry Lewis Bone-A-Thons"),
        std::make_pair("Wilhelm Schneider-Clauss & Heimersdorfer Kinderchor", "Wilhelm Schneider-Clauss"),
        std::make_pair("Jim Foster with The Kountry Kut-Ups", "Jim Foster")
    )
);

// Recording test cases ported from Python  
class RecordingParameterizedTest : public CleanerTest,
                                  public ::testing::WithParamInterface<std::pair<std::string, std::string>> {
};

TEST_P(RecordingParameterizedTest, CleanRecordingCases) {
    const auto& [input, expected] = GetParam();
    std::string result = cleaner->clean_recording(input);
    EXPECT_EQ(result, expected) << "Input: '" << input << "'";
}

INSTANTIATE_TEST_SUITE_P(RecordingTests, RecordingParameterizedTest,
    ::testing::Values(
        // Basic cases
        std::make_pair("Nothing fancy about this!", "Nothing fancy about this!"),
        std::make_pair("Birds Without a Feather", "Birds Without a Feather"),
        std::make_pair("Amy Winehouse, Weird Al", "Amy Winehouse, Weird Al"),
        
        // Featuring patterns
        std::make_pair("Tabula Rasa (feat. Lorraine Weiss)", "Tabula Rasa"),
        std::make_pair("For The Love feat. Amy True", "For The Love"),
        std::make_pair("For The Love ft. Amy True", "For The Love"),
        std::make_pair("For The Love ft Amy True", "For The Love"),
        std::make_pair("For The Lovefeat. Amy True", "For The Lovefeat. Amy True"),  // Should not match
        std::make_pair("For The Love feat.Amy True", "For The Love"),
        std::make_pair("TO STAY ALIVE [Feat. SkullyOSkully]", "TO STAY ALIVE"),
        
        // Dash patterns
        std::make_pair("Don't Give up - 2001 remaster", "Don't Give up"),
        std::make_pair("Define Self-delusion at Dictionary.Com", "Define Self-delusion at Dictionary.Com"),
        std::make_pair("Running up that hill (a deal with god) - 2018 Remaster", "Running up that hill (a deal with god)"),
        std::make_pair("Herbert West – Reanimator", "Herbert West"),  // en dash
        std::make_pair("Philip the Chancelor: Clavus pungens — 2vv conductus", "Philip the Chancelor: Clavus pungens"), // em dash
        std::make_pair("\"Beckoning Darkness\" ~ Menu", "\"Beckoning Darkness\""),
        std::make_pair("Big Phat at Glen Echo, MD Contradance / CALLER: Ridge Kennedy", "Big Phat at Glen Echo, MD Contradance"),
        
        // Parenthetical content (guff detection)
        std::make_pair("!!ESIRPRUS (Ver. 2)", "!!ESIRPRUS"),
        std::make_pair("Amoeba (raft boy)", "Amoeba (raft boy)"),  // Should be kept (not guff)
        std::make_pair("Other Place [Live]", "Other Place"),
        std::make_pair("Other Place {Live}", "Other Place"),  
        std::make_pair("Other Place <Live>", "Other Place"),
        std::make_pair("Other Place [boo - hiss]", "Other Place [boo - hiss]"),  // Should be kept
        std::make_pair("Other Place {boo - hiss}", "Other Place {boo - hiss}"),  // Should be kept
        std::make_pair("Other Place <boo - hiss>", "Other Place <boo - hiss>"),  // Should be kept
        std::make_pair("Other Place [2028 - remaster]", "Other Place"),
        std::make_pair("Other Place {2018 - release}", "Other Place"),
        std::make_pair("Other Place <1985 - tryout>", "Other Place"),
        std::make_pair("this random text (quite worthy!)", "this random text (quite worthy!)"), // Should be kept
        std::make_pair("this random text (released 2018)", "this random text"),
        std::make_pair("this random text (demo session)", "this random text"),
        std::make_pair("this random text (tryout tape)", "this random text"),
        std::make_pair("I Miss You (Howie B mix)", "I Miss You (Howie B mix)"),  // Should be kept 
        std::make_pair("Run Jozi (Godly)", "Run Jozi (Godly)"),  // Should be kept
        
        // Special cases
        std::make_pair("\"...as others see us...\": I. Henry VIII (1491-1547)", "\"...as others see us...\": I. Henry VIII"),
        std::make_pair("T - Ball Rag", "T - Ball Rag"),  // Should not be split (too short result)
        std::make_pair(")--- ---x--- ---(", ")--- ---x--- ---("),  // Should remain unchanged
        std::make_pair("[intentionally left blank]", "[intentionally left blank]"),  // Should remain unchanged
        std::make_pair("Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena", "Babylumalotoroony and the Jerry Lewis Bone-A-Thons"),
        std::make_pair("Guff (Johann Sebastian - Bach)", "Guff (Johann Sebastian - Bach)"), // Should not be split (dash inside parens)
        std::make_pair("-10 on the Care-Meter", "-10 on the Care-Meter"),  // Should not be split
        std::make_pair("264 - Das Herz", "264 - Das Herz"),  // Should not be split (too short result)
        
        // Complex parenthetical cases
        std::make_pair("(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural", 
                      "(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural"), // Dash inside parens, should not split
        
        // Unicode cases  
        std::make_pair("Kikagaku Moyo/幾何学模様", "Kikagaku Moyo/"),  // Foreign script removal
        std::make_pair("山地情歌 San-Di Love Song", "San-Di Love Song"),  // Foreign script removal
        std::make_pair("Madness (DJ Gollum feat. DJ Cap Remix)", "Madness (DJ Gollum feat. DJ Cap Remix)"),  // Complex feat in parens
        std::make_pair("Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)", "Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)"),  // Greek, should be kept
        std::make_pair("Όσο Και Να Σ' Αγαπάω (2018 remaster)", "Όσο Και Να Σ' Αγαπάω"),  // Greek with guff
        std::make_pair("Όσο Και Να Σ' Αγαπάω - Remix 2023", "Όσο Και Να Σ' Αγαπάω")  // Greek with dash
    )
);

// Individual tests for special functionality
TEST_F(CleanerTest, Constructor) {
    // Test that constructor doesn't throw
    EXPECT_NO_THROW(MetadataCleaner cleaner);
    EXPECT_NO_THROW(MetadataCleaner cleaner("Cyrillic"));
    EXPECT_NO_THROW(MetadataCleaner cleaner("Greek"));
}

TEST_F(CleanerTest, EmptyInputs) {
    EXPECT_EQ(cleaner->clean_recording(""), "");
    EXPECT_EQ(cleaner->clean_artist(""), "");
}

TEST_F(CleanerTest, DropForeignChars) {
    // Basic test - this functionality is simplified in current implementation
    std::string latin_input = "Test Title";
    std::string result = cleaner->drop_foreign_chars(latin_input);
    EXPECT_FALSE(result.empty());
}

TEST_F(CleanerTest, GuffWords) {
    // Test that guff words are properly loaded
    EXPECT_FALSE(GUFF_PAREN_WORDS.empty());
    EXPECT_GT(GUFF_PAREN_WORDS.size(), 50);  // Should have many guff words
    
    // Check for some known guff words
    const auto& guff = GUFF_PAREN_WORDS;
    EXPECT_NE(std::find(guff.begin(), guff.end(), "remaster"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "acoustic"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "live"), guff.end());
    EXPECT_NE(std::find(guff.begin(), guff.end(), "remix"), guff.end());
}