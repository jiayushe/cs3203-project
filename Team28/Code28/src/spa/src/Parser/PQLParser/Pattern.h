#pragma once

#include "PatternAssign.h"
#include "PatternIf.h"
#include "PatternWhile.h"

namespace Parser {

enum class PatternType {
    INVALID,
    ASSIGN,
    WHILE,
    IF,
};

class Pattern {
public:
    Pattern();

    PatternType get_type() const;
    PatternAssign get_pattern_assign() const;
    PatternWhile get_pattern_while() const;
    PatternIf get_pattern_if() const;

    void set_type(PatternType type);
    void set_pattern_assign(const PatternAssign& pattern_assign);
    void set_pattern_while(const PatternWhile& pattern_while);
    void set_pattern_if(const PatternIf& pattern_if);

    bool operator==(const Pattern& other) const;

private:
    PatternType type;
    PatternAssign pattern_assign;
    PatternWhile pattern_while;
    PatternIf pattern_if;
};

class PatternHash {
public:
    std::size_t operator()(const Pattern& pattern) const;
};

} // namespace Parser
