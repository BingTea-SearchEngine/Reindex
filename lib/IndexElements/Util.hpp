#pragma once

#include <fcntl.h>     // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <sys/resource.h>
#include <unistd.h>    // For ftruncate, close
#include <algorithm>
#include <utility>

#include <stdexcept>
#include <string>

std::string strip_utf8_spaces(const std::string& input);

bool is_ascii(const std::string& word);
/*
 * @brief Create a memory mapped region
 *
 * */
void* create_mmap_region(int& fd, size_t size, std::string filename);

std::pair<void*, size_t> read_mmap_region(int& fd, std::string filename);

long getBytesUsed();
