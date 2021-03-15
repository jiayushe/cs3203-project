#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace QueryEvaluator {

enum class AssignmentType { INVALID, STRING, INTEGER, BOTH };

// Denotes a single assignment for a given synonym.
//
// For example, given the SIMPLE source:
// ```
// procedure proc {
//   read x;
//   print y;
// }
// ```
//
// And the query:
// ```
// stmt s; Select s
// ```
//
// Some possible assignments for the synonym `s` include `1` and `2`.
// This is because there are only 2 statements in the source, and the
// synonym `s` refers to any type of statements.
class Assignment {
public:
    Assignment();
    explicit Assignment(std::string value);
    explicit Assignment(int value);
    explicit Assignment(int value, std::string string_value);

    AssignmentType get_type() const;
    std::string get_string_value() const;
    int get_int_value() const;

    bool operator==(const Assignment& other) const;

private:
    AssignmentType type;
    std::string string_value;
    int int_value;
};

class AssignmentHash {
public:
    std::size_t operator()(const Assignment& assignment) const;
};

} // namespace QueryEvaluator
