#pragma once

#include "Elem.h"
#include <vector>

namespace Parser {

enum class ResultType { INVALID, TUPLE, BOOLEAN };

class Result {
public:
    Result();

    ResultType get_type() const;
    std::vector<Elem> get_tuple() const;

    void set_type(ResultType result_type);
    void set_tuple(const std::vector<Elem>& tuple);

private:
    ResultType type;
    std::vector<Elem> tuple;
};

} // namespace Parser
