#pragma once

#include "Models.hpp"
#include <string>
#include <fstream>
#include <stdexcept>

class MemoryStatReader {
    MemoryStats memory_stats = {};
public:
    void Read();
    MemoryStats GetMemoryStats() const;
};