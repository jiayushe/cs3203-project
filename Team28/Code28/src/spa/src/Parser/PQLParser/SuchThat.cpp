#include "SuchThat.h"

using namespace Parser;

SuchThat::SuchThat() : type(SuchThatType::INVALID) {}

void SuchThat::set_type(SuchThatType new_type) { type = new_type; }

void SuchThat::set_left_ref(const SuchThatRef& new_left_ref) { left_ref = new_left_ref; }

void SuchThat::set_right_ref(const SuchThatRef& new_right_ref) { right_ref = new_right_ref; }

SuchThatType SuchThat::get_type() const { return type; }

SuchThatRef SuchThat::get_left_ref() const { return left_ref; }

SuchThatRef SuchThat::get_right_ref() const { return right_ref; }

bool SuchThat::operator==(const SuchThat& other) const {
    return type == other.get_type() && left_ref == other.left_ref && right_ref == other.right_ref;
}

std::size_t SuchThatHash::operator()(const SuchThat& such_that) const {
    SuchThatRefHash such_that_ref_hash;
    return static_cast<std::size_t>(such_that.get_type()) ^
           such_that_ref_hash(such_that.get_left_ref()) ^
           such_that_ref_hash(such_that.get_right_ref());
}