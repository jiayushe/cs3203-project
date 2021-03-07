#pragma once

#include "AttrRef.h"
#include <string>

namespace Parser {

enum class WithRefType { INVALID, NAME, STATEMENT_NUMBER, SYNONYM, ATTR_REF };

class WithRef {
public:
    WithRef();

    WithRefType get_type() const;
    std::string get_name() const;
    int get_statement_number() const;
    std::string get_synonym() const;
    AttrRef get_attr_ref() const;

    void set_type(WithRefType type);
    void set_name(const std::string& name);
    void set_statement_number(int statement_number);
    void set_synonym(const std::string& synonym);
    void set_attr_ref(const AttrRef& attr_ref);

private:
    WithRefType type;

    std::string name;
    int statement_number;
    std::string synonym;
    AttrRef attr_ref;
};

} // namespace Parser
