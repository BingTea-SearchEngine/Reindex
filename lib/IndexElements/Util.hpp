#pragma once

#include <fcntl.h>  // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close

#include <stdexcept>
#include <string>

/*
 * @brief Create a memory mapped region
 *
 * */
void* create_mmap_region(int& fd, size_t size, std::string filename);

void* read_mmap_region(int& fd, size_t size, std::string filename);
