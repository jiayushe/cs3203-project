#include <algorithm>
#include <cctype>
#include <string>
#include "Parser.h"

using namespace SimpleParser;

Parser::Parser(Lexer &lexer) : tokens(lexer.tokens()), next_statement_id(1) {}

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
    auto first_stmt_node = parse_stmt();
    auto stmt_lst_node = new Node(NodeType::STMT_LST);
    stmt_lst_node->add_child(first_stmt_node);
    return repeat(
        [&](Node *stmt_lst_node) {
            auto next_stmt_node = parse_stmt();
            stmt_lst_node->add_child(next_stmt_node);
            return stmt_lst_node;
        },
        stmt_lst_node
    );
}

Node *Parser::parse_stmt() {
    return choice(
        {
            // stmt: read
            [this]() {
                return parse_read();
            },

            // stmt: print
            [this]() {
                return parse_print();
            },

            // stmt: call
            [this]() {
                return parse_call();
            },

            // stmt: while
            [this]() {
                return parse_while();
            },

            // stmt: if
            [this]() {
                return parse_if();
            },

            // stmt: assign
            [this]() {
                return parse_assign();
            },
        },
        "Expected stmt, received '" + tokens->front()->get_value() + "'"
    );
}

Node *Parser::parse_read() {
    auto statement_id = next_statement_id++;

    expect_word("read");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto read_node = new Node(NodeType::READ, statement_id);
    read_node->add_child(var_name_node);

    return read_node;
}

Node *Parser::parse_print() {
    auto statement_id = next_statement_id++;

    expect_word("print");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto print_node = new Node(NodeType::PRINT, statement_id);
    print_node->add_child(var_name_node);

    return print_node;
}

Node *Parser::parse_call() {
    auto statement_id = next_statement_id++;

    expect_word("call");
    auto proc_name_node = parse_proc_name();
    expect_token(TokenType::SEMICOLON);

    auto call_node = new Node(NodeType::CALL, statement_id);
    call_node->add_child(proc_name_node);

    return call_node;
}

Node *Parser::parse_while() {
    auto statement_id = next_statement_id++;

    expect_word("while");
    expect_token(TokenType::LPAREN);
    auto cond_expr_node = parse_cond_expr();
    expect_token(TokenType::RPAREN);
    expect_token(TokenType::LCURL);
    auto stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto while_node = new Node(NodeType::WHILE, statement_id);
    while_node->add_child(cond_expr_node);
    while_node->add_child(stmt_lst_node);

    return while_node;
}

Node *Parser::parse_if() {
    auto statement_id = next_statement_id++;

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

    auto if_node = new Node(NodeType::IF, statement_id);
    if_node->add_child(cond_expr_node);
    if_node->add_child(then_stmt_lst_node);
    if_node->add_child(else_stmt_lst_node);

    return if_node;
}

Node *Parser::parse_assign() {
    auto statement_id = next_statement_id++;

    auto var_name_node = parse_var_name();
    expect_token(TokenType::EQUAL);
    auto expr_node = parse_expr();
    expect_token(TokenType::SEMICOLON);

    auto assign_node = new Node(NodeType::ASSIGN, statement_id);
    assign_node->add_child(var_name_node);
    assign_node->add_child(expr_node);

    return assign_node;
}

Node *Parser::parse_cond_expr() {
    return choice(
        {
            // cond_expr: ‘!’ ‘(’ cond_expr ‘)’
            [this]() {
                auto op_token = expect_token(TokenType::NOT);
                expect_token(TokenType::LPAREN);
                auto inner_cond_expr_node = parse_cond_expr();
                expect_token(TokenType::RPAREN);

                auto cond_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
                cond_expr_node->add_child(inner_cond_expr_node);

                return cond_expr_node;
            },

            // cond_expr: ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’
            // cond_expr: ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’
            [this]() {
                expect_token(TokenType::LPAREN);
                auto left_cond_expr_node = parse_cond_expr();
                expect_token(TokenType::RPAREN);
                auto op_token = expect_token({TokenType::AND, TokenType::OR});
                expect_token(TokenType::LPAREN);
                auto right_cond_expr_node = parse_cond_expr();
                expect_token(TokenType::RPAREN);

                auto cond_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
                cond_expr_node->add_child(left_cond_expr_node);
                cond_expr_node->add_child(right_cond_expr_node);

                return cond_expr_node;
            },

            // cond_expr: rel_expr
            [this]() {
                return parse_rel_expr();
            }
        },
        "Expected cond_expr, received '" + tokens->front()->get_value() + "'"
    );
}

Node *Parser::parse_rel_expr() {
    auto left_rel_factor = parse_rel_factor();
    auto op_token = expect_token(
        {
            TokenType::GT,
            TokenType::GTE,
            TokenType::LT,
            TokenType::LTE,
            TokenType::DEQUAL,
            TokenType::NEQUAL
        }
    );
    auto right_rel_factor = parse_rel_factor();

    auto rel_expr_node = new Node(NodeType::CONDITIONAL, op_token->get_value());
    rel_expr_node->add_child(left_rel_factor);
    rel_expr_node->add_child(right_rel_factor);

    return rel_expr_node;
}

Node *Parser::parse_rel_factor() {
    return choice(
        {
            // rel_factor: expr
            [this]() {
                return parse_expr();
            },

            // rel_factor: var_name
            [this]() {
                return parse_var_name();
            },

            // rel_factor: const_value
            [this]() {
                return parse_const_value();
            }
        },
        "Expected rel_factor, received '" + tokens->front()->get_value() + "'"
    );
}

// expr: term (('+' | '-') term)*
Node *Parser::parse_expr() {
    auto term_node = parse_term();
    return repeat(
        [&](Node *prev_node) {
            auto op_token = expect_token({TokenType::PLUS, TokenType::MINUS});
            auto term_node = parse_term();

            auto expr_node = new Node(NodeType::ARITHMETIC, op_token->get_value());
            expr_node->add_child(prev_node);
            expr_node->add_child(term_node);

            return expr_node;
        },
        term_node
    );
}

// term: factor (('*' | '/' | '%') factor)*
Node *Parser::parse_term() {
    auto factor_node = parse_factor();
    return repeat(
        [&](Node *prev_node) {
            auto op_token = expect_token({TokenType::ASTERISK, TokenType::SLASH, TokenType::PERCENT});
            auto factor_node = parse_factor();

            auto term_node = new Node(NodeType::ARITHMETIC, op_token->get_value());
            term_node->add_child(prev_node);
            term_node->add_child(factor_node);

            return term_node;
        },
        factor_node
    );
}

Node *Parser::parse_factor() {
    return choice(
        {
            // factor: var_name
            [this]() {
                return parse_var_name();
            },

            // factor: const_value
            [this]() {
                return parse_const_value();
            },

            // factor: '(' expr ')'
            [this]() {
                expect_token(TokenType::LPAREN);
                auto expr_node = parse_expr();
                expect_token(TokenType::RPAREN);
                return expr_node;
            },
        },
        "Expected parse_factor, received '" + tokens->front()->get_value() + "'"
    );
}

Node *Parser::choice(const std::vector<std::function<Node *()>> &parse_funcs, std::string error_message) {
    auto saved_pos = tokens->current_pos();
    auto saved_next_statement_id = next_statement_id;
    for (const auto &parse_func : parse_funcs) {
        try {
            return parse_func();
        } catch (...) {
            tokens->reset_pos(saved_pos);
            next_statement_id = saved_next_statement_id;
        }
    }
    throw error_message;
}

Node *Parser::repeat(const std::function<Node *(Node *)> &parse_func, Node *initial_node) {
    auto node = initial_node;
    auto saved_pos = tokens->current_pos();
    auto saved_next_statement_id = next_statement_id;
    try {
        // Given a valid SIMPLE source, this loop should eventually terminate.
        while (true) {
            node = parse_func(node);
            saved_pos = tokens->current_pos();
            saved_next_statement_id = next_statement_id;
        }
    } catch (...) {
        tokens->reset_pos(saved_pos);
        next_statement_id = saved_next_statement_id;
    }
    return node;
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

Token *Parser::expect_word(const std::string &expected_value) {
    auto token = tokens->pop_front();
    if (!is_word(token, expected_value)) {
        throw "Expected word token with value '" + expected_value + "', received '" + token->get_value() + "'";
    }
    return token;
}

Token *Parser::expect_name(const std::string &identifier) {
    auto token = tokens->pop_front();
    if (!is_name(token)) {
        throw "Expected name token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

Token *Parser::expect_integer(const std::string &identifier) {
    auto token = tokens->pop_front();
    if (!is_integer(token)) {
        throw "Expected integer token " + identifier + ", received '" + token->get_value() + "'";
    }
    return token;
}

bool Parser::is_token_type(Token *token, TokenType expected_type) {
    return token->get_type() == expected_type;
}

bool Parser::is_word(Token *token, const std::string &expected_value) {
    return token->get_type() == TokenType::WORD && token->get_value() == expected_value;
}

bool Parser::is_name(Token *token) {
    return token->get_type() == TokenType::WORD && !token->get_value().empty() && isalpha(token->get_value().front());
}

bool Parser::is_integer(Token *token) {
    auto token_value = token->get_value();
    if (token->get_type() != TokenType::WORD || token_value.empty()) {
        return false;
    }
    return std::all_of(token_value.begin(), token_value.end(), [](char character) {
        return isdigit(character);
    });
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
