#include "SuchThat.h"
#include <string>

using namespace Parser;

SuchThat::SuchThat() {}

SuchThat::~SuchThat() {}

void SuchThat::set_type(SuchThatType type) { this->type = type; }

void SuchThat::set_left_ref(Ref left_ref) { this->left_ref = left_ref; }

void SuchThat::set_right_ref(Ref right_ref) { this->right_ref = right_ref; }

SuchThatType SuchThat::get_type() { return type; }

Ref SuchThat::get_left_ref() { return left_ref; }

Ref SuchThat::get_right_ref() { return right_ref; }