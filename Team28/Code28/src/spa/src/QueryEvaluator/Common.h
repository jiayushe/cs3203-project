#pragma once

#include "Assignment.h"
#include "AssignmentMap.h"
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace QueryEvaluator {

// A forward declaration of the BinaryConstraint class, to prevent cyclical imports.
class BinaryConstraint;

typedef std::unordered_set<Assignment, AssignmentHash> Domain;
typedef std::unordered_map<std::string, Domain> DomainMap;
typedef std::unordered_set<AssignmentMap, AssignmentMapHash> AssignmentMapSet;
typedef std::vector<AssignmentMap> AssignmentMapVector;
typedef std::tuple<std::unordered_set<std::string>, DomainMap, std::vector<BinaryConstraint>>
    AssignmentGroup;

} // namespace QueryEvaluator
