//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_PATTERN_H
#define PQLPARSER_PATTERN_H

#include "ExpressionSpec.h"
#include "EntityRef.h"

namespace PQLParser {
    class Pattern {
    public:
        std::string assigned_synonym();
        EntityRef get_entity_ref();
        ExpressionSpec get_expression_spec();

        Pattern(std::string assign_synonym, EntityRef entity_ref, ExpressionSpec expression_spec);

    private:
        std::string synonym;
        EntityRef entity_ref;
        ExpressionSpec expression_spec;

    };

}

#endif //PQLPARSER_PATTERN_H
