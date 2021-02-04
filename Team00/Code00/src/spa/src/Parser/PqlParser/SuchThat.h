#pragma once

#include "Ref.h"

namespace Parser {
    enum SuchThatType {
        MODIFIES_S,
        USES_S,
        PARENT,
        PARENT_T,
        FOLLOWS,
        FOLLOWS_T,
    };

    class SuchThat {
    public:
        SuchThatType get_type();
        Ref get_left_ref();
        Ref get_right_ref();

        SuchThat();

        ~SuchThat();

        void set_type(SuchThatType type);

        void set_left_ref(Ref left_ref);

        void set_right_ref(Ref right_ref);

    private:
        SuchThatType type;
        Ref left_ref;
        Ref right_ref;
    };

} // namespace Parser
