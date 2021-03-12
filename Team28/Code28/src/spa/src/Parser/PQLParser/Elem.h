#pragma once

#include "AttrRef.h"
#include <string>
#include <vector>

namespace Parser {

enum class ElemType { INVALID, SYNONYM, ATTR_REF };

class Elem {
public:
    Elem();

    ElemType get_type() const;
    std::string get_synonym() const;
    AttrRef get_attr_ref() const;

    void set_synonym(std::string synonym);
    void set_type(ElemType type);
    void set_attr_ref(AttrRef attr_ref);

private:
    ElemType type;
    std::string synonym;
    AttrRef attr_ref;
};

} // namespace Parser
