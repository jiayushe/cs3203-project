#include "Result.h"
#include <stdexcept>

using namespace Parser;

Result::Result() : type(ResultType::INVALID) {}

void Result::set_type(ResultType new_type) { type = new_type; }

ResultType Result::get_type() const { return type; }

void Result::set_tuple(const std::vector<Elem>& new_tuple) { tuple = new_tuple; }

std::vector<Elem> Result::get_tuple() const {
    if (type != ResultType::TUPLE) {
        throw std::runtime_error("ResultType is not TUPLE type");
    }
    return tuple;
}