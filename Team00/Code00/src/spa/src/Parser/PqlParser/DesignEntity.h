#pragma once

namespace Parser {
    enum DesignEntityType {
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

        DesignEntity();

        DesignEntity(DesignEntityType type, std::string synonym);

        ~DesignEntity();

    private:
        DesignEntityType type;
        std::string synonym;

    };

}
