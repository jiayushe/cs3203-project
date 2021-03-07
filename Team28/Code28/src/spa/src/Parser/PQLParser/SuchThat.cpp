#include "SuchThat.h"

using namespace Parser;

SuchThat::SuchThat() : type(SuchThatType::INVALID) {}

void SuchThat::set_type(SuchThatType new_type) { type = new_type; }

void SuchThat::set_left_ref(const SuchThatRef& new_left_ref) { left_ref = new_left_ref; }

void SuchThat::set_right_ref(const SuchThatRef& new_right_ref) { right_ref = new_right_ref; }

SuchThatType SuchThat::get_type() const { return type; }

SuchThatRef SuchThat::get_left_ref() const { return left_ref; }

SuchThatRef SuchThat::get_right_ref() const { return right_ref; }