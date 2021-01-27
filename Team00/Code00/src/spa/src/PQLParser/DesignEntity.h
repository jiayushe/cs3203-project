//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_DESIGNENTITY_H
#define PQLPARSER_DESIGNENTITY_H

namespace PQLParser {
    enum class DesignEntityType {
        STMT,
        READ,
        PRINT,
        WHILE,
        IF,
        ASSIGN,
        VARIABLE,
        CONSTANT,
        PROCEDURE,
    };

    class DesignEntity {
    public:
        DesignEntityType get_type();
        std::string get_synonym();

        DesignEntity(DesignEntityType type);

        DesignEntity(DesignEntityType type, std::string synonym);

    private:
        DesignEntityType type;
        std::string synonym;

    };

}

#endif //PQLPARSER_DESIGNENTITY_H
