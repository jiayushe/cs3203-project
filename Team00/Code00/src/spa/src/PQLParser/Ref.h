#ifndef PQLPARSER_REF_H
#define PQLPARSER_REF_H

#include "EntityRef.h"
#include "StatementRef.h"

namespace PQLParser {
    enum class RefType {
        ENTITY,
        STATEMENT,
    };

    class Ref {
    public:
        RefType get_type();
        EntityRef get_entity_ref();
        StatementRef get_statement_ref();

        Ref();

        Ref(EntityRef entity_ref);

        Ref(StatementRef statement_ref);

        ~Ref();

    private:
        RefType type;
        EntityRef entity_ref;
        StatementRef statement_ref;

    };

}

#endif //PQLPARSER_REF_H
