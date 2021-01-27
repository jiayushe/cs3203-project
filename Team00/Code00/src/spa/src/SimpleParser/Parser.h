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

        Token *expect_token(TokenType expected_type);

        Token *expect_token(const std::vector<TokenType> &expected_types);

        Token *expect_word(const std::string &expected_value);

        Token *expect_name(const std::string &identifier);

        Token *expect_integer(const std::string &identifier);

        static bool is_token_type(Token *token, TokenType expected_type);

        static bool is_word(Token *token, const std::string &expected_value);

        static bool is_name(Token *token);

        static bool is_integer(Token *token);

        static std::string to_string(const std::vector<TokenType> &token_types);
    };

}

#endif //SIMPLEPARSER_PARSER_H
