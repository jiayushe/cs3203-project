#include "Assignment.h"
#include <stdexcept>

using namespace QueryEvaluator;

Assignment::Assignment() : type(AssignmentType::INVALID) {}

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

std::string Assignment::value_as_string() const {
    // FIXME: Don't use value_as_string anymore
    switch (type) {
    case AssignmentType::INTEGER:
        return std::to_string(int_value);
    case AssignmentType::STRING:
        return string_value;
    case AssignmentType::BOTH:
        return std::to_string(int_value);
    default:
        throw std::runtime_error("Unknown assignment type");
    }
}
