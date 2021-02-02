#ifndef PQLPARSER_PATTERN_H
#define PQLPARSER_PATTERN_H

#include "ExpressionSpec.h"
#include "EntityRef.h"

namespace PQLParser {
    class Pattern {
    public:
        std::string get_assigned_synonym();
        EntityRef get_entity_ref();
        ExpressionSpec get_expression_spec();

        Pattern();

        ~Pattern();

        void set_assigned_synonym(std::string assigned_synonym);
        void set_entity_ref(EntityRef entity_ref);
        void set_expression_spec(ExpressionSpec expression_spec);

    private:
        std::string synonym;
        EntityRef entity_ref;
        ExpressionSpec expression_spec;

    };

}

#endif //PQLPARSER_PATTERN_H
