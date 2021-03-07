#pragma once

#include <string>

namespace Parser {

class AttrRef {
public:
    AttrRef();

    std::string get_synonym() const;
    std::string get_attr_name() const;

    void set_synonym(const std::string& synonym);
    void set_attr_name(const std::string& attr_name);

private:
    std::string synonym;
    std::string attr_name;
};

} // namespace Parser
