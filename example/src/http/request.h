#pragma once

#include <mstring.h>

struct HTTPRequest {
    _Atomic string  url, result;
    _Atomic bool    done;
    
};
