#include "SimpleParser.h"

using namespace Parser;

SimpleParser::SimpleParser(std::shared_ptr<Source> source)
    : BaseParser(std::make_shared<SimpleLexer>(source)), source(source), next_statement_id(1) {}

std::shared_ptr<SimpleNode> SimpleParser::parse_program() {
    auto program_node = std::make_shared<SimpleNode>(SimpleNodeType::PROGRAM);
    auto first_procedure_node = parse_procedure();
    program_node->add_child(first_procedure_node);
    repeat(
        [&](std::shared_ptr<SimpleNode> program_node) {
            auto next_procedure_node = parse_procedure();
            program_node->add_child(next_procedure_node);
            return program_node;
        },
        program_node);
    expect_token(TokenType::END);

    validate_program(program_node);

    return program_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_procedure() {
    expect_word("procedure");
    auto proc_name_node = parse_proc_name();
    expect_token(TokenType::LCURL);
    auto stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto procedure_node = std::make_shared<SimpleNode>(SimpleNodeType::PROCEDURE);
    procedure_node->add_child(proc_name_node);
    procedure_node->add_child(stmt_lst_node);

    return procedure_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_stmt_lst() {
    auto stmt_lst_node = std::make_shared<SimpleNode>(SimpleNodeType::STMT_LST);
    auto first_stmt_node = parse_stmt();
    stmt_lst_node->add_child(first_stmt_node);
    return repeat(
        [&](std::shared_ptr<SimpleNode> stmt_lst_node) {
            auto next_stmt_node = parse_stmt();
            stmt_lst_node->add_child(next_stmt_node);
            return stmt_lst_node;
        },
        stmt_lst_node);
}

std::shared_ptr<SimpleNode> SimpleParser::parse_stmt() {
    return choice({
                      // stmt: read
                      [this]() { return parse_read(); },

                      // stmt: print
                      [this]() { return parse_print(); },

                      // stmt: call
                      [this]() { return parse_call(); },

                      // stmt: while
                      [this]() { return parse_while(); },

                      // stmt: if
                      [this]() { return parse_if(); },

                      // stmt: assign
                      [this]() { return parse_assign(); },
                  },
                  "Expected stmt, received '" + lexer->peek_token()->get_value() + "'");
}

std::shared_ptr<SimpleNode> SimpleParser::parse_read() {
    auto statement_id = next_statement_id++;

    expect_word("read");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto read_node = std::make_shared<SimpleNode>(SimpleNodeType::READ, statement_id);
    read_node->add_child(var_name_node);

    return read_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_print() {
    auto statement_id = next_statement_id++;

    expect_word("print");
    auto var_name_node = parse_var_name();
    expect_token(TokenType::SEMICOLON);

    auto print_node = std::make_shared<SimpleNode>(SimpleNodeType::PRINT, statement_id);
    print_node->add_child(var_name_node);

    return print_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_call() {
    auto statement_id = next_statement_id++;

    expect_word("call");
    auto proc_name_node = parse_proc_name();
    expect_token(TokenType::SEMICOLON);

    auto call_node = std::make_shared<SimpleNode>(SimpleNodeType::CALL, statement_id);
    call_node->add_child(proc_name_node);

    return call_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_while() {
    auto statement_id = next_statement_id++;

    expect_word("while");
    expect_token(TokenType::LPAREN);
    auto cond_expr_node = parse_cond_expr();
    expect_token(TokenType::RPAREN);
    expect_token(TokenType::LCURL);
    auto stmt_lst_node = parse_stmt_lst();
    expect_token(TokenType::RCURL);

    auto while_node = std::make_shared<SimpleNode>(SimpleNodeType::WHILE, statement_id);
    while_node->add_child(cond_expr_node);
    while_node->add_child(stmt_lst_node);

    return while_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_if() {
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

    auto if_node = std::make_shared<SimpleNode>(SimpleNodeType::IF, statement_id);
    if_node->add_child(cond_expr_node);
    if_node->add_child(then_stmt_lst_node);
    if_node->add_child(else_stmt_lst_node);

    return if_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_assign() {
    auto statement_id = next_statement_id++;

    auto var_name_node = parse_var_name();
    expect_token(TokenType::EQUAL);
    auto expr_node = parse_expr();
    expect_token(TokenType::SEMICOLON);

    auto assign_node = std::make_shared<SimpleNode>(SimpleNodeType::ASSIGN, statement_id);
    assign_node->add_child(var_name_node);
    assign_node->add_child(expr_node);

    return assign_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_cond_expr() {
    return choice({// cond_expr: ???!??? ???(??? cond_expr ???)???
                   [this]() {
                       auto op_token = expect_token(TokenType::NOT);
                       expect_token(TokenType::LPAREN);
                       auto inner_cond_expr_node = parse_cond_expr();
                       expect_token(TokenType::RPAREN);

                       auto cond_expr_node = std::make_shared<SimpleNode>(
                           SimpleNodeType::CONDITIONAL, op_token->get_value());
                       cond_expr_node->add_child(inner_cond_expr_node);

                       return cond_expr_node;
                   },

                   // cond_expr: ???(??? cond_expr ???)??? ???&&??? ???(??? cond_expr ???)???
                   // cond_expr: ???(??? cond_expr ???)??? ???||??? ???(??? cond_expr ???)???
                   [this]() {
                       expect_token(TokenType::LPAREN);
                       auto left_cond_expr_node = parse_cond_expr();
                       expect_token(TokenType::RPAREN);
                       auto op_token = expect_token({TokenType::AND, TokenType::OR});
                       expect_token(TokenType::LPAREN);
                       auto right_cond_expr_node = parse_cond_expr();
                       expect_token(TokenType::RPAREN);

                       auto cond_expr_node = std::make_shared<SimpleNode>(
                           SimpleNodeType::CONDITIONAL, op_token->get_value());
                       cond_expr_node->add_child(left_cond_expr_node);
                       cond_expr_node->add_child(right_cond_expr_node);

                       return cond_expr_node;
                   },

                   // cond_expr: rel_expr
                   [this]() { return parse_rel_expr(); }},
                  "Expected cond_expr, received '" + lexer->peek_token()->get_value() + "'");
}

std::shared_ptr<SimpleNode> SimpleParser::parse_rel_expr() {
    auto left_rel_factor = parse_rel_factor();
    auto op_token = expect_token({TokenType::GT, TokenType::GTE, TokenType::LT, TokenType::LTE,
                                  TokenType::DEQUAL, TokenType::NEQUAL});
    auto right_rel_factor = parse_rel_factor();

    auto rel_expr_node =
        std::make_shared<SimpleNode>(SimpleNodeType::CONDITIONAL, op_token->get_value());
    rel_expr_node->add_child(left_rel_factor);
    rel_expr_node->add_child(right_rel_factor);

    return rel_expr_node;
}

std::shared_ptr<SimpleNode> SimpleParser::parse_rel_factor() {
    return choice({// rel_factor: expr
                   [this]() { return parse_expr(); },

                   // rel_factor: var_name
                   [this]() { return parse_var_name(); },

                   // rel_factor: const_value
                   [this]() { return parse_const_value(); }},
                  "Expected rel_factor, received '" + lexer->peek_token()->get_value() + "'");
}

// expr: term (('+' | '-') term)*
std::shared_ptr<SimpleNode> SimpleParser::parse_expr() {
    auto term_node = parse_term();
    return repeat(
        [&](std::shared_ptr<SimpleNode> prev_node) {
            auto op_token = expect_token({TokenType::PLUS, TokenType::MINUS});
            auto term_node = parse_term();

            auto expr_node =
                std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, op_token->get_value());
            expr_node->add_child(prev_node);
            expr_node->add_child(term_node);

            return expr_node;
        },
        term_node);
}

// term: factor (('*' | '/' | '%') factor)*
std::shared_ptr<SimpleNode> SimpleParser::parse_term() {
    auto factor_node = parse_factor();
    return repeat(
        [&](std::shared_ptr<SimpleNode> prev_node) {
            auto op_token =
                expect_token({TokenType::ASTERISK, TokenType::SLASH, TokenType::PERCENT});
            auto factor_node = parse_factor();

            auto term_node =
                std::make_shared<SimpleNode>(SimpleNodeType::ARITHMETIC, op_token->get_value());
            term_node->add_child(prev_node);
            term_node->add_child(factor_node);

            return term_node;
        },
        factor_node);
}

std::shared_ptr<SimpleNode> SimpleParser::parse_factor() {
    return choice({
                      // factor: var_name
                      [this]() { return parse_var_name(); },

                      // factor: const_value
                      [this]() { return parse_const_value(); },

                      // factor: '(' expr ')'
                      [this]() {
                          expect_token(TokenType::LPAREN);
                          auto expr_node = parse_expr();
                          expect_token(TokenType::RPAREN);
                          return expr_node;
                      },
                  },
                  "Expected parse_factor, received '" + lexer->peek_token()->get_value() + "'");
}

std::shared_ptr<SimpleNode> SimpleParser::parse_var_name() {
    auto var_name_token = expect_name("parse_var_name");
    return std::make_shared<SimpleNode>(SimpleNodeType::VAR_NAME, var_name_token->get_value());
}

std::shared_ptr<SimpleNode> SimpleParser::parse_proc_name() {
    auto proc_name_token = expect_name("parse_proc_name");
    return std::make_shared<SimpleNode>(SimpleNodeType::PROC_NAME, proc_name_token->get_value());
}

std::shared_ptr<SimpleNode> SimpleParser::parse_const_value() {
    auto const_value_token = expect_integer("parse_const_value");
    return std::make_shared<SimpleNode>(SimpleNodeType::CONST_VALUE,
                                        const_value_token->get_value());
}

std::shared_ptr<SimpleNode>
SimpleParser::choice(const std::vector<std::function<std::shared_ptr<SimpleNode>()>>& parse_funcs,
                     std::string error_message) {
    auto saved_pos = source->current_pos();
    auto saved_next_statement_id = next_statement_id;
    for (const auto& parse_func : parse_funcs) {
        try {
            return parse_func();
        } catch (...) {
            source->reset_pos(saved_pos);
            next_statement_id = saved_next_statement_id;
        }
    }
    throw std::runtime_error(error_message);
}

std::shared_ptr<SimpleNode> SimpleParser::repeat(
    const std::function<std::shared_ptr<SimpleNode>(std::shared_ptr<SimpleNode>)>& parse_func,
    std::shared_ptr<SimpleNode> initial_node) {
    auto node = initial_node;
    auto saved_pos = source->current_pos();
    auto saved_next_statement_id = next_statement_id;
    try {
        // Given a valid SIMPLE source, this loop should eventually terminate.
        while (true) {
            node = parse_func(node);
            saved_pos = source->current_pos();
            saved_next_statement_id = next_statement_id;
        }
    } catch (...) {
        source->reset_pos(saved_pos);
        next_statement_id = saved_next_statement_id;
    }
    return node;
}

void SimpleParser::validate_program(std::shared_ptr<SimpleNode> program_node) {
    validate_no_duplicate_proc_name(program_node);
    validate_no_call_missing_proc(program_node);
    validate_no_call_cycle(program_node);
}

void SimpleParser::validate_no_duplicate_proc_name(std::shared_ptr<SimpleNode> program_node) {
    std::unordered_set<std::string> used_proc_names;
    for (auto const& procedure_node : program_node->get_children()) {
        auto proc_name = procedure_node->get_child(0)->get_value();
        if (used_proc_names.find(proc_name) != used_proc_names.end()) {
            throw std::runtime_error("Encountered duplicate procedure names");
        }
        used_proc_names.insert(proc_name);
    }
}

void SimpleParser::validate_no_call_missing_proc(std::shared_ptr<SimpleNode> program_node) {
    auto call_map = get_call_map(program_node);
    for (auto const& entry : call_map) {
        auto proc_name = entry.first;
        auto procs_called = entry.second;
        for (auto const& proc_called : procs_called) {
            if (call_map.find(proc_called) == call_map.end()) {
                throw std::runtime_error("Encountered call to a missing procedure");
            }
        }
    }
}

void SimpleParser::validate_no_call_cycle(std::shared_ptr<SimpleNode> program_node) {
    auto call_map = get_call_map(program_node);
    for (auto const& entry : call_map) {
        std::unordered_map<std::string, int> status;
        if (has_cycle(entry.first, call_map, status)) {
            throw std::runtime_error("Encountered call cycle");
        }
    }
}

bool SimpleParser::has_cycle(
    const std::string& start,
    std::unordered_map<std::string, std::unordered_set<std::string>>& call_map,
    std::unordered_map<std::string, int>& status) {

    if (status[start] == 1) {
        return true;
    }
    if (status[start] == 2) {
        return false;
    }

    status[start] = 1;

    for (auto const& next : call_map[start]) {
        if (has_cycle(next, call_map, status)) {
            return true;
        }
    }

    status[start] = 2;
    return false;
}

std::unordered_map<std::string, std::unordered_set<std::string>>
SimpleParser::get_call_map(std::shared_ptr<SimpleNode> program_node) {
    std::unordered_map<std::string, std::unordered_set<std::string>> call_map;
    for (auto const& procedure_node : program_node->get_children()) {
        auto proc_name = procedure_node->get_child(0)->get_value();
        call_map[proc_name] = get_procs_called(procedure_node);
    }
    return call_map;
}

std::unordered_set<std::string> SimpleParser::get_procs_called(std::shared_ptr<SimpleNode> node) {
    std::unordered_set<std::string> procs_called;
    for (auto const& child : node->get_children()) {
        if (node->get_type() == SimpleNodeType::CALL) {
            auto proc_name = node->get_child(0)->get_value();
            procs_called.insert(proc_name);
        } else {
            auto child_procs_called = get_procs_called(child);
            procs_called.insert(child_procs_called.begin(), child_procs_called.end());
        }
    }
    return procs_called;
}
