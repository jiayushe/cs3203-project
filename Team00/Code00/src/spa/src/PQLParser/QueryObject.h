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
        QueryObject();

        ~QueryObject();

        // Function
        std::unordered_map<std::string, DesignEntity> getDeclarations();
        void add_declaration(std::string synonym, DesignEntity designEntity);
        std::string get_selection();

        bool has_such_that();
        bool has_pattern();

        SuchThat get_such_that();

        Pattern get_pattern();

        void set_selection(std::string selection);
        void set_has_such_that(bool has_such_that);
        void set_has_pattern(bool has_pattern);

        void set_such_that(SuchThat such_that_cl);
        void set_pattern(Pattern pattern_cl);

        void to_string();

    private:
        std::unordered_map<std::string, DesignEntity> declarations;
        std::string selection;

        bool has_such_that_cl;
        bool has_pattern_cl;

        SuchThat such_that_obj;
        Pattern pattern_obj;
    };
}

#endif //PQLPARSER_QUERYOBJECT_H
