//
// Created by Victor Varian on 27/1/21.
//

#include <string>
#include "StatementRef.h"

using namespace PQLParser;

StatementRef::StatementRef() {}

StatementRef::~StatementRef() {

}


void StatementRef::set_type(StatementRefType statementRefType) {
    this->type = statementRefType;
}

void StatementRef::set_synonym(std::string synonym) {
    this->synonym = synonym;
}

void StatementRef::set_statement_number(int statement_number) {
    this->statement_number = statement_number;
}

StatementRefType StatementRef::get_type() {
    return type;
}

std::string StatementRef::get_synonym() {
    if (type != StatementRefType::SYNONYM) {
        throw 'StatementRefType is not SYNONYM type';
    }

    return synonym;
}

int StatementRef::get_statement_number() {
    if (type != StatementRefType::STATEMENT_NUMBER) {
        throw 'StatementRefType is not STATEMENT_NUMBER type';
    }

    return statement_number;
}