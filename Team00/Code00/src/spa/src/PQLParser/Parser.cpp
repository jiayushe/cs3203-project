#include <string>
#include <iostream>
#include <map>
#include <regex>
#include "Parser.h"
#include "TokenList.h"
#include "StringLexer.h"
#include "DesignEntity.h"

using namespace PQLParser;

Parser::Parser(std::string tmp_query) :
    query(tmp_query) {}

Parser::~Parser() {}


// Accepts query string in the form of declaration and save those declarations to query_object (add_declaration)
void Parser::process_declaration(QueryObject *query_object, TokenList *tokens, DesignEntityType design_entity_type) {
    Token *token = tokens->pop_front();

    while (token->get_type() != TokenType::SEMICOLON) {
        if (token->get_type() == TokenType::COMMA) {
            token = tokens->pop_front();
            continue;
        }

        DesignEntity design_entity(design_entity_type, token->get_value());
        query_object->add_declaration(token->get_value(), design_entity);
        token = tokens->pop_front();
    }
}


// compare two TokenType and throw error if it is different
void Parser::expect_token(TokenType given_type, TokenType expected_type) {
    if (given_type != expected_type) {
        throw "Expected token type " + std::to_string(expected_type) + ", received " +
              std::to_string(given_type);
    }
}


// check if a given string contains all numeric number
bool is_digits(const std::string &str) {
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}


// Example: "hello"  will return true
//          "haha, "ha"ha, haha will return false
bool is_enclosed_by_double_quote(std::string statement_string) {
    std::regex regex_pattern("^(\".+\")$");
    return std::regex_match(statement_string, regex_pattern);
}


// Convert string of expected type StatementRef to a StatementRef class object
StatementRef Parser::process_statement_ref(std::string statement_string) {
    StatementRef statement_ref;
    StatementRefType statement_ref_type;

    if (statement_string == "_") {
        statement_ref_type = StatementRefType::ANY;
    } else if (is_digits(statement_string)) {
        statement_ref_type = StatementRefType::STATEMENT_NUMBER;
        statement_ref.set_statement_number(std::stoi(statement_string));
    } else {
        statement_ref_type = StatementRefType::SYNONYM;
        statement_ref.set_synonym(statement_string);
    }

    statement_ref.set_type(statement_ref_type);
    return statement_ref;
}


// Convert string of expected type EntityRef to a EntityRef class object
EntityRef Parser::process_entity_ref(std::string statement_string) {
    EntityRef entity_ref;
    EntityRefType entity_ref_type;

    if (statement_string == "_") {
        entity_ref_type = EntityRefType::ANY;
    } else if (is_enclosed_by_double_quote(statement_string)) {
        entity_ref_type = EntityRefType::NAME;
        entity_ref.set_name(statement_string);
    } else {
        entity_ref_type = EntityRefType::SYNONYM;
        entity_ref.set_synonym(statement_string);
    }

    entity_ref.set_type(entity_ref_type);
    return entity_ref;
}


// Parse string with pattern: "pattern a(..., ...)" and return a Pattern class object
Pattern Parser::process_pattern_cl(TokenList *tokens) {
    Pattern pattern_obj;
    Token *token = tokens->pop_front();

    pattern_obj.set_assigned_synonym(token->get_value());

    expect_token(tokens->pop_front()->get_type(), TokenType::LPAREN);  // LPAREN
    std::string left_string = "";
    std::string right_string = "";

    // append token string to left_string until it reached COMMA
    token = tokens->pop_front();
    while (token->get_type() != TokenType::COMMA) {
        left_string += token->get_value();
        token = tokens->pop_front();
    }

    // append token string to right_string until it reached RPAREN
    token = tokens->pop_front();
    while (token->get_type() != TokenType::RPAREN) {
        right_string += token->get_value();
        token = tokens->pop_front();
    }

    // LHS is always an entity ref
    EntityRef left_statement_ref = process_entity_ref(left_string);
    pattern_obj.set_entity_ref(left_statement_ref);

    // RHS is an expression spec, set it to the query_object
    if (right_string == "_") {
        ExpressionSpec expression_spec;
        pattern_obj.set_expression_spec(expression_spec);
    } else {
        std::regex regex_pattern("_\"(.*)\"_");
        std::smatch matches;

        // return matches if it is enclosed by _" and "_
        // example: _"x+i"_ will return matches but "x+i"_ won't etc.
        if (std::regex_search(right_string, matches, regex_pattern)) {
            // TODO Integrate with Simple Parser
            std::string target_string = matches[matches.size() - 1];
            ExpressionSpec expression_spec;
            pattern_obj.set_expression_spec(expression_spec);
        } else {
            throw "Invalid expression spec parsed";
        }
    }

    return pattern_obj;
}


// Parse string with pattern: "such that Modifies(..., ...)" and return a SuchThat class object
SuchThat Parser::process_such_that_cl(TokenList *tokens) {
    SuchThat such_that_obj;
    std::map<std::string, SuchThatType> such_that_map {
        {"Modifies", SuchThatType::MODIFIES_S},
        {"Uses", SuchThatType::USES_S},
        {"Parent", SuchThatType::PARENT},
        {"Parent*", SuchThatType::PARENT_T},
        {"Follows", SuchThatType::FOLLOWS},
        {"Follows*", SuchThatType::FOLLOWS_T},
    };

    SuchThatType type;
    std::string such_that_key;

    // check and parse what SuchThatType is used
    Token *token = tokens->pop_front();
    if (tokens->front()->get_value() == "*") {
        std::string star_char = tokens->pop_front()->get_value();
        such_that_key = token->get_value() + star_char;
    } else {
        such_that_key = token->get_value();
    }

    // convert such_that_key string to SuchThatType enum type
    if (such_that_map.find(such_that_key) == such_that_map.end()) {
        throw "Invalid SuchThatType parsed";
    } else {
        type = such_that_map[such_that_key];
        such_that_obj.set_type(type);
    }

    expect_token(tokens->pop_front()->get_type(), TokenType::LPAREN);  // LPAREN
    std::string left_string = "";
    std::string right_string = "";

    // append token string to left_string until it reached COMMA
    token = tokens->pop_front();
    while (token->get_type() != TokenType::COMMA) {
        left_string += token->get_value();
        token = tokens->pop_front();
    }

    // append token string to right_string until it reached RPAREN
    token = tokens->pop_front();
    while (token->get_type() != TokenType::RPAREN) {
        right_string += token->get_value();
        token = tokens->pop_front();
    }

    // LHS is always a StatementRef
    StatementRef left_statement_ref = process_statement_ref(left_string);
    Ref left_ref(left_statement_ref);
    such_that_obj.set_left_ref(left_ref);

    // RHS type depends on SuchThatType
    if (type == SuchThatType::MODIFIES_S || type == SuchThatType::USES_S) {
        EntityRef right_statement_ref = process_entity_ref(right_string);
        Ref right_ref(right_statement_ref);
        such_that_obj.set_right_ref(right_ref);
    } else {
        StatementRef right_statement_ref = process_statement_ref(right_string);
        Ref right_ref(right_statement_ref);
        such_that_obj.set_right_ref(right_ref);
    }

    return such_that_obj;
}


// Parse token string and store it as a QueryObject
QueryObject *Parser::parse_query() {
    auto queryObject = new QueryObject();
    std::cout << "Query String: " << query << std::endl;

    StringLexer lexer(query);
    TokenList *tokens = lexer.tokens();
    Token *token = tokens->pop_front();

    std::map<std::string, DesignEntityType> designEntityMap {
        {"stmt", DesignEntityType::STMT},
        {"read", DesignEntityType::READ},
        {"print", DesignEntityType::PRINT},
        {"while", DesignEntityType::WHILE},
        {"if", DesignEntityType::IF},
        {"assign", DesignEntityType::ASSIGN},
        {"variable", DesignEntityType::VARIABLE},
        {"constant", DesignEntityType::CONSTANT},
        {"procedure", DesignEntityType::PROCEDURE}
    };


    // process_declaration
    while (token->get_value() != "Select" && token->get_type() != TokenType::END) {
        if (designEntityMap.find(token->get_value()) != designEntityMap.end()) {
            process_declaration(queryObject, tokens, designEntityMap[token->get_value()]);
        } else {
            throw "Invalid design entity type parsed";
        }

        token = tokens->pop_front();
    }

    do {
        if (token->get_value() == "Select") {
            // expect synonym (for now it is only one variable)
            token = tokens->pop_front();
            queryObject->set_selection(token->get_value());
        } else if (token->get_value() == "pattern") {
            Pattern pattern_obj = process_pattern_cl(tokens);
            queryObject->set_pattern(pattern_obj);
            queryObject->set_has_pattern(true);
        } else if (token->get_value() == "such") {
            token = tokens->pop_front();
            if (token->get_value() != "that") {
                throw "Invalid argument such clause, expected such that clause";
            }

            SuchThat such_that_obj = process_such_that_cl(tokens);
            queryObject->set_such_that(such_that_obj);
            queryObject->set_has_such_that(true);
        }

        token = tokens->pop_front();
    } while (token->get_type() != TokenType::END);

    // print for checking purposes
    queryObject->to_string();
    return queryObject;
}