#ifndef SIMPLEPARSER_PARSER_H
#define SIMPLEPARSER_PARSER_H

#include "Lexer.h"
#include "Token.h"
#include "TokenList.h"
#include "Node.h"

namespace SimpleParser {

    // TODO:
    class Parser {
    public:
        Parser(Lexer &lexer);

        ~Parser();

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

        bool is_token_type(Token *token, TokenType expected_type);

        Token *expect_token(TokenType expected_type);

        Token *expect_token(const std::vector<TokenType> &expected_types);

        bool is_word(Token *token, const std::string &expected_value);

        Token *expect_word(const std::string &expected_value);

        bool is_name(Token *token);

        Token *expect_name(const std::string &identifier);

        bool is_integer(Token *token);

        Token *expect_integer(const std::string &identifier);

        std::string to_string(const std::vector<TokenType> &token_types);
    };

}

#endif //SIMPLEPARSER_PARSER_H
