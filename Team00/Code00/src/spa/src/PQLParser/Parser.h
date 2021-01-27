//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_PARSER_H
#define PQLPARSER_PARSER_H

#include "QueryObject.h"

namespace PQLParser {
    class Parser {
    public:
        Parser(std::string tmp_query);

        ~Parser();

        QueryObject parse_query();

    private:
        std::string query;

    };

}

#endif //PQLPARSER_PARSER_H
