#pragma once

#include "WithRef.h"

namespace Parser {

class With {
public:
    With();

    WithRef get_left_ref() const;
    WithRef get_right_ref() const;

    void set_left_ref(const WithRef& left_ref);
    void set_right_ref(const WithRef& right_ref);

private:
    WithRef left_ref;
    WithRef right_ref;
};

} // namespace Parser
