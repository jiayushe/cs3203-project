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

bool StatementRef::operator==(const StatementRef& other) const {
    return type == other.type && synonym == other.synonym && statement_id == other.statement_id;
}

std::size_t StatementRefHash::operator()(const StatementRef& statement_ref) const {
    std::hash<int> int_hash;
    std::hash<std::string> string_hash;
    switch (statement_ref.get_type()) {
    case StatementRefType::INVALID:
        return int_hash(1);
    case StatementRefType::ANY:
        return int_hash(2);
    case StatementRefType::STATEMENT_ID:
        return int_hash(3) ^ int_hash(statement_ref.get_statement_id());
    case StatementRefType::SYNONYM:
        return int_hash(4) ^ string_hash(statement_ref.get_synonym());
    default:
        throw std::runtime_error("Unknown statement ref type");
    }
}