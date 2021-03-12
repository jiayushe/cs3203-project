#include <stdexcept>
#include <string>

#include "DesignEntity.h"
#include "PQLLexer.h"
#include "PQLParser.h"
#include "Parser/SimpleParser/SimpleParser.h"

using namespace Parser;

PQLParser::PQLParser(std::shared_ptr<Source> source)
    : BaseParser(std::make_shared<PQLLexer>(source)), source(source) {}

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
            },
            [&]() {
                expect_word("call");
                process_declaration_synonym(DesignEntityType::CALL);
            },
            [&]() {
                expect_word("prog_line");
                process_declaration_synonym(DesignEntityType::PROG_LINE);
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

    Result result;
    choice({// 'BOOLEAN'
            [&]() {
                expect_word("BOOLEAN");
                result.set_type(ResultType::BOOLEAN);
            },
            [&]() { // elem
                auto elem = process_selection_elem();
                result.set_tuple({elem});
                result.set_type(ResultType::TUPLE);
            },
            [&]() { // tuple
                expect_token(TokenType::LT);
                auto tuple = process_selection_tuple();
                result.set_tuple(tuple);
                result.set_type(ResultType::TUPLE);
                expect_token(TokenType::GT);
            }},
           "Invalid selection format");

    query_object.set_result(result);

    repeat({[&]() {
        choice({[&]() { process_such_that(); }, [&]() { process_pattern(); },
                [&]() { process_with(); }},
               "Should not throw anything in process_selection");
    }});
}

Elem PQLParser::process_selection_elem() {
    Elem elem;
    choice({// attrRef
            [&]() {
                auto synonym = expect_name("synonym")->get_value();
                expect_token(TokenType::DOT);
                auto attr_name = expect_name("attr_name")->get_value();

                AttrRef attr_ref;
                attr_ref.set_synonym(synonym);
                attr_ref.set_attr_name(attr_name);
                elem.set_type(ElemType::ATTR_REF);
                elem.set_attr_ref(attr_ref);
            },
            [&]() { // synonym
                auto synonym = expect_name("synonym")->get_value();
                elem.set_type(ElemType::SYNONYM);
                elem.set_synonym(synonym);
            }},
           "Invalid elem type format");

    return elem;
}

std::vector<Elem> PQLParser::process_selection_tuple() {
    std::vector<Elem> selection_tuples;
    Elem elem = process_selection_elem();
    selection_tuples.push_back(elem);

    repeat([&]() {
        expect_token(TokenType::COMMA);
        elem = process_selection_elem();
        selection_tuples.push_back(elem);
    });

    return selection_tuples;
}

void PQLParser::process_such_that() {
    expect_word("such that");
    process_such_that_condition();

    repeat([&]() {
        expect_word("and");
        process_such_that_condition();
    });
}

void PQLParser::process_such_that_condition() {
    choice({[&]() {
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
            },
            [&]() {
                expect_word("Calls");
                process_such_that_body(SuchThatType::CALLS);
            },
            [&]() {
                expect_word("Calls*");
                process_such_that_body(SuchThatType::CALLS_T);
            },
            [&]() {
                expect_word("Next");
                process_such_that_body(SuchThatType::NEXT);
            },
            [&]() {
                expect_word("Next*");
                process_such_that_body(SuchThatType::NEXT_T);
            },
            [&]() {
                expect_word("Affects");
                process_such_that_body(SuchThatType::AFFECTS);
            },
            [&]() {
                expect_word("Affects*");
                process_such_that_body(SuchThatType::AFFECTS_T);
            },
            [&]() {
                expect_word("Modifies");
                process_such_that_body(SuchThatType::MODIFIES_S);
            },
            [&]() {
                expect_word("Modifies");
                process_such_that_body(SuchThatType::MODIFIES_P);
            },
            [&]() {
                expect_word("Uses");
                process_such_that_body(SuchThatType::USES_S);
            },
            [&]() {
                expect_word("Uses");
                process_such_that_body(SuchThatType::USES_P);
            }},
           "Invalid such that type parsed");
}

SuchThatRef PQLParser::process_such_that_left_ref(SuchThatType such_that_type) {
    switch (such_that_type) { // stmtRef and lineRef
    case SuchThatType::PARENT:
    case SuchThatType::PARENT_T:
    case SuchThatType::FOLLOWS:
    case SuchThatType::FOLLOWS_T:
    case SuchThatType::AFFECTS:
    case SuchThatType::AFFECTS_T:
    case SuchThatType::NEXT:
    case SuchThatType::NEXT_T: {
        auto left_statement_ref = process_statement_ref();
        return SuchThatRef(left_statement_ref);
    }
    case SuchThatType::MODIFIES_S: {
        auto left_statement_ref = process_statement_ref();
        validate_statement_ref(left_statement_ref,
                               {DesignEntityType::STMT, DesignEntityType::READ,
                                DesignEntityType::IF, DesignEntityType::WHILE,
                                DesignEntityType::ASSIGN, DesignEntityType::CALL,
                                DesignEntityType::PROG_LINE});
        return SuchThatRef(left_statement_ref);
    }
    case SuchThatType::USES_S: {
        auto left_statement_ref = process_statement_ref();
        validate_statement_ref(left_statement_ref,
                               {DesignEntityType::STMT, DesignEntityType::PRINT,
                                DesignEntityType::IF, DesignEntityType::WHILE,
                                DesignEntityType::ASSIGN, DesignEntityType::CALL,
                                DesignEntityType::PROG_LINE});
        return SuchThatRef(left_statement_ref);
    }
    case SuchThatType::CALLS:
    case SuchThatType::CALLS_T: {
        auto left_entity_ref = process_entity_ref();
        return SuchThatRef(left_entity_ref);
    }
    case SuchThatType::MODIFIES_P:
    case SuchThatType::USES_P: {
        auto left_entity_ref = process_entity_ref();
        validate_entity_ref(left_entity_ref, {DesignEntityType::PROCEDURE});
        return SuchThatRef(left_entity_ref);
    }
    default:
        throw std::runtime_error("Unmatched such that type with LHS synonym");
    }
}

SuchThatRef PQLParser::process_such_that_right_ref(SuchThatType such_that_type) {
    switch (such_that_type) { // stmtRef and lineRef
    case SuchThatType::PARENT:
    case SuchThatType::PARENT_T:
    case SuchThatType::FOLLOWS:
    case SuchThatType::FOLLOWS_T:
    case SuchThatType::AFFECTS:
    case SuchThatType::AFFECTS_T:
    case SuchThatType::NEXT:
    case SuchThatType::NEXT_T: {
        auto right_statement_ref = process_statement_ref();
        return SuchThatRef(right_statement_ref);
    }
    case SuchThatType::MODIFIES_S:
    case SuchThatType::MODIFIES_P:
    case SuchThatType::USES_S:
    case SuchThatType::USES_P:
    case SuchThatType::CALLS:
    case SuchThatType::CALLS_T: {
        auto right_entity_ref = process_entity_ref();
        return SuchThatRef(right_entity_ref);
    }
    default:
        throw std::runtime_error("Unmatched such that type with RHS synonym");
    }
}

// Parse content of the such_that type
void PQLParser::process_such_that_body(SuchThatType such_that_type) {
    SuchThat such_that;
    such_that.set_type(such_that_type);
    expect_token(TokenType::LPAREN);

    SuchThatRef left_such_that_ref = process_such_that_left_ref(such_that_type);
    such_that.set_left_ref(left_such_that_ref);

    expect_token(TokenType::COMMA);

    SuchThatRef right_such_that_ref = process_such_that_right_ref(such_that_type);
    such_that.set_right_ref(right_such_that_ref);

    expect_token(TokenType::RPAREN);
    query_object.add_such_that(such_that);
}

void PQLParser::process_pattern() {
    expect_word("pattern");
    process_pattern_condition();

    repeat([&]() {
        expect_word("and");
        process_pattern_condition();
    });
}

void PQLParser::process_pattern_condition() {
    choice({[&]() { process_pattern_assign(); }, [&]() { process_pattern_while(); },
            [&]() { process_pattern_if(); }},
           "Invalid pattern condition given");
}

void PQLParser::process_pattern_assign() {
    PatternAssign pattern_assign;

    auto token = expect_name("pattern_synonym");
    expect_declaration(token->get_value(), {DesignEntityType::ASSIGN});
    pattern_assign.set_assign_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    auto entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    auto expression_spec = process_expression_spec();
    expect_token(TokenType::RPAREN);
    pattern_assign.set_entity_ref(entity_ref);
    pattern_assign.set_expression_spec(expression_spec);

    Pattern pattern;
    pattern.set_type(PatternType::ASSIGN);
    pattern.set_pattern_assign(pattern_assign);
    query_object.add_pattern(pattern);
}

void PQLParser::process_pattern_while() {
    PatternWhile pattern_while;

    auto token = expect_name("pattern_synonym");
    expect_declaration(token->get_value(), {DesignEntityType::WHILE});
    pattern_while.set_while_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    auto entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    expect_token(TokenType::UNDERSCORE);
    expect_token(TokenType::RPAREN);
    pattern_while.set_entity_ref(entity_ref);

    Pattern pattern;
    pattern.set_type(PatternType::WHILE);
    pattern.set_pattern_while(pattern_while);
    query_object.add_pattern(pattern);
}

void PQLParser::process_pattern_if() {
    PatternIf pattern_if;

    auto token = expect_name("pattern_synonym");
    expect_declaration(token->get_value(), {DesignEntityType::IF});
    pattern_if.set_if_synonym(token->get_value());

    expect_token(TokenType::LPAREN);
    auto entity_ref = process_entity_ref();
    expect_token(TokenType::COMMA);
    expect_token(TokenType::UNDERSCORE);
    expect_token(TokenType::COMMA);
    expect_token(TokenType::UNDERSCORE);
    expect_token(TokenType::RPAREN);
    pattern_if.set_entity_ref(entity_ref);

    Pattern pattern;
    pattern.set_type(PatternType::IF);
    pattern.set_pattern_if(pattern_if);
    query_object.add_pattern(pattern);
}

void PQLParser::process_with() {
    expect_word("with");
    process_with_condition();

    repeat([&]() {
        expect_word("and");
        process_with_condition();
    });
}

WithRef PQLParser::process_with_ref() {
    WithRef with_ref;

    choice({[&]() { // entRef: attrRef
                auto synonym = expect_name("synonym")->get_value();
                expect_token(TokenType::DOT);
                auto attr_name = expect_name("attr_name")->get_value();

                AttrRef attr_ref;
                attr_ref.set_synonym(synonym);
                attr_ref.set_attr_name(attr_name);
                with_ref.set_type(WithRefType::ATTR_REF);
                with_ref.set_attr_ref(attr_ref);
            },

            // entRef: synonym
            [&]() {
                auto synonym = expect_name("ent_synonym")->get_value();
                with_ref.set_type(WithRefType::SYNONYM);
                with_ref.set_synonym(synonym);
            },

            // entRef: 'INTEGER'
            [&]() {
                std::string statement_id = expect_integer("statement_id")->get_value();
                with_ref.set_type(WithRefType::STATEMENT_ID);
                with_ref.set_statement_id(std::stoi(statement_id));
            },

            // entRef: '"' IDENT '"'
            [&]() {
                expect_token(TokenType::DOUBLE_QUOTE);
                std::string identifier = expect_name("identifier")->get_value();
                expect_token(TokenType::DOUBLE_QUOTE);
                with_ref.set_type(WithRefType::NAME);
                with_ref.set_name(identifier);
            }},
           "Invalid entity ref format");

    return with_ref;
}

void PQLParser::process_with_condition() {
    WithRef left_ref = process_with_ref();
    expect_token(TokenType::EQUAL);
    WithRef right_ref = process_with_ref();

    With with;
    with.set_left_ref(left_ref);
    with.set_right_ref(right_ref);
    query_object.add_with(with);
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

    choice({[&]() { // expSpec: " factor "
                expect_token(TokenType::DOUBLE_QUOTE);
                auto simple_node = SimpleParser(source).parse_expr();
                expect_token(TokenType::DOUBLE_QUOTE);
                expression_spec.set_exact_pattern(simple_node);
                expression_spec.set_type(ExpressionSpecType::EXACT_PATTERN);
            },
            [&]() { // expSpec: _" factor "_
                expect_token(TokenType::UNDERSCORE);
                expect_token(TokenType::DOUBLE_QUOTE);
                auto simple_node = SimpleParser(source).parse_expr();
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
    auto saved_pos = source->current_pos();
    for (const auto& parse_func : parse_funcs) {
        try {
            parse_func();
            return;
        } catch (...) {
            source->reset_pos(saved_pos);
        }
    }
    throw std::runtime_error(error_message);
}

// atomic, repeatedly running the parse_func until it fails
void PQLParser::repeat(const std::function<void()>& parse_func) {
    auto saved_pos = source->current_pos();
    try {
        while (true) {
            parse_func();
            saved_pos = source->current_pos();
        }
    } catch (...) {
        source->reset_pos(saved_pos);
    }
}

void PQLParser::validate_query() {
    validate_result(query_object.get_result());
    for (auto const& such_that : query_object.get_all_such_that()) {
        validate_such_that(such_that);
    }
    for (auto const& pattern : query_object.get_all_pattern()) {
        validate_pattern(pattern);
    }
    for (auto const& with : query_object.get_all_with()) {
        validate_with(with);
    }
}

void PQLParser::validate_with(const With& with) {
    auto left_with_ref = with.get_left_ref();
    validate_with_ref(left_with_ref);

    auto right_with_ref = with.get_right_ref();
    validate_with_ref(right_with_ref);

    if (get_with_ref_data_type(left_with_ref) != get_with_ref_data_type(right_with_ref)) {
        throw std::runtime_error("LHS with ref and RHS with ref are not comparable");
    }
}

// 0 = INTEGER, 1 = STRING
int PQLParser::get_with_ref_data_type(const WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case WithRefType::SYNONYM:
    case WithRefType::STATEMENT_ID:
        return 0;
    case WithRefType::NAME:
        return 1;
    case WithRefType::ATTR_REF: {
        auto attr_name = with_ref.get_attr_ref().get_attr_name();
        if (attr_name == "stmt#" || attr_name == "value") {
            return 0;
        } else if (attr_name == "procName" || attr_name == "varName") {
            return 1;
        } else {
            throw std::runtime_error("Unknown attribute name");
        }
    }
    default:
        throw std::runtime_error("Unknown with ref type");
    }
}

void PQLParser::validate_with_ref(const WithRef& with_ref) {
    switch (with_ref.get_type()) {
    case WithRefType::ATTR_REF: {
        AttrRef attr_ref = with_ref.get_attr_ref();
        validate_attr_ref(attr_ref);
        break;
    }
    case WithRefType::SYNONYM: {
        expect_declaration(with_ref.get_synonym(), {DesignEntityType::PROG_LINE});
        break;
    }
    case WithRefType::NAME:
    case WithRefType::STATEMENT_ID:
        break;
    default:
        throw std::runtime_error("Unhandled attr ref type");
    }
}

void PQLParser::validate_attr_ref(const AttrRef& attr_ref) {
    std::string synonym = attr_ref.get_synonym();
    std::string attr_name = attr_ref.get_attr_name();

    if (attr_name == "procName") {
        expect_declaration(synonym, {DesignEntityType::PROCEDURE, DesignEntityType::CALL});
    } else if (attr_name == "varName") {
        expect_declaration(
            synonym, {DesignEntityType::VARIABLE, DesignEntityType::READ, DesignEntityType::PRINT});
    } else if (attr_name == "value") {
        expect_declaration(synonym, {DesignEntityType::CONSTANT});
    } else if (attr_name == "stmt#") {
        expect_declaration(synonym,
                           {DesignEntityType::STMT, DesignEntityType::READ, DesignEntityType::PRINT,
                            DesignEntityType::CALL, DesignEntityType::WHILE, DesignEntityType::IF,
                            DesignEntityType::ASSIGN});
    } else {
        throw std::runtime_error("Unhandled attr name type");
    }
}

void PQLParser::validate_result(const Result& result) {
    switch (result.get_type()) {
    case ResultType::TUPLE: {
        auto tuple = result.get_tuple();
        for (auto const& elem : tuple) {
            validate_elem(elem);
        }
        break;
    }
    case ResultType::BOOLEAN:
        break;
    default:
        throw std::runtime_error("Unhandled result type");
    }
}

void PQLParser::validate_such_that(const SuchThat& such_that) {
    auto left_ref = such_that.get_left_ref();
    auto right_ref = such_that.get_right_ref();

    switch (such_that.get_type()) {
    case SuchThatType::MODIFIES_P:
    case SuchThatType::USES_P: {
        // LHS must be not be ANY and must be valid statement ref
        auto left_entity_ref = left_ref.get_entity_ref();
        if (left_entity_ref.get_type() == EntityRefType::ANY) {
            throw std::runtime_error("LHS of ModifiesP/UsesP cannot be ANY");
        }

        // RHS must be VARIABLE
        auto right_entity_ref = right_ref.get_entity_ref();
        validate_entity_ref(right_entity_ref, {DesignEntityType::VARIABLE});
        break;
    }
    case SuchThatType::MODIFIES_S:
    case SuchThatType::USES_S: {
        // LHS must be not be ANY and must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        if (left_statement_ref.get_type() == StatementRefType::ANY) {
            throw std::runtime_error("LHS of ModifiesS/UsesS cannot be ANY");
        }

        // RHS must be VARIABLE
        auto right_entity_ref = right_ref.get_entity_ref();
        validate_entity_ref(right_entity_ref, {DesignEntityType::VARIABLE});
        break;
    }
    case SuchThatType::CALLS:
    case SuchThatType::CALLS_T: {
        // LHS must be PROCEDURE
        auto left_entity_ref = left_ref.get_entity_ref();
        validate_entity_ref(left_entity_ref, {DesignEntityType::PROCEDURE});

        // RHS must be PROCEDURE
        auto right_entity_ref = right_ref.get_entity_ref();
        validate_entity_ref(right_entity_ref, {DesignEntityType::PROCEDURE});
        break;
    }
    case SuchThatType::AFFECTS:
    case SuchThatType::AFFECTS_T: {
        std::vector<DesignEntityType> expected_types = {
            DesignEntityType::STMT, DesignEntityType::PROG_LINE, DesignEntityType::ASSIGN};

        // LHS must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        validate_statement_ref(left_statement_ref, expected_types);

        // RHS must be valid statement ref
        auto right_statement_ref = right_ref.get_statement_ref();
        validate_statement_ref(right_statement_ref, expected_types);
        break;
    }
    case SuchThatType::PARENT:
    case SuchThatType::PARENT_T: {
        // LHS must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        validate_statement_ref(left_statement_ref,
                               {DesignEntityType::STMT, DesignEntityType::PROG_LINE,
                                DesignEntityType::IF, DesignEntityType::WHILE});

        // RHS must be valid statement ref
        auto right_statement_ref = right_ref.get_statement_ref();
        validate_statement_ref(right_statement_ref);
        break;
    }

    case SuchThatType::FOLLOWS:
    case SuchThatType::FOLLOWS_T:
    case SuchThatType::NEXT:
    case SuchThatType::NEXT_T: {
        // LHS must be valid statement ref
        auto left_statement_ref = left_ref.get_statement_ref();
        validate_statement_ref(left_statement_ref);

        // RHS must be valid statement ref
        auto right_statement_ref = right_ref.get_statement_ref();
        validate_statement_ref(right_statement_ref);
        break;
    }
    default:
        throw std::runtime_error("Unhandled such that type");
    }
}

void PQLParser::validate_pattern(const Pattern& pattern) {
    switch (pattern.get_type()) {
    case PatternType::ASSIGN: {
        auto pattern_assign = pattern.get_pattern_assign();

        // PatternAssign synonym must be ASSIGN
        auto synonym = pattern_assign.get_assign_synonym();
        expect_declaration(synonym, {DesignEntityType::ASSIGN});

        // LHS must be VARIABLE
        auto left_entity_ref = pattern_assign.get_entity_ref();
        validate_entity_ref(left_entity_ref, {DesignEntityType::VARIABLE});
        break;
    }
    case PatternType::WHILE: {
        auto pattern_while = pattern.get_pattern_while();

        // PatternWhile synonym must be WHILE
        auto synonym = pattern_while.get_while_synonym();
        expect_declaration(synonym, {DesignEntityType::WHILE});

        // LHS must be VARIABLE
        auto left_entity_ref = pattern_while.get_entity_ref();
        validate_entity_ref(left_entity_ref, {DesignEntityType::VARIABLE});
        break;
    }
    case PatternType::IF: {
        auto pattern_if = pattern.get_pattern_if();

        // PatternIf synonym must be IF
        auto synonym = pattern_if.get_if_synonym();
        expect_declaration(synonym, {DesignEntityType::IF});

        // LHS must be VARIABLE
        auto left_entity_ref = pattern_if.get_entity_ref();
        validate_entity_ref(left_entity_ref, {DesignEntityType::VARIABLE});
        break;
    }
    default:
        throw std::runtime_error("Unhandled pattern type");
    }
}

void PQLParser::validate_elem(const Elem& elem) {
    switch (elem.get_type()) {
    case ElemType::SYNONYM:
        expect_declaration(elem.get_synonym());
        break;
    case ElemType::ATTR_REF:
        validate_attr_ref(elem.get_attr_ref());
        break;
    default:
        throw std::runtime_error("Unhandled elem type");
    }
}

void PQLParser::validate_statement_ref(const StatementRef& statement_ref) {
    std::vector<DesignEntityType> expected_types = {
        DesignEntityType::STMT,  DesignEntityType::READ,     DesignEntityType::PRINT,
        DesignEntityType::WHILE, DesignEntityType::IF,       DesignEntityType::ASSIGN,
        DesignEntityType::CALL,  DesignEntityType::PROG_LINE};

    validate_statement_ref(statement_ref, expected_types);
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
        throw std::runtime_error("Unhandled statement ref type");
    }
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
        throw std::runtime_error("Unhandled entity ref type");
    }
}

DesignEntity PQLParser::expect_declaration(const std::string& synonym) {
    auto declarations = query_object.get_declarations();
    if (!declarations.has(synonym)) {
        throw std::runtime_error("Declaration for synonym '" + synonym + "' doesn't exist");
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
    throw std::runtime_error("Declaration for synonym '" + synonym +
                             "' doesn't match the expected type");
}
