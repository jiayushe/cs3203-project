#pragma once

#include "AttrRef.h"
#include <string>
#include <vector>

namespace Parser {

enum class ElemType { INVALID, SYNONYM, ATTR_REF };

class Elem {
public:
    explicit Elem(std::string synonym);
    explicit Elem(AttrRef attr_ref);

    ElemType get_type() const;
    std::string get_synonym() const;
    AttrRef get_attr_ref() const;

private:
    ElemType type;
    std::string synonym;
    AttrRef attr_ref;
};

} // namespace Parser
