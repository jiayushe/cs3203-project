//
// Created by Victor Varian on 27/1/21.
//

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

Parser::~Parser() {

}

void Parser::process_declaration(QueryObject *queryObject, TokenList *tokens, DesignEntityType designEntityType) {
    Token *token = tokens->pop_front();

    while (token->get_type() != TokenType::SEMICOLON) {
        if (token->get_type() == TokenType::COMMA) {
            continue;
        }

        DesignEntity designEntity(designEntityType, token->get_value());
        queryObject->add_declaration(token->get_value(), designEntity);
        token = tokens->pop_front();
    } ;
}


bool is_digits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}


StatementRef Parser::process_statement_ref(std::string statement_string) {
    StatementRef statementRef;
    StatementRefType statementRefType;

    if (statement_string == "_") {
        statementRefType = StatementRefType::ANY;
    } else if (is_digits(statement_string)) {
        statementRefType = StatementRefType::STATEMENT_NUMBER;
        statementRef.set_statement_number(std::stoi(statement_string));
    } else {
        statementRefType = StatementRefType::SYNONYM;
        statementRef.set_synonym(statement_string);
    }

    return statementRef;
}

bool is_enclosed_by_double_quote(std::string statement_string) {
    std::regex regex_pattern("^(\".+\")$");
    return std::regex_match(statement_string, regex_pattern);
}

EntityRef Parser::process_entity_ref(std::string statement_string) {
    EntityRef entityRef;
    EntityRefType entityRefType;

    if (statement_string == "_") {
        entityRefType = EntityRefType::ANY;
    } else if (is_enclosed_by_double_quote(statement_string)) {
        entityRefType = EntityRefType::NAME;
        entityRef.set_name(statement_string);
    } else {
        entityRefType = EntityRefType::SYNONYM;
        entityRef.set_synonym(statement_string);
    }

    entityRef.set_type(entityRefType);
    return entityRef;
}


void Parser::process_such_that_cl(QueryObject *queryObject, TokenList *tokens) {
    std::map<std::string, SuchThatType> suchThatMap {
        {"Modifies", SuchThatType::MODIFIES_S},
        {"Uses", SuchThatType::USES_S},
        {"Parent", SuchThatType::PARENT},
        {"Parent*", SuchThatType::PARENT_T},
        {"Follows", SuchThatType::FOLLOWS},
        {"Follows*", SuchThatType::FOLLOWS_T},
    };

    SuchThatType type;

    Token *token = tokens->pop_front();
    if (tokens->front()->get_value() == "*") {
        std::string star_char = tokens->pop_front()->get_value();
        type = suchThatMap[token->get_value() + star_char];
    } else {
        type = suchThatMap[token->get_value()];
    }

    expect_token(tokens->pop_front()->get_type(), TokenType::LPAREN);  // LPAREN
    std::string left_string = "";
    std::string right_string = "";

    token = tokens->pop_front();
    while (token->get_type() != TokenType::SEMICOLON) {
        left_string += token->get_value();
        token = tokens->pop_front();
    }

    token = tokens->pop_front();
    while (token->get_type() != TokenType::RPAREN) {
        right_string += token->get_value();
        token = tokens->pop_front();
    }

    StatementRef process_statement_ref(left_string);


    EntityRef process_entity_ref(right_string);

    if (type == SuchThatType::MODIFIES_S || type == SuchThatType::USES_S) {

    }
    if (left_string == "_") {
        statementRefType = StatementRefType::ANY;
    } else if (is_digits(left_string)) {
        statementRefType = StatementRefType::STATEMENT_NUMBER;
        statementRef.set_statement_number(std::stoi(left_string));
    } else {
        statementRefType = StatementRefType::SYNONYM;
        statementRef.set_synonym(left_string);
    }


}


void *Parser::expect_token(TokenType given_type, TokenType expected_type) {
    if (given_type != expected_type) {
        throw "Expected token type " + std::to_string(expected_type) + ", received " +
              std::to_string(given_type);
    }
}


QueryObject *Parser::parse_query() {
    auto queryObject = new QueryObject();
    std::cout << query;
    std::cout << static_cast<int>(DesignEntityType::ASSIGN);

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

    while (token->get_value() != "Select" && token->get_value() != TokenType::END) {
        if (designEntityMap.find(token->get_value()) != designEntityMap.end()) {
            process_declaration(queryObject, tokens, designEntityMap[token->get_value()]);
        } else {
            throw "Invalid design entity type parsed";
        }
    }


    token = tokens->pop_front();

    do {
        if (token->get_value() == "Select") {
            // expect synonym (for now it is only one variable)
            token = tokens->pop_front();
            queryObject->set_selection(token->get_value());
        } else if (token->get_value() == "pattern") {

        } else if (token->get_value() == "such") {
            token = tokens->pop_front();
            if (token->get_value() != "that") {
                throw "Invalid argument such clause, expected such that clause"
            }

            process_such_that_cl(queryObject, tokens)
        }

    } while (token->get_type() != TokenType::END);

    return qo;
}