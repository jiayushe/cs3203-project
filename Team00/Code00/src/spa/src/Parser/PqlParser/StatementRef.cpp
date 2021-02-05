#include <string>
#include "StatementRef.h"

using namespace Parser;

StatementRef::StatementRef() {}

StatementRef::~StatementRef() {

}


void StatementRef::set_type(StatementRefType statement_ref_type) {
    this->type = statement_ref_type;
}

void StatementRef::set_synonym(std::string synonym) {
    this->synonym = synonym;
}

void StatementRef::set_statement_id(int statement_id) {
    this->statement_id = statement_id;
}

StatementRefType StatementRef::get_type() {
    return type;
}

std::string StatementRef::get_synonym() {
    if (type != StatementRefType::SYNONYM) {
        throw "StatementRefType is not SYNONYM type";
    }

    return synonym;
}

int StatementRef::get_statement_id() {
    if (type != StatementRefType::STATEMENT_ID) {
        throw "StatementRefType is not STATEMENT_ID type";
    }

    return statement_id;
}