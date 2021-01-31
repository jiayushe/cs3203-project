//
// Created by Victor Varian on 27/1/21.
//


#include <string>
#include <iostream>
#include "SuchThat.h"

using namespace PQLParser;

SuchThat::SuchThat() {}

SuchThat::SuchThat(SuchThatType type, Ref left_ref, Ref right_ref) :
        type(type),
        left_ref(left_ref),
        right_ref(right_ref) {}

SuchThat::~SuchThat() {

}

SuchThatType SuchThat::get_type() {
    return type;
}

Ref SuchThat::get_left_ref() {
    return left_ref;
}

Ref SuchThat::get_right_ref() {
    return right_ref;
}