#ifndef GENRES_H
#define GENRES_H

#include <cstddef>
#include <cstdint>
#include <vector>


extern const char* const kGenreNames[];
extern const std::size_t kGenreCount;

const char* GenreName(std::uint16_t index);
std::vector<std::uint16_t> SortedGenreIndices();


#endif
