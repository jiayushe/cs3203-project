#include "AssignmentMap.h"

using namespace QueryEvaluator;

std::size_t AssignmentMapHash::operator()(const AssignmentMap& assignment_map) const {
    // TODO: Verify the quality of this hash function
    std::hash<std::string> string_hash;
    AssignmentHash assignment_hash;
    std::size_t result = 0;
    for (auto const& entry : assignment_map) {
        result ^= string_hash(entry.first) + assignment_hash(entry.second);
    }
    return result;
}
