//
// Created by Victor Varian on 27/1/21.
//

#include <string>
#include <iostream>
#include "Pattern.h"

using namespace PQLParser;

Pattern::Pattern() {}

Pattern::Pattern(std::string assigned_synonym, EntityRef entity_ref, ExpressionSpec expression_spec) :
        synonym(assigned_synonym),
        entity_ref(entity_ref),
        expression_spec(expression_spec) {}

Pattern::~Pattern() {

}

std::string Pattern::get_assigned_synonym() {
    return synonym;
}

EntityRef Pattern::get_entity_ref() {
    return entity_ref;
}

ExpressionSpec Pattern::get_expression_spec() {
    return expression_spec;
}