#pragma once

#include <string>
#include <unordered_map>

namespace QueryEvaluator {

enum class AssignmentType { INVALID, STRING, INTEGER };

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

    AssignmentType get_type() const;
    std::string get_string_value() const;
    int get_int_value() const;

    std::string value_as_string() const;

private:
    AssignmentType type;
    std::string synonym;
    std::string string_value;
    int int_value;
};

// Denotes a group of assignments, i.e. synonym -> Assignment.
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

} // namespace QueryEvaluator
