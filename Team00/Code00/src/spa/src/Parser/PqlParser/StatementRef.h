#pragma once

namespace Parser {
    enum class StatementRefType {
        ANY,
        SYNONYM,
        STATEMENT_ID,
    };

    class StatementRef {
    public:
        StatementRefType get_type();
        std::string get_synonym();
        int get_statement_id();

        StatementRef();

        ~StatementRef();

        void set_type(StatementRefType statement_ref_type);

        void set_synonym(std::string synonym);

        void set_statement_id(int statement_id);

    private:
        StatementRefType type;
        std::string synonym;
        int statement_id;
    };

} // namespace Parser
