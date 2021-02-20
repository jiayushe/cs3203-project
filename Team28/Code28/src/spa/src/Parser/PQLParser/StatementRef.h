#pragma once

#include <string>

namespace Parser {
enum class StatementRefType {
    INVALID,
    ANY,
    SYNONYM,
    STATEMENT_ID,
};

class StatementRef {
public:
    StatementRefType get_type() const;
    std::string get_synonym() const;
    int get_statement_id() const;

    StatementRef();

    void set_type(StatementRefType statement_ref_type);
    void set_synonym(const std::string& synonym);
    void set_statement_id(int statement_id);

private:
    StatementRefType type;
    std::string synonym;
    int statement_id;
};

} // namespace Parser
