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

        DesignEntity();

        DesignEntity(DesignEntityType type, std::string synonym);

        ~DesignEntity();

    private:
        DesignEntityType type;
        std::string synonym;

    };

}

#endif //PQLPARSER_DESIGNENTITY_H
