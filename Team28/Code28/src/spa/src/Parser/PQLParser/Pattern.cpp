#include "Pattern.h"
#include <stdexcept>

using namespace Parser;

Pattern::Pattern() = default;

void Pattern::set_type(PatternType new_type) { type = new_type; }

void Pattern::set_pattern_assign(const PatternAssign& new_pattern_assign) {
    pattern_assign = new_pattern_assign;
}

void Pattern::set_pattern_while(const PatternWhile& new_pattern_while) {
    pattern_while = new_pattern_while;
}

void Pattern::set_pattern_if(const PatternIf& new_pattern_if) { pattern_if = new_pattern_if; }

PatternType Pattern::get_type() const { return type; }

PatternAssign Pattern::get_pattern_assign() const {
    if (type != PatternType::ASSIGN) {
        throw std::runtime_error("PatternType is not ASSIGN type");
    }
    return pattern_assign;
}

PatternWhile Pattern::get_pattern_while() const {
    if (type != PatternType::WHILE) {
        throw std::runtime_error("PatternType is not WHILE type");
    }
    return pattern_while;
}

PatternIf Pattern::get_pattern_if() const {
    if (type != PatternType::IF) {
        throw std::runtime_error("PatternType is not IF type");
    }
    return pattern_if;
}

bool Pattern::operator==(const Pattern& other) const {
    return type == other.type && pattern_assign == other.pattern_assign &&
           pattern_while == other.pattern_while && pattern_if == other.pattern_if;
}

std::size_t PatternHash::operator()(const Pattern& pattern) const {
    std::hash<int> int_hash;
    PatternAssignHash pattern_assign_hash;
    PatternWhileHash pattern_while_hash;
    PatternIfHash pattern_if_hash;
    switch (pattern.get_type()) {
    case PatternType::INVALID:
        return int_hash(1);
    case PatternType::ASSIGN:
        return int_hash(2) ^ pattern_assign_hash(pattern.get_pattern_assign());
    case PatternType::WHILE:
        return int_hash(3) ^ pattern_while_hash(pattern.get_pattern_while());
    case PatternType::IF:
        return int_hash(4) ^ pattern_if_hash(pattern.get_pattern_if());
    default:
        throw std::runtime_error("Unknown pattern type");
    }
}