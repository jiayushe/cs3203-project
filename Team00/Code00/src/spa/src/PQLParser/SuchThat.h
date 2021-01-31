//
// Created by Victor Varian on 27/1/21.
//

#ifndef PQLPARSER_SUCHTHAT_H
#define PQLPARSER_SUCHTHAT_H

#include "Ref.h"

namespace PQLParser {
    enum class SuchThatType {
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

        SuchThat(SuchThatType type, Ref left_ref, Ref right_ref);

        ~SuchThat();

    private:
        SuchThatType type;
        Ref left_ref;
        Ref right_ref;

    };

}

#endif //PQLPARSER_SUCHTHAT_H
