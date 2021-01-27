//
// Created by Victor Varian on 27/1/21.
//

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

        Ref(RefType type);

    private:
        RefType type;
        EntityRef entity_ref;
        StatementRef statement_ref;

    };

}

#endif //PQLPARSER_REF_H
