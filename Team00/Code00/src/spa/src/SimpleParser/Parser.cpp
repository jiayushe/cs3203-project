#include <cctype>
#include <string>
#include "Parser.h"

using namespace SimpleParser;

Parser::Parser(Lexer &lexer) : tokens(lexer.tokens()) {}

Parser::~Parser() {
    delete tokens;
}

Node *Parser::parse_program() {
    auto procedure_node = parse_procedure();
    expect_token(TokenType::END);

    auto program_node = new Node(NodeType::PROGRAM);
    program_node->add_child(procedure_node);

    return program_node;
}

Node *Parser::parse_procedure() {
    expect_word("procedure");
    auto proc_name_node = parse_proc_name();
    expect_token(TokenType::LCURL);
    auto stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto procedure_node = new Node(NodeType::PROCEDURE);
    procedure_node->add_child(proc_name_node);
    procedure_node->add_child(stmt_lst_node);

    return procedure_node;
}

Node *Parser::parse_stmt_lst() {
    auto stmt_node = parse_stmt();

    auto stmt_lst_node = new Node(NodeType::STMT_LST);
    stmt_lst_node->add_child(stmt_node);

    auto saved_pos = tokens->current_pos();
    try {
        while (true) {
            stmt_lst_node->add_child(parse_stmt());
            saved_pos = tokens->current_pos();
        }
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    return stmt_lst_node;
}

Node *Parser::parse_stmt() {
    auto saved_pos = tokens->current_pos();

    // stmt: read
    try {
        return parse_read();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // stmt: print
    try {
        return parse_print();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // stmt: call
    try {
        return parse_call();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // stmt: while
    try {
        return parse_while();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // stmt: if
    try {
        return parse_if();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // stmt: assign
    try {
        return parse_assign();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    throw "Expected stmt, received '" + tokens->front()->get_value() + "'";
}

Node *Parser::parse_read() {
    expect_word("read");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto read_node = new Node(NodeType::READ);
    read_node->add_child(var_name_node);

    return read_node;
}

Node *Parser::parse_print() {
    expect_word("print");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto print_node = new Node(NodeType::PRINT);
    print_node->add_child(var_name_node);

    return print_node;
}

Node *Parser::parse_call() {
    expect_word("call");
    auto proc_name_node = parse_proc_name();
    expect_token(TokenType::SEMICOLON);

    auto call_node = new Node(NodeType::CALL);
    call_node->add_child(proc_name_node);

    return call_node;
}

Node *Parser::parse_while() {
    expect_word("while");
    expect_token(TokenType::LPAREN);
    auto cond_expr_node = parse_cond_expr();
    expect_token(TokenType::RPAREN);
    expect_token(TokenType::LCURL);
    auto stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto while_node = new Node(NodeType::WHILE);
    while_node->add_child(cond_expr_node);
    while_node->add_child(stmt_lst_node);

    return while_node;
}

Node *Parser::parse_if() {
    expect_word("if");
    expect_token(TokenType::LPAREN);
    auto cond_expr_node = parse_cond_expr();
    expect_token(TokenType::RPAREN);
    expect_word("then");
    expect_token(TokenType::LCURL);
    auto then_stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);
    expect_word("else");
    expect_token(TokenType::LCURL);
    auto else_stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto if_node = new Node(NodeType::IF);
    if_node->add_child(cond_expr_node);
    if_node->add_child(then_stmt_lst_node);
    if_node->add_child(else_stmt_lst_node);

    return if_node;
}

Node *Parser::parse_assign() {
    auto var_name_node = parse_var_name();
    expect_token(TokenType::EQUAL);
    auto expr_node = parse_expr();
    expect_token(TokenType::SEMICOLON);

    auto assign_node = new Node(NodeType::ASSIGN);
    assign_node->add_child(var_name_node);
    assign_node->add_child(expr_node);

    return assign_node;
}

Node *Parser::parse_cond_expr() {
    auto saved_pos = tokens->current_pos();

    // cond_expr: ‘!’ ‘(’ cond_expr ‘)’
    try {
        auto op_token = expect_token(TokenType::NOT);
        expect_token(TokenType::LPAREN);
        auto inner_cond_expr_node = parse_cond_expr();
        expect_token(TokenType::RPAREN);

        auto cond_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
        cond_expr_node->add_child(inner_cond_expr_node);

        return cond_expr_node;
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // cond_expr: ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’
    try {
        expect_token(TokenType::LPAREN);
        auto left_cond_expr_node = parse_cond_expr();
        expect_token(TokenType::RPAREN);
        auto op_token = expect_token(TokenType::AND);
        expect_token(TokenType::LPAREN);
        auto right_cond_expr_node = parse_cond_expr();
        expect_token(TokenType::RPAREN);

        auto cond_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
        cond_expr_node->add_child(left_cond_expr_node);
        cond_expr_node->add_child(right_cond_expr_node);

        return cond_expr_node;
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // cond_expr: ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’
    try {
        expect_token(TokenType::LPAREN);
        auto left_cond_expr_node = parse_cond_expr();
        expect_token(TokenType::RPAREN);
        auto op_token = expect_token(TokenType::OR);
        expect_token(TokenType::LPAREN);
        auto right_cond_expr_node = parse_cond_expr();
        expect_token(TokenType::RPAREN);

        auto cond_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
        cond_expr_node->add_child(left_cond_expr_node);
        cond_expr_node->add_child(right_cond_expr_node);

        return cond_expr_node;
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // cond_expr: rel_expr
    try {
        return parse_rel_expr();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    throw "Expected cond_expr, received '" + tokens->front()->get_value() + "'";
}

Node *Parser::parse_rel_expr() {
    auto left_rel_factor = parse_rel_factor();
    auto op_token = expect_token(std::vector<TokenType>{
            TokenType::GT,
            TokenType::GTE,
            TokenType::LT,
            TokenType::LTE,
            TokenType::DEQUAL,
            TokenType::NEQUAL,
    });
    auto right_rel_factor = parse_rel_factor();

    auto rel_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
    rel_expr_node->add_child(left_rel_factor);
    rel_expr_node->add_child(right_rel_factor);

    return rel_expr_node;
}

Node *Parser::parse_rel_factor() {
    auto saved_pos = tokens->current_pos();

    // rel_factor: expr
    try {
        return parse_expr();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // rel_factor: var_name
    try {
        return parse_var_name();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // rel_factor: const_value
    try {
        return parse_const_value();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    throw "Expected parse_rel_factor, received '" + tokens->front()->get_value() + "'";
}

// expr: term (('+' | '-') term)*
Node *Parser::parse_expr() {
    Node *expr_node = parse_term();

    auto saved_pos = tokens->current_pos();
    try {
        while (true) {
            auto op_token = expect_token(std::vector<TokenType>{
                    TokenType::PLUS,
                    TokenType::MINUS
            });
            auto term_node = parse_term();

            auto prev_expr_node = expr_node;
            expr_node = new Node(NodeType::ARITHMETIC, op_token->get_value());
            expr_node->add_child(prev_expr_node);
            expr_node->add_child(term_node);

            saved_pos = tokens->current_pos();
        }
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    return expr_node;
}

// term: factor (('*' | '/' | '%') factor)*
Node *Parser::parse_term() {
    Node *term_node = parse_factor();

    auto saved_pos = tokens->current_pos();
    try {
        while (true) {
            auto op_token = expect_token(std::vector<TokenType>{
                    TokenType::ASTERISK,
                    TokenType::SLASH,
                    TokenType::PERCENT
            });
            auto factor_node = parse_factor();

            auto prev_term_node = term_node;
            term_node = new Node(NodeType::ARITHMETIC, op_token->get_value());
            term_node->add_child(prev_term_node);
            term_node->add_child(factor_node);

            saved_pos = tokens->current_pos();
        }
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    return term_node;
}

Node *Parser::parse_factor() {
    auto saved_pos = tokens->current_pos();

    // factor: var_name
    try {
        return parse_var_name();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // factor: const_value
    try {
        return parse_const_value();
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    // factor: '(' expr ')'
    try {
        expect_token(TokenType::LPAREN);
        auto expr_node = parse_expr();
        expect_token(TokenType::RPAREN);
        return expr_node;
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }

    throw "Expected parse_factor, received '" + tokens->front()->get_value() + "'";
}

Node *Parser::parse_var_name() {
    auto var_name_token = expect_name("parse_var_name");
    return new Node(NodeType::VAR_NAME, var_name_token->get_value());
}

Node *Parser::parse_proc_name() {
    auto proc_name_token = expect_name("parse_proc_name");
    return new Node(NodeType::PROC_NAME, proc_name_token->get_value());
}

Node *Parser::parse_const_value() {
    auto const_value_token = expect_integer("parse_const_value");
    return new Node(NodeType::CONST_VALUE, const_value_token->get_value());
}

bool Parser::is_token_type(Token *token, TokenType expected_type) {
    return token->get_type() == expected_type;
}

Token *Parser::expect_token(TokenType expected_type) {
    auto token = tokens->pop_front();
    if (!is_token_type(token, expected_type)) {
        throw "Expected token type " + std::to_string(expected_type) + ", received " +
              std::to_string(token->get_type());
    }
    return token;
}

Token *Parser::expect_token(const std::vector<TokenType> &expected_types) {
    auto token = tokens->pop_front();
    for (auto const &expected_type : expected_types) {
        if (is_token_type(token, expected_type)) {
            return token;
        }
    }
    throw "Expected token type " + to_string(expected_types) + ", received " + std::to_string(token->get_type());
}

bool Parser::is_word(Token *token, const std::string &expected_value) {
    return token->get_type() == TokenType::WORD && token->get_value() == expected_value;
}

Token *Parser::expect_word(const std::string &expected_value) {
    auto token = tokens->pop_front();
    if (!is_word(token, expected_value)) {
        throw "Expected word token with value '" + expected_value + "', received '" + token->get_value() + "'";
    }
    return token;
}

bool Parser::is_name(Token *token) {
    return token->get_type() == TokenType::WORD && !token->get_value().empty() && isalpha(token->get_value().front());
}

Token *Parser::expect_name(const std::string &identifier) {
    auto token = tokens->pop_front();
    if (!is_name(token)) {
        throw "Expected name token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

bool Parser::is_integer(Token *token) {
    if (token->get_type() != TokenType::WORD || token->get_value().empty()) {
        return false;
    }
    for (auto const &character : token->get_value()) {
        if (!isnumber(character)) {
            return false;
        }
    }
    return true;
}

Token *Parser::expect_integer(const std::string &identifier) {
    auto token = tokens->pop_front();
    if (!is_integer(token)) {
        throw "Expected integer token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

std::string Parser::to_string(const std::vector<TokenType> &token_types) {
    std::string value;
    for (auto const token_type : token_types) {
        if (value.empty()) {
            value += "[" + std::to_string(token_type);
        } else {
            value += ", " + std::to_string(token_type);
        }
    }
    value += "]";
    return value;
}
