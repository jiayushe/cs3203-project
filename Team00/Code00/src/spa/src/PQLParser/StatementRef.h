//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_STATEMENTREF_H
#define PQLPARSER_STATEMENTREF_H

namespace PQLParser {
    enum class StatementRefType {
        ANY,
        SYNONYM,
        STATEMENT_NUMBER,
    };

    class StatementRef {
    public:
        StatementRefType get_type();
        std::string get_synonym();
        int get_statement_number();

        StatementRef();

        ~StatementRef();

        void set_type(StatementRefType statementRefType);

        void set_synonym(std::string synonym);

        void set_statement_number(int statement_number);

    private:
        StatementRefType type;
        std::string synonym;
        int statement_number;

    };

}

#endif //PQLPARSER_STATEMENTREF_H
