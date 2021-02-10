#include "BaseConstraint.h"

using namespace QueryEvaluator;

int BaseConstraint::get_statement_id(const AssignmentMap& assignments,
                                     Parser::StatementRef& statement_ref) {
    switch (statement_ref.get_type()) {
    case Parser::StatementRefType::STATEMENT_ID:
        return statement_ref.get_statement_id();
    case Parser::StatementRefType::SYNONYM:
        return assignments.at(statement_ref.get_synonym()).get_int_value();
    case Parser::StatementRefType::ANY:
        throw "Cannot get statement id for ANY statement ref";
    default:
        throw "Unknown statement ref type";
    }
}
