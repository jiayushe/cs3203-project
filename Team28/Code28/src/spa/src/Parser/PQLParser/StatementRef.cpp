#include "StatementRef.h"
#include <stdexcept>
#include <string>

using namespace Parser;

StatementRef::StatementRef() : type(StatementRefType::INVALID), statement_id(-1) {}

void StatementRef::set_type(StatementRefType new_type) { type = new_type; }

void StatementRef::set_synonym(const std::string& new_synonym) { synonym = new_synonym; }

void StatementRef::set_statement_id(int new_statement_id) { statement_id = new_statement_id; }

StatementRefType StatementRef::get_type() const { return type; }

std::string StatementRef::get_synonym() const {
    if (type != StatementRefType::SYNONYM) {
        throw std::runtime_error("StatementRefType is not SYNONYM type");
    }
    return synonym;
}

int StatementRef::get_statement_id() const {
    if (type != StatementRefType::STATEMENT_ID) {
        throw std::runtime_error("StatementRefType is not STATEMENT_ID type");
    }
    return statement_id;
}