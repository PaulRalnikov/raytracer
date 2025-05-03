#pragma once
#include <algorithm>

void inline sort(float& x, float& y) {
    if (x > y) {
        std::swap(x, y);
    }
}
