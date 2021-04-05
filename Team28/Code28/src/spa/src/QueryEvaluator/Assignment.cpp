#include "Assignment.h"

using namespace QueryEvaluator;

Assignment::Assignment() : type(AssignmentType::INVALID), int_value(-1) {}

Assignment::Assignment(int value) : type(AssignmentType::INTEGER), int_value(value) {}

Assignment::Assignment(std::string value)
    : type(AssignmentType::STRING), string_value(std::move(value)), int_value(-1) {}

Assignment::Assignment(int value, std::string string_value)
    : type(AssignmentType::BOTH), string_value(std::move(string_value)), int_value(value) {}

AssignmentType Assignment::get_type() const { return type; }

std::string Assignment::get_string_value() const {
    if (type == AssignmentType::INTEGER) {
        throw std::runtime_error("Cannot get string value for non-string assignment");
    }
    return string_value;
}

int Assignment::get_int_value() const {
    if (type == AssignmentType::STRING) {
        throw std::runtime_error("Cannot get int value for non-int assignment");
    }
    return int_value;
}

bool Assignment::operator==(const Assignment& other) const {
    return type == other.type && string_value == other.string_value && int_value == other.int_value;
}

std::size_t AssignmentHash::operator()(const Assignment& assignment) const {
    std::hash<int> int_hash;
    std::hash<std::string> string_hash;
    switch (assignment.get_type()) {
    case AssignmentType::INTEGER:
        return int_hash(1) ^ int_hash(assignment.get_int_value());
    case AssignmentType::STRING:
        return int_hash(2) ^ string_hash(assignment.get_string_value());
    case AssignmentType::BOTH:
        return int_hash(3) ^ int_hash(assignment.get_int_value()) ^
               string_hash(assignment.get_string_value());
    default:
        throw std::runtime_error("Unhandled assignment type");
    }
}
