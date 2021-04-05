#include "AssignmentMap.h"

using namespace QueryEvaluator;

// Inspired by boost's hash_combine
// https://github.com/boostorg/container_hash/blob/171c012d4723c5e93cc7cffe42919afdf8b27dfa/include/boost/container_hash/hash.hpp#L310-L314
std::size_t AssignmentMapHash::operator()(const AssignmentMap& assignment_map) const {
    std::hash<std::string> string_hash;
    AssignmentHash assignment_hash;
    std::size_t result = 0;
    for (auto const& entry : assignment_map) {
        result ^= (string_hash(entry.first) ^ assignment_hash(entry.second)) + 0x9e3779b9 +
                  (result << 6) + (result >> 2);
    }
    return result;
}
