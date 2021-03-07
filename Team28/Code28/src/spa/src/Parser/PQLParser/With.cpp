#include "With.h"

using namespace Parser;

With::With() = default;

void With::set_left_ref(const WithRef& new_left_ref) { left_ref = new_left_ref; }

void With::set_right_ref(const WithRef& new_right_ref) { right_ref = new_right_ref; }

WithRef With::get_left_ref() const { return left_ref; }

WithRef With::get_right_ref() const { return right_ref; }