//
// Created by Victor Varian on 26/1/21.
//

#ifndef PQLPARSER_QUERYOBJECT_H
#define PQLPARSER_QUERYOBJECT_H

#include <unordered_map>
#include "DesignEntity.h"
#include "SuchThat.h"
#include "Pattern.h"

namespace PQLParser {
    // AST node for the SIMPLE language.
    class QueryObject {
    public:
        QueryObject(std::string selection, bool has_such_that_cl, bool has_pattern_cl);

        ~QueryObject();

        // Function
        std::unordered_map<std::string, DesignEntity> getDeclarations();
        void add_declaration(std::string synonym, DesignEntity designEntity);
        std::string get_selection();

        bool get_has_such_that_cl();
        bool get_has_pattern_cl();

        SuchThat get_such_that();

        Pattern get_pattern();

        std::string to_string();

    private:
        std::unordered_map<std::string, DesignEntity> declarations;
        std::string selection;

        bool has_such_that_cl;
        bool has_pattern_cl;

        SuchThat *such_that_obj;
        Pattern *pattern_obj;
    };
}

#endif //PQLPARSER_QUERYOBJECT_H
