#pragma once

#include "Ref.h"

namespace Parser {
enum class SuchThatType {
    INVALID,
    MODIFIES_S,
    USES_S,
    PARENT,
    PARENT_T,
    FOLLOWS,
    FOLLOWS_T,
};

class SuchThat {
public:
    SuchThatType get_type() const;
    Ref get_left_ref() const;
    Ref get_right_ref() const;

    SuchThat();

    void set_type(SuchThatType type);
    void set_left_ref(const Ref& left_ref);
    void set_right_ref(const Ref& right_ref);

private:
    SuchThatType type;
    Ref left_ref;
    Ref right_ref;
};

} // namespace Parser
