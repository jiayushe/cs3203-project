#include <string>
#include <iostream>

#include <regex>
#include "PQLParser.h"
#include "DesignEntity.h"
#include "Parser/SimpleParser/SimpleParser.h"


using namespace Parser;


PQLParser::PQLParser(QueryObject* query_object, TokenList* tokens) :
    query_object(query_object),
    BaseParser(tokens) {}

PQLParser::~PQLParser() {
    delete tokens;
}


// Accepts query string in the form of declaration and save those declarations to query_object (add_declaration)
void PQLParser::process_declaration(std::map<std::string, DesignEntityType> design_entity_map) {
    Token *token = expect_token(TokenType::WORD);
    if (design_entity_map.find(token->get_value()) == design_entity_map.end()) {
        throw "No declaration/Invalid declaration name provided";
    }

    DesignEntityType design_entity_type = design_entity_map[token->get_value()];
    do {
         choice({
            [&]() { // expect synonym
                Token *token = expect_token(TokenType::WORD);
                DesignEntity design_entity(design_entity_type, token->get_value());
                query_object->add_declaration(token->get_value(), design_entity);

            },
            [&]() { // expect comma
                expect_token(TokenType::COMMA);
            }
         }, "Error in process_declaration func");
    } while (tokens->front()->get_type() != TokenType::SEMICOLON &&
              tokens->front()->get_type() != TokenType::END);

    if (tokens->front()->get_type() == TokenType::SEMICOLON)
        tokens->pop_front();
}

void PQLParser::process_selection() {
    Token *token;
    token = expect_token(TokenType::WORD);
    expect_equal(token->get_value(), "Select");

    token = expect_token(TokenType::WORD);
    query_object->set_selection(token->get_value());

    do {
        choice({
           [&]() {
               process_pattern_cl();
           },
           [&]() {
               process_such_that_cl();
           }
        }, "");
    } while (tokens->front()->get_type() != TokenType::END);
}


void PQLParser::process_such_that_cl() {
    std::map<std::string, SuchThatType> such_that_map {
            {"Modifies", SuchThatType::MODIFIES_S},
            {"Uses", SuchThatType::USES_S},
            {"Parent", SuchThatType::PARENT},
            {"Parent*", SuchThatType::PARENT_T},
            {"Follows", SuchThatType::FOLLOWS},
            {"Follows*", SuchThatType::FOLLOWS_T},
    };

    Token *token;
    SuchThat such_that_obj;

    expect_word("such");
    expect_word("that");

    SuchThatType such_that_type;
    token = expect_token(TokenType::WORD);
    if (tokens->front()->get_type() == TokenType::ASTERISK) {
        expect_token(TokenType::ASTERISK);
        such_that_type = such_that_map[token->get_value() + "*"];
    } else {
        such_that_type = such_that_map[token->get_value()];
    }

    such_that_obj.set_type(such_that_type);
    expect_token(TokenType::LPAREN);
    StatementRef left_statement_ref = process_statement_ref();
    such_that_obj.set_left_ref(left_statement_ref);
    expect_token(TokenType::COMMA);
    if (such_that_type == SuchThatType::MODIFIES_S || such_that_type == SuchThatType::USES_S) {
        EntityRef entity_ref = process_entity_ref();
        such_that_obj.set_right_ref(entity_ref);
    } else {
        StatementRef right_statement_ref = process_statement_ref();
        such_that_obj.set_right_ref(right_statement_ref);
    }

    expect_token(TokenType::RPAREN);
    query_object->set_has_such_that(true);
}


void PQLParser::process_pattern_cl() {
    Token *token;
    Pattern pattern_obj;

    expect_word("pattern");

    token = expect_token(TokenType::WORD);
    pattern_obj.set_assigned_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    EntityRef entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    ExpressionSpec expression_spec = process_expression_spec();
    expect_token(TokenType::RPAREN);
    pattern_obj.set_entity_ref(entity_ref);
    pattern_obj.set_expression_spec(expression_spec);

    query_object->set_has_pattern(true);
}

StatementRef PQLParser::process_statement_ref() {
    StatementRef statement_ref;

    choice({
       // stmtRef: synonym
       [&]() {
           statement_ref.set_synonym(expect_token(TokenType::WORD)->get_value());
           statement_ref.set_type(StatementRefType::SYNONYM);
       },

       // stmtRef: '_'
       [&]() {
           expect_token(TokenType::UNDERSCORE)->get_value();
           statement_ref.set_type(StatementRefType::ANY);
       },

       // stmtRef: INTEGER
       [&]() {
           expect_integer(expect_token(TokenType::WORD)->get_value());
           statement_ref.set_type(StatementRefType::STATEMENT_NUMBER);
       }
   }, "Invalid statement ref format");
    return statement_ref;
}


EntityRef PQLParser::process_entity_ref() {
    EntityRef entity_ref;

    choice({
       // entRef: synonym
       [&]() {
           entity_ref.set_synonym(expect_token(TokenType::WORD)->get_value());
           entity_ref.set_type(EntityRefType::SYNONYM);
       },

       // entRef: '_'
       [&]() {
           expect_token(TokenType::UNDERSCORE)->get_value();
           entity_ref.set_type(EntityRefType::ANY);
       },

       // entRef: '"' IDENT '"'
       [&]() {
           std::string identifier;
           identifier += expect_token(TokenType::DOUBLE_QUOTE)->get_value();
           identifier += expect_token(TokenType::WORD)->get_value();
           identifier += expect_token(TokenType::DOUBLE_QUOTE)->get_value();
           entity_ref.set_synonym(identifier);
           entity_ref.set_type(EntityRefType::NAME);
       }
    }, "Invalid entity ref format");
    return entity_ref;
}


ExpressionSpec PQLParser::process_expression_spec() {
    ExpressionSpec expression_spec;
    choice({
       [&]() { // expSpec: _" factor "_
           expect_token(TokenType::UNDERSCORE);
           expect_token(TokenType::DOUBLE_QUOTE);
           SimpleNode *simple_node = SimpleParser(tokens).parse_expr();
           expect_token(TokenType::DOUBLE_QUOTE);
           expect_token(TokenType::UNDERSCORE);
           expression_spec.set_pattern(simple_node);
           expression_spec.set_type(ExpressionSpecType::PATTERN);
       },
       [&]() {
           expect_token(TokenType::UNDERSCORE);
           expression_spec.set_type(ExpressionSpecType::ANY);
       }
    }, "Invalid expression spec format");
    return expression_spec;
}


void PQLParser::choice(const std::vector<std::function<void()>>& parse_funcs,
            std::string error_message) {

    auto saved_pos = tokens->current_pos();
    for (const auto& parse_func : parse_funcs) {
        try {
            parse_func();
            return;
        } catch (...) {
            tokens->reset_pos(saved_pos);
        }
    }

    throw error_message;
}


// Parse token string and store it as a QueryObject
QueryObject *PQLParser::parse_query() {
    Token *token = tokens->front();
    std::map<std::string, DesignEntityType> design_entity_map {
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

    if (design_entity_map.find(token->get_value()) == design_entity_map.end()) {
        throw "No declaration/Invalid declaration name provided";
    }



    while (tokens->front()->get_type() != TokenType::END) {
        try {
            choice({
               [&]() {
                   process_declaration(design_entity_map);
               },
               [&]() {
                   process_selection();
               }
           }, "Expected declaration or Select keyword, received: '" + tokens->front()->get_value() + "'");
        } catch (const std::string& message) {
            std::cout << "Error: " << message << std::endl;
        }
    }

    // print for checking purposes
    query_object->to_string();

    return query_object;
}