#ifndef SIMPLEPARSER_PARSER_H
#define SIMPLEPARSER_PARSER_H

#include "Lexer.h"
#include "Token.h"
#include "TokenList.h"
#include "Node.h"

namespace SimpleParser {

    // Parser for the SIMPLE language (converts tokens to AST).
    class Parser {
    public:
        explicit Parser(Lexer &lexer);

        ~Parser();

        // The methods below correspond 1-to-1 to the grammar rule for the SIMPLE language.
        // For example, parse_stmt is called when we're parsing a SIMPLE "stmt".
        // See https://github.com/nus-cs3203/project-wiki/wiki/Project-Iteration-1#21-implement-the-almost-full-specification-of-simple

        Node *parse_program();

        Node *parse_procedure();

        Node *parse_stmt_lst();

        Node *parse_stmt();

        Node *parse_read();

        Node *parse_print();

        Node *parse_while();

        Node *parse_if();

        Node *parse_call();

        Node *parse_assign();

        Node *parse_cond_expr();

        Node *parse_rel_expr();

        Node *parse_rel_factor();

        Node *parse_expr();

        Node *parse_term();

        Node *parse_factor();

        Node *parse_var_name();

        Node *parse_proc_name();

        Node *parse_const_value();

    private:
        TokenList *tokens;
        int next_statement_id;

        // Registers that the next token should be of the expected type.
        // Throws if the expectation is not fulfilled.
        Token *expect_token(TokenType expected_type);

        // Registers that the next token should be any of the expected types.
        // Throws if the expectation is not fulfilled.
        Token *expect_token(const std::vector<TokenType> &expected_types);

        // Registers that the next token should be a WORD token with the expected value.
        // Throws if the expectation is not fulfilled.
        Token *expect_word(const std::string &expected_value);

        // Registers that the next token should be a WORD token whose value is a name
        // (an alphanumeric string which starts with an alphabet).
        // Throws if the expectation is not fulfilled.
        Token *expect_name(const std::string &identifier);

        // Registers that the next token should be a WORD token whose value is an integer
        // (a numeric string).
        // Throws if the expectation is not fulfilled.
        Token *expect_integer(const std::string &identifier);

        // Checks if the given token type matches with the expected type.
        static bool is_token_type(Token *token, TokenType expected_type);

        // Checks if the given token type matches with any of the expected types.
        static bool is_word(Token *token, const std::string &expected_value);

        // Checks if the given token is a WORD token whose value is a name
        // (an alphanumeric string which starts with an alphabet).
        static bool is_name(Token *token);

        // Checks if the given token is a WORD token whose value is an integer
        // (a numeric string).
        static bool is_integer(Token *token);

        static std::string to_string(const std::vector<TokenType> &token_types);
    };

}

#endif //SIMPLEPARSER_PARSER_H
