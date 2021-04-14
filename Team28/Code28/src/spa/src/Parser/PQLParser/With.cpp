#include "With.h"

using namespace Parser;

With::With() = default;

void With::set_left_ref(const WithRef& new_left_ref) { left_ref = new_left_ref; }

void With::set_right_ref(const WithRef& new_right_ref) { right_ref = new_right_ref; }

WithRef With::get_left_ref() const { return left_ref; }

WithRef With::get_right_ref() const { return right_ref; }

bool With::operator==(const With& other) const {
    return left_ref == other.left_ref && right_ref == other.right_ref;
}

std::size_t WithHash::operator()(const With& with) const {
    WithRefHash with_ref_hash;
    return with_ref_hash(with.get_left_ref()) ^ with_ref_hash(with.get_right_ref());
}