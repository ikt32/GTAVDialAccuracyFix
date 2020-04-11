#pragma once

namespace Math {
    template <typename T>
    T Lerp(T a, T b, T f) {
        return a + f * (b - a);
    }
}
