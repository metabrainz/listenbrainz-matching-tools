#include <iostream>
#include "lb_matching_tools/cleaner.hpp"

int main() {
    std::cout << "ListenBrainz Matching Tools C++ Example\n";
    std::cout << "========================================\n\n";
    
    // Show guff words
    std::cout << "Number of guff words: " << lb_matching_tools::GUFF_PAREN_WORDS.size() << "\n";
    std::cout << "First few guff words:\n";
    
    for (size_t i = 0; i < std::min(size_t(10), lb_matching_tools::GUFF_PAREN_WORDS.size()); ++i) {
        std::cout << "  " << lb_matching_tools::GUFF_PAREN_WORDS[i] << "\n";
    }
    
    std::cout << "\n";
    
    // Demonstrate metadata cleaning
    try {
        lb_matching_tools::MetadataCleaner cleaner;
        
        // Test recordings
        std::vector<std::string> test_recordings = {
            "Hotel California (2013 Remaster)",
            "Bohemian Rhapsody - Live at Wembley",  
            "Sweet Child O' Mine (feat. Slash)",
            "Stairway to Heaven [Remastered]",
            "Imagine (Original Version)"
        };
        
        std::cout << "Recording Cleaning Examples:\n";
        std::cout << "----------------------------\n";
        for (const auto& recording : test_recordings) {
            std::string cleaned = cleaner.clean_recording(recording);
            std::cout << "Original: \"" << recording << "\"\n";
            std::cout << "Cleaned:  \"" << cleaned << "\"\n\n";
        }
        
        // Test artists  
        std::vector<std::string> test_artists = {
            "The Beatles, Paul McCartney",
            "Led Zeppelin & Jimmy Page", 
            "Queen with David Bowie"
        };
        
        std::cout << "Artist Cleaning Examples:\n";
        std::cout << "-------------------------\n";
        for (const auto& artist : test_artists) {
            std::string cleaned = cleaner.clean_artist(artist);
            std::cout << "Original: \"" << artist << "\"\n";
            std::cout << "Cleaned:  \"" << cleaned << "\"\n\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}