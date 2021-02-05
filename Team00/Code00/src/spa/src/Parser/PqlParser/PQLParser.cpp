#include <string>
#include <iostream>

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


void PQLParser::process_declaration() {
    choice({
        [&]() {
            expect_word("stmt");
            process_declaration_synonym(DesignEntityType::STMT);
        },
        [&]() {
           expect_word("read");
           process_declaration_synonym(DesignEntityType::READ);
        },
        [&]() {
           expect_word("print");
           process_declaration_synonym(DesignEntityType::PRINT);
        },
        [&]() {
           expect_word("while");
           process_declaration_synonym(DesignEntityType::WHILE);
        },
        [&]() {
           expect_word("if");
           process_declaration_synonym(DesignEntityType::IF);
        },
        [&]() {
           expect_word("assign");
           process_declaration_synonym(DesignEntityType::ASSIGN);
        },
        [&]() {
           expect_word("variable");
           process_declaration_synonym(DesignEntityType::VARIABLE);
        },
        [&]() {
           expect_word("constant");
           process_declaration_synonym(DesignEntityType::CONSTANT);
        },
        [&]() {
           expect_word("procedure");
           process_declaration_synonym(DesignEntityType::PROCEDURE);
        }
    }, "Invalid Design Entity Type parsed");
}


void PQLParser::process_declaration_synonym(DesignEntityType design_entity_type) {
    Token *token = expect_name("synonym");
    DesignEntity design_entity(design_entity_type, token->get_value());
    query_object->add_declaration(token->get_value(), design_entity);

    repeat([&]() {
        expect_token(TokenType::COMMA);
        token = expect_name("synonym");
        DesignEntity design_entity(design_entity_type, token->get_value());
        query_object->add_declaration(token->get_value(), design_entity);
    });
    expect_token(TokenType::SEMICOLON);
}


void PQLParser::process_selection() {
    expect_word("Select");
    Token *token = expect_name("select_synonym");
    query_object->set_selection(token->get_value());

    choice({
        [&]() {
            process_such_that_cl();
            process_pattern_cl();
        },
        [&]() {
            process_such_that_cl();
        },
        [&]() {
            process_pattern_cl();
        },
        [&]() { }
    }, "Should not throw anything in process_selection");
}


void PQLParser::process_such_that_cl() {
    expect_word("such");
    expect_word("that");

    choice({
        [&]() {
            expect_word("Modifies");
            process_such_that_body(SuchThatType::MODIFIES_S);
        },
        [&]() {
            expect_word("Uses");
            process_such_that_body(SuchThatType::USES_S);
        },
        [&]() {
            expect_word("Parent");
            process_such_that_body(SuchThatType::PARENT);
        },
        [&]() {
            expect_word("Parent*");
            process_such_that_body(SuchThatType::PARENT_T);
        },
        [&]() {
            expect_word("Follows");
            process_such_that_body(SuchThatType::FOLLOWS);
        },
        [&]() {
            expect_word("Follows*");
            process_such_that_body(SuchThatType::FOLLOWS_T);
        }
    }, "Invalid such that type parsed");
}


// Parse content of the such_that type
void PQLParser::process_such_that_body(SuchThatType such_that_type) {
    SuchThat such_that_obj;

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
    query_object->set_such_that(such_that_obj);
    query_object->set_has_such_that(true);
}


void PQLParser::process_pattern_cl() {
    Pattern pattern_obj;
    expect_word("pattern");

    Token *token = expect_name("pattern_synonym");
    pattern_obj.set_assigned_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    EntityRef entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    ExpressionSpec expression_spec = process_expression_spec();
    expect_token(TokenType::RPAREN);
    pattern_obj.set_entity_ref(entity_ref);
    pattern_obj.set_expression_spec(expression_spec);

    query_object->set_pattern(pattern_obj);
    query_object->set_has_pattern(true);
}


StatementRef PQLParser::process_statement_ref() {
    StatementRef statement_ref;

    choice({
       // stmtRef: synonym
       [&]() {
           statement_ref.set_synonym(expect_name("stmt_synonym")->get_value());
           statement_ref.set_type(StatementRefType::SYNONYM);
       },

       // stmtRef: '_'
       [&]() {
           expect_token(TokenType::UNDERSCORE);
           statement_ref.set_type(StatementRefType::ANY);
       },

       // stmtRef: INTEGER
       [&]() {
           std::string statement_id = expect_integer("statement_id")->get_value();
           statement_ref.set_statement_id(std::stoi(statement_id));
           statement_ref.set_type(StatementRefType::STATEMENT_ID);
       }
   }, "Invalid statement ref format");
    return statement_ref;
}


EntityRef PQLParser::process_entity_ref() {
    EntityRef entity_ref;

    choice({
       // entRef: synonym
       [&]() {
           entity_ref.set_synonym(expect_name("ent_synonym")->get_value());
           entity_ref.set_type(EntityRefType::SYNONYM);
       },

       // entRef: '_'
       [&]() {
           expect_token(TokenType::UNDERSCORE);
           entity_ref.set_type(EntityRefType::ANY);
       },

       // entRef: '"' IDENT '"'
       [&]() {
           expect_token(TokenType::DOUBLE_QUOTE)->get_value();
           std::string identifier = expect_name("identifier")->get_value();
           expect_token(TokenType::DOUBLE_QUOTE)->get_value();
           entity_ref.set_synonym(identifier);
           entity_ref.set_type(EntityRefType::NAME);
       }
    }, "Invalid entity ref format");
    return entity_ref;
}


// Parse pattern type second parameter: ExpressionSpec
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


// Matched one of the vector function of parse_funcs then terminate, otherwise throw error if none match
void PQLParser::choice(const std::vector<std::function<void()>>& parse_funcs, std::string error_message) {
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


// atomic, repeatedly running the parse_func until it fails
void PQLParser::repeat(const std::function<void()>& parse_func) {
    auto saved_pos = tokens->current_pos();
    try {
        while (true) {
            parse_func();
            saved_pos = tokens->current_pos();
        }
    } catch (...) {
        tokens->reset_pos(saved_pos);
    }
}


// Parse token string and store it as a QueryObject
QueryObject *PQLParser::parse_query() {
    repeat({
     [&]() {
           process_declaration();
        }
    });
    process_selection();
    expect_token(TokenType::END);

    // print for checking purposes
    query_object->to_string();

    return query_object;
}