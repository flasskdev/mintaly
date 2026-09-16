#include <core/features/combat/ballistics.hpp>
#include <cassert>
#include <limits>

namespace {
    struct vector { float x{}, y{}, z{}; };
    using namespace features::combat::ballistics;

    void geometry()
    {
        const vector low{-1, -1, -1}, high{1, 1, 1};
        float fraction{};
        assert(segment_box(vector{-2, 0, 0}, vector{4, 0, 0}, low, high, fraction));