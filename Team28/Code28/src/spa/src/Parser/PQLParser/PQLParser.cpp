#include <string>

#include "DesignEntity.h"
#include "PQLParser.h"
#include "Parser/SimpleParser/SimpleParser.h"

using namespace Parser;

PQLParser::PQLParser(BaseLexer& lexer) : BaseParser(lexer.tokens()) {}

// Parse token string and store it as a QueryObject
QueryObject PQLParser::parse_query() {
    repeat({[&]() { process_declaration(); }});
    process_selection();
    expect_token(TokenType::END);

    validate_query();

    return query_object;
}

void PQLParser::process_declaration() {
    choice({[&]() {
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
            }},
           "Invalid Design Entity Type parsed");
}

void PQLParser::process_declaration_synonym(DesignEntityType design_entity_type) {
    auto token = expect_name("synonym");
    DesignEntity design_entity(design_entity_type, token->get_value());
    query_object.add_declaration(token->get_value(), design_entity);

    repeat([&]() {
        expect_token(TokenType::COMMA);
        token = expect_name("synonym");
        DesignEntity design_entity(design_entity_type, token->get_value());
        query_object.add_declaration(token->get_value(), design_entity);
    });

    expect_token(TokenType::SEMICOLON);
}

void PQLParser::process_selection() {
    expect_word("Select");
    auto token = expect_name("select_synonym");
    query_object.set_selection(token->get_value());

    choice({[&]() {
                process_such_that_cl();
                process_pattern_cl();
            },
            [&]() { process_such_that_cl(); }, [&]() { process_pattern_cl(); }, [&]() {}},
           "Should not throw anything in process_selection");
}

void PQLParser::process_such_that_cl() {
    expect_word("such that");

    choice({[&]() {
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
            }},
           "Invalid such that type parsed");
}

// Parse content of the such_that type
void PQLParser::process_such_that_body(SuchThatType such_that_type) {
    SuchThat such_that_obj;

    such_that_obj.set_type(such_that_type);
    expect_token(TokenType::LPAREN);
    auto left_statement_ref = process_statement_ref();
    such_that_obj.set_left_ref(Ref(left_statement_ref));
    expect_token(TokenType::COMMA);

    if (such_that_type == SuchThatType::MODIFIES_S || such_that_type == SuchThatType::USES_S) {
        auto right_entity_ref = process_entity_ref();
        such_that_obj.set_right_ref(Ref(right_entity_ref));
    } else {
        auto right_statement_ref = process_statement_ref();
        such_that_obj.set_right_ref(Ref(right_statement_ref));
    }

    expect_token(TokenType::RPAREN);
    query_object.set_such_that(such_that_obj);
    query_object.set_has_such_that(true);
}

void PQLParser::process_pattern_cl() {
    Pattern pattern_obj;

    expect_word("pattern");

    auto token = expect_name("pattern_synonym");
    pattern_obj.set_assigned_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    auto entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    auto expression_spec = process_expression_spec();
    expect_token(TokenType::RPAREN);
    pattern_obj.set_entity_ref(entity_ref);
    pattern_obj.set_expression_spec(expression_spec);

    query_object.set_pattern(pattern_obj);
    query_object.set_has_pattern(true);
}

StatementRef PQLParser::process_statement_ref() {
    StatementRef statement_ref;

    choice({// stmtRef: synonym
            [&]() {
                auto synonym = expect_name("stmt_synonym")->get_value();
                statement_ref.set_synonym(synonym);
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
            }},
           "Invalid statement ref format");

    return statement_ref;
}

EntityRef PQLParser::process_entity_ref() {
    EntityRef entity_ref;

    choice({// entRef: synonym
            [&]() {
                auto synonym = expect_name("ent_synonym")->get_value();
                entity_ref.set_synonym(synonym);
                entity_ref.set_type(EntityRefType::SYNONYM);
            },

            // entRef: '_'
            [&]() {
                expect_token(TokenType::UNDERSCORE);
                entity_ref.set_type(EntityRefType::ANY);
            },

            // entRef: '"' IDENT '"'
            [&]() {
                expect_token(TokenType::DOUBLE_QUOTE);
                std::string identifier = expect_name("identifier")->get_value();
                expect_token(TokenType::DOUBLE_QUOTE);
                entity_ref.set_name(identifier);
                entity_ref.set_type(EntityRefType::NAME);
            }},
           "Invalid entity ref format");

    return entity_ref;
}

// Parse pattern type second parameter: ExpressionSpec
ExpressionSpec PQLParser::process_expression_spec() {
    ExpressionSpec expression_spec;

    choice({// expSpec: _" factor "_
            [&]() {
                expect_token(TokenType::UNDERSCORE);
                expect_token(TokenType::DOUBLE_QUOTE);
                auto simple_node = SimpleParser(tokens).parse_expr();
                expect_token(TokenType::DOUBLE_QUOTE);
                expect_token(TokenType::UNDERSCORE);
                expression_spec.set_pattern(simple_node);
                expression_spec.set_type(ExpressionSpecType::PATTERN);
            },
            [&]() {
                expect_token(TokenType::UNDERSCORE);
                expression_spec.set_type(ExpressionSpecType::ANY);
            }},
           "Invalid expression spec format");

    return expression_spec;
}

// Matched one of the vector function of parse_funcs then terminate, otherwise throw error if none
// match
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

void PQLParser::validate_query() {
    expect_declaration(query_object.get_selection());
    if (query_object.has_such_that()) {
        validate_such_that_cl(query_object.get_such_that());
    }
    if (query_object.has_pattern()) {
        validate_pattern_cl(query_object.get_pattern());
    }
}

void PQLParser::validate_such_that_cl(const SuchThat& such_that) {
    auto left_ref = such_that.get_left_ref();
    auto right_ref = such_that.get_right_ref();

    switch (such_that.get_type()) {
    case SuchThatType::MODIFIES_S:
    case SuchThatType::USES_S: {
        // LHS must be not be ANY and must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        if (left_statement_ref.get_type() == StatementRefType::ANY) {
            throw "LHS of Uses cannot be ANY";
        }
        validate_statement_ref(left_statement_ref);

        // RHS must be VARIABLE
        auto right_entity_ref = right_ref.get_entity_ref();
        validate_entity_ref(right_entity_ref, {DesignEntityType::VARIABLE});

        break;
    }
    case SuchThatType::PARENT:
    case SuchThatType::PARENT_T:
    case SuchThatType::FOLLOWS:
    case SuchThatType::FOLLOWS_T: {
        // LHS must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        validate_statement_ref(left_statement_ref);

        // RHS must be valid statement ref
        auto right_statement_ref = right_ref.get_statement_ref();
        validate_statement_ref(right_statement_ref);

        break;
    }
    default:
        throw "Unhandled such that type";
    }
}

void PQLParser::validate_pattern_cl(const Pattern& pattern) {
    // Pattern synonym must be ASSIGN
    auto synonym = pattern.get_assigned_synonym();
    expect_declaration(synonym, {DesignEntityType::ASSIGN});

    // LHS must be VARIABLE
    auto left_entity_ref = pattern.get_entity_ref();
    validate_entity_ref(left_entity_ref, {DesignEntityType::VARIABLE});
}

void PQLParser::validate_statement_ref(const StatementRef& statement_ref) {
    validate_statement_ref(statement_ref, {DesignEntityType::STMT, DesignEntityType::READ,
                                           DesignEntityType::PRINT, DesignEntityType::WHILE,
                                           DesignEntityType::IF, DesignEntityType::ASSIGN});
}

void PQLParser::validate_statement_ref(const StatementRef& statement_ref,
                                       const std::vector<DesignEntityType>& expected_types) {
    switch (statement_ref.get_type()) {
    case StatementRefType::SYNONYM:
        expect_declaration(statement_ref.get_synonym(), expected_types);
        break;
    case StatementRefType::STATEMENT_ID:
    case StatementRefType::ANY:
        break;
    default:
        throw "Unhandled statement ref type";
    }
}

void PQLParser::validate_entity_ref(const EntityRef& entity_ref) {
    validate_entity_ref(entity_ref, {DesignEntityType::VARIABLE, DesignEntityType::PROCEDURE});
}

void PQLParser::validate_entity_ref(const EntityRef& entity_ref,
                                    const std::vector<DesignEntityType>& expected_types) {
    switch (entity_ref.get_type()) {
    case EntityRefType::SYNONYM:
        expect_declaration(entity_ref.get_synonym(), expected_types);
        break;
    case EntityRefType::NAME:
    case EntityRefType::ANY:
        break;
    default:
        throw "Unhandled entity ref type";
    }
}

DesignEntity PQLParser::expect_declaration(const std::string& synonym) {
    auto declarations = query_object.get_declarations();
    if (!declarations.has(synonym)) {
        throw "Declaration for synonym '" + synonym + "' doesn't exist";
    }
    return declarations.get(synonym);
}

DesignEntity PQLParser::expect_declaration(const std::string& synonym,
                                           const std::vector<DesignEntityType>& expected_types) {
    auto declaration = expect_declaration(synonym);
    for (auto& expected_type : expected_types) {
        if (declaration.get_type() == expected_type) {
            return declaration;
        }
    }
    throw "Declaration for synonym '" + synonym + "' doesn't match the expected type";
}
