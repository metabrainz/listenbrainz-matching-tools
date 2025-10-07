# ListenBrainz Matching Tools - C++ Port

This is a **header-only C++17** library that provides music metadata cleaning functionality, ported from the original Python implementation.

## Features

- 🎯 **Header-only**: No shared libraries needed - just include and use
- ⚡ **High Performance**: C++17 with PCRE2 regex for fast text processing
- 🌍 **Unicode Support**: Handles international characters (Greek, Cyrillic, etc.)
- 🧹 **Smart Cleaning**: Removes metadata noise while preserving meaningful content
- ✅ **Thoroughly Tested**: 63 comprehensive tests covering real-world scenarios
- 🔧 **Easy Integration**: Simple CMake setup for testing and examples

## Quick Start

### Prerequisites
- C++17 compatible compiler
- PCRE2 library: `sudo apt install libpcre2-dev` (Ubuntu/Debian)
- CMake 3.15+ (for building tests/examples)

### Build and Test
```bash
mkdir build && cd build
cmake ..
cmake --build .
./tests/lb_matching_tests        # Run all 63 tests
./examples/simple_example        # See it in action
```

### Usage in Your Project
```cpp
#include <lb_matching_tools/cleaner.hpp>

int main() {
    lb_matching_tools::MetadataCleaner cleaner;
    
    // Clean recording titles (removes guff like remasters, versions)
    std::string cleaned = cleaner.clean_recording("Hotel California (2013 Remaster)");
    // Result: "Hotel California"
    
    // Clean artist names (removes collaborator info)
    std::string artist = cleaner.clean_artist("The Beatles feat. Paul McCartney");
    // Result: "The Beatles"
    
    return 0;
}
```

## Integration

### Option 1: Copy Headers
1. Copy `include/lb_matching_tools/` to your project
2. Link against PCRE2 in your build system
3. Include the headers and use

### Option 2: Use CMake
```cmake
# Add as subdirectory or use FetchContent
add_subdirectory(path/to/lb_matching_tools)
target_link_libraries(your_target PRIVATE lb_matching_tools)
```

## What It Cleans

- **Recording titles**: Removes remaster info, version tags, guff words
- **Artist names**: Removes collaborator indicators, featuring text
- **Unicode handling**: Properly processes international text
- **Smart detection**: Preserves meaningful parenthetical content

## Files Structure

```
cpp/
├── CMakeLists.txt              # Main build configuration
├── include/
│   └── lb_matching_tools/
│       ├── cleaner.hpp         # Main cleaning functionality
│       └── guff.hpp           # Guff words data
├── tests/                      # Comprehensive test suite
├── examples/                   # Usage examples
└── README.md                  # This file
```

**Essential files for integration:** Just the `include/` directory and PCRE2 dependency.
