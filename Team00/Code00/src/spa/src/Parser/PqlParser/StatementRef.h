#pragma once

namespace Parser {
    enum StatementRefType {
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

        void set_type(StatementRefType statement_ref_type);

        void set_synonym(std::string synonym);

        void set_statement_number(int statement_number);

    private:
        StatementRefType type;
        std::string synonym;
        int statement_number;

    };

}
