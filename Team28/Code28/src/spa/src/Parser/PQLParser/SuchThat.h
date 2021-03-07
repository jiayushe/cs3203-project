#pragma once

#include "SuchThatRef.h"

namespace Parser {

enum class SuchThatType {
    INVALID,
    MODIFIES_S,
    MODIFIES_P,
    USES_S,
    USES_P,
    PARENT,
    PARENT_T,
    FOLLOWS,
    FOLLOWS_T,
    CALLS,
    CALLS_T,
    NEXT,
    NEXT_T,
    AFFECTS,
    AFFECTS_T
};

class SuchThat {
public:
    SuchThatType get_type() const;
    SuchThatRef get_left_ref() const;
    SuchThatRef get_right_ref() const;

    SuchThat();

    void set_type(SuchThatType type);
    void set_left_ref(const SuchThatRef& left_ref);
    void set_right_ref(const SuchThatRef& right_ref);

private:
    SuchThatType type;
    SuchThatRef left_ref;
    SuchThatRef right_ref;
};

} // namespace Parser
