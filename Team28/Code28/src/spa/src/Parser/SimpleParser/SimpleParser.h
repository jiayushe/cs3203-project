#pragma once

#include "Parser/shared/BaseParser.h"
#include "Parser/shared/Source.h"
#include "Parser/shared/Token.h"
#include "SimpleLexer.h"
#include "SimpleNode.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Parser {

// SimpleParser for the SIMPLE language (converts tokens to AST).
class SimpleParser : public BaseParser {
public:
    // Create a SimpleParser instance from the given lexer instance.
    explicit SimpleParser(std::shared_ptr<Source> source);

    // The methods below correspond 1-to-1 to the grammar rule for the SIMPLE language.
    // For example, parse_stmt is called when we're parsing a SIMPLE "stmt".
    // See
    // https://github.com/nus-cs3203/project-wiki/wiki/Project-Iteration-1#21-implement-the-almost-full-specification-of-simple
    std::shared_ptr<SimpleNode> parse_program();
    std::shared_ptr<SimpleNode> parse_procedure();
    std::shared_ptr<SimpleNode> parse_stmt_lst();
    std::shared_ptr<SimpleNode> parse_stmt();
    std::shared_ptr<SimpleNode> parse_read();
    std::shared_ptr<SimpleNode> parse_print();
    std::shared_ptr<SimpleNode> parse_while();
    std::shared_ptr<SimpleNode> parse_if();
    std::shared_ptr<SimpleNode> parse_call();
    std::shared_ptr<SimpleNode> parse_assign();
    std::shared_ptr<SimpleNode> parse_cond_expr();
    std::shared_ptr<SimpleNode> parse_rel_expr();
    std::shared_ptr<SimpleNode> parse_rel_factor();
    std::shared_ptr<SimpleNode> parse_expr();
    std::shared_ptr<SimpleNode> parse_term();
    std::shared_ptr<SimpleNode> parse_factor();
    std::shared_ptr<SimpleNode> parse_var_name();
    std::shared_ptr<SimpleNode> parse_proc_name();
    std::shared_ptr<SimpleNode> parse_const_value();

private:
    std::shared_ptr<Source> source;
    int next_statement_id;

    // Helper for choosing one valid parser from parse_funcs.
    // Validity is decided based on whether the chosen parser throws an error.
    // Useful for building a disjunctive parsing logic (see parse_cond_expr for an example).
    std::shared_ptr<SimpleNode>
    choice(const std::vector<std::function<std::shared_ptr<SimpleNode>()>>& parse_funcs,
           std::string error_message);

    // Helper for repeatedly calling parse_func until it throws an error.
    // Useful for building an iterative parsing logic (see parse_stmt_lst for an example).
    std::shared_ptr<SimpleNode> repeat(
        const std::function<std::shared_ptr<SimpleNode>(std::shared_ptr<SimpleNode>)>& parse_func,
        std::shared_ptr<SimpleNode> initial_node);

    // Validation logic
    void validate_program(std::shared_ptr<SimpleNode> program_node);
    void validate_no_duplicate_proc_name(std::shared_ptr<SimpleNode> program_node);
    void validate_no_call_missing_proc(std::shared_ptr<SimpleNode> program_node);
    void validate_no_call_cycle(std::shared_ptr<SimpleNode> program_node);
    bool has_cycle(const std::string& start,
                   std::unordered_map<std::string, std::unordered_set<std::string>>& call_map,
                   std::unordered_map<std::string, int>& status);
    std::unordered_map<std::string, std::unordered_set<std::string>>
    get_call_map(std::shared_ptr<SimpleNode> program_node);
    std::unordered_set<std::string> get_procs_called(std::shared_ptr<SimpleNode> node);
};

} // namespace Parser
