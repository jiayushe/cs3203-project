#pragma once

#include "Assignment.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace QueryEvaluator {

// Denotes a group of assignments, i.e. a mapping of synonym -> Assignment.
//
// For example, given the source:
// ```
// procedure proc {
//   read x;
//   x = x + 1;
// }
// ```
//
// And the query:
// ```
// stmt s; assign a;
// Select s pattern a(_, _)
// ```
//
// Some possible assignment maps include:
// - {"s" -> 1, "a" -> 2}
// - {"s" -> 2, "a" -> 2}
typedef std::unordered_map<std::string, Assignment> AssignmentMap;

class AssignmentMapHash {
public:
    std::size_t operator()(const AssignmentMap& assignment_map) const;
};

} // namespace QueryEvaluator
