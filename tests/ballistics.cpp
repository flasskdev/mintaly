#include <core/features/combat/ballistics.hpp>
#include <cassert>
#include <limits>

namespace {
    struct vector { float x{}, y{}, z{}; };
    using namespace features::combat::ballistics;

    void hitbox_lookup()
    {
        struct entry { int index; };
        const std::array<entry, 8> entries{{{4}, {0}, {19}, {4}, {27}, {-2}, {27}, {2}}};
        const indexed_view<entry, 20> lookup{std::span<const entry>{entries}};
        // Compare exact addresses: duplicate IDs must retain the FIRST match.
        for (int id = -5; id <= 40; ++id)
        {
            const entry* expected = nullptr;
            for (const auto& value : entries)
            {
                if (value.index == id)
                {
                    expected = &value;
                    break;
                }
            }
            assert(lookup.find(id) == expected);
        }
        const indexed_view<entry, 20> empty{std::span<const entry>{}};
        assert(!empty.find(0) && !empty.find(19) && !empty.find(-1) && !empty.find(20));
        const indexed_view<entry, 0> fallback{std::span<const entry>{entries}};
        assert(fallback.find(4) == &entries[0]);
        assert(fallback.find(27) == &entries[4]);
        assert(!lookup.find(std::numeric_limits<int>::min()));
        assert(!lookup.find(std::numeric_limits<int>::max()));
        const auto copy = lookup;
        assert(copy.find(19) == &entries[2]);
    }

    void geometry()
    {
        const vector low{-1, -1, -1}, high{1, 1, 1};
        float fraction{};
        assert(segment_box(vector{-2, 0, 0}, vector{4, 0, 0}, low, high, fraction));
        assert(fraction == 0.25f);
        assert(segment_box(vector{2, 0, 0}, vector{-4, 0, 0}, low, high, fraction));
        assert(fraction == 0.25f);
        assert(!segment_box(vector{-3, 0, 0}, vector{1, 0, 0}, low, high, fraction));
        assert(segment_box(vector{-2, 0, 0}, vector{1, 0, 0}, low, high, fraction));
        assert(fraction == 1.0f);
        assert(segment_box(vector{}, vector{}, low, high, fraction));
        assert(fraction == 0.0f);
        assert(!segment_box(vector{2, 0, 0}, vector{}, low, high, fraction));
        assert(!segment_box(vector{-2, 2, 0}, vector{4, 0, 0}, low, high, fraction));
        assert(segment_box(vector{-2, 1, 0}, vector{4, 0, 0}, low, high, fraction));
        assert(!segment_box(vector{}, vector{1, 0, 0}, high, low, fraction));
        const auto nan = std::numeric_limits<float>::quiet_NaN();
        const auto inf = std::numeric_limits<float>::infinity();
        assert(!segment_box(vector{nan, 0, 0}, vector{1, 0, 0}, low, high, fraction));
        assert(!segment_box(vector{}, vector{inf, 0, 0}, low, high, fraction));
        assert(!segment_box(vector{}, vector{1, 0, 0}, vector{nan, -1, -1}, high, fraction));
        // Small nonzero motion is not the same thing as a parallel segment.
        assert(segment_box(vector{-2e-9f, 0, 0}, vector{2e-9f, 0, 0},
            vector{-1e-9f, -1, -1}, vector{1e-9f, 1, 1}, fraction));
        assert(std::fabs(fraction - 0.5f) < 1e-6f);
    }

    void probabilities()
    {
        int calls = 0;
        assert(sample_ratio(0, [&](int) { ++calls; return true; }) == 0.0f);
        assert(calls == 0);
        assert(sample_ratio(128, [](int i) { return i >= 32; }) == 0.75f);
        assert(sample_ratio(128, [](int i) { return i >= 64; }) == 0.5f);
        assert(sample_ratio(256, [](int i) { return i == 255; }) == 1.0f / 256.0f);
        assert(sample_ratio(1, [](int) { return true; }) == 1.0f);
        assert(sample_ratio(128, [](int) { return false; }) == 0.0f);
        for (int count = 1; count <= 256; ++count)
        {
            for (int prefix = 0; prefix <= count; ++prefix)
                assert(sample_ratio(count, [=](int i) { return i >= prefix; }) ==
                    static_cast<float>(count - prefix) / static_cast<float>(count));
        }
    }

    void score_bounds()
    {
        for (float damage : {1.0f, 25.0f, 99.0f, 100.0f, 150.0f})
        {
            for (float needed : {0.0f, 0.5f, 1.0f})
            {
                const auto upper = target_score(damage, 100, 1, needed, false, true, false, 4, 90);
                auto previous = target_score(damage, 100, 0, needed, false, true, false, 4, 90);
                for (int i = 0; i <= 128; ++i)
                {
                    const auto score = target_score(damage, 100, i / 128.0f, needed, false, true, false, 4, 90);
                    assert(score >= previous && score <= upper);
                    previous = score;
                }
            }
        }
    }

    void command_seed()
    {
        const vector ballistic{20, 30, 45};
        const vector punch{5, 2, 0};
        const auto command = command_angles(ballistic, punch);
        assert(command.x == 15 && command.y == 28 && command.z == 45);

        // The fixed point exists only in command space, after removing punch.
        // Hashing the ballistic pitch instead would reject this solution.
        const auto hash_command = [](const vector& angle)
        {
            return angle.x == 15.0f ? std::uint32_t{7} : std::uint32_t{9};
        };
        int calls = 0;
        const auto result = solve_spread(ballistic,
            [&](const vector& angle) { return hash_command(command_angles(angle, punch)); },
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                ++calls;
                return seed == 7 ? std::optional<vector>{ballistic} : std::nullopt;
            });
        assert(result && calls == 1);
        assert(hash_command(command_angles(*result, punch)) == 7);
        const auto zero_punch = command_angles(ballistic, vector{});
        assert(zero_punch.x == ballistic.x && zero_punch.y == ballistic.y && zero_punch.z == ballistic.z);
    }

    void timestamps()
    {
        auto stamp = normalize_stamp(100, 0.75f, 2, 0.5f);
        assert(stamp && stamp->tick == 103 && stamp->fraction == 0.25f);
        stamp = normalize_stamp(100, -0.25f, -2, 0.0f);
        assert(stamp && stamp->tick == 97 && stamp->fraction == 0.75f);
        stamp = normalize_stamp(100, 1.0f, 0, 0.0f);
        assert(stamp && stamp->tick == 101 && stamp->fraction == 0.0f);
        const auto low = std::numeric_limits<int>::min();
        const auto high = std::numeric_limits<int>::max();
        assert(!normalize_stamp(high, 0.75f, 0, 0.5f));
        assert(!normalize_stamp(low, -0.25f, 0, 0.0f));
        assert(!normalize_stamp(high, 0.0f, 1, 0.0f));
        stamp = normalize_stamp(high, 0.0f, low, 0.0f);
        assert(stamp && stamp->tick == -1 && stamp->fraction == 0.0f);
        const auto nan = std::numeric_limits<float>::quiet_NaN();
        const auto inf = std::numeric_limits<float>::infinity();
        assert(!normalize_stamp(0, nan, 0, 0));
        assert(!normalize_stamp(0, 0, 0, inf));
        assert(!normalize_stamp(0, std::numeric_limits<float>::max(), 0, 0));
        for (int i = -64; i <= 64; ++i)
        {
            stamp = normalize_stamp(100, i / 32.0f, 2, 0.25f);
            assert(stamp && stamp->fraction >= 0 && stamp->fraction < 1);
            assert(stamp->tick + static_cast<double>(stamp->fraction) == 102.25 + i / 32.0);
        }
    }

    void solver()
    {
        const auto seed_for = [](const vector& angle) { return static_cast<std::uint32_t>(angle.x); };
        int calls = 0;
        auto result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t) -> std::optional<vector> { ++calls; return vector{}; });
        assert(result && result->x == 0.0f && calls == 1);

        // Two-step fixed-point convergence from the actual aim seed.
        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                ++calls;
                return vector{seed < 2 ? static_cast<float>(seed + 1) : 2.0f, 0, 0};
            });
        assert(result && result->x == 2 && calls == 3);

        // A 0 <-> 1 cycle must terminate; the grid can still find seed 3.
        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                ++calls;
                if (seed == 3) return vector{3, 0, 0};
                return vector{seed == 0 ? 1.0f : 0.0f, 0, 0};
            });
        assert(result && result->x == 3 && calls == 3);

        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                ++calls;
                return vector{static_cast<float>(seed + 1), 0, 0};
            });
        assert(!result && calls <= 264);

        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t) -> std::optional<vector> { ++calls; return std::nullopt; });
        assert(!result && calls <= 241);

        // The initial seed fails. A grid probe reaches an off-grid fixed point
        // that a single correction per grid sample would never discover.
        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                ++calls;
                if (seed == 3 || seed == 1001) return vector{1001, 0, 0};
                return std::nullopt;
            });
        assert(result && result->x == 1001 && calls == 4);

        // Failed fallback chains remain bounded and never re-evaluate a seed.
        std::array<bool, 2048> evaluated{};
        calls = 0;
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t seed) -> std::optional<vector>
            {
                assert(seed < evaluated.size() && !evaluated[seed]);
                evaluated[seed] = true;
                ++calls;
                if (seed == 0) return std::nullopt;
                return vector{seed < 1000 ? 1000.0f : 1.0f, 0, 0};
            });
        assert(!result && calls <= 264);

        // Quantized grid seeds must not repeat expensive inverse calculations.
        for (const auto repeated : {std::uint32_t{0}, std::numeric_limits<std::uint32_t>::max()})
        {
            calls = 0;
            result = solve_spread(vector{},
                [=](const vector&) { return repeated; },
                [&](std::uint32_t) -> std::optional<vector> { ++calls; return std::nullopt; });
            assert(!result && calls == 1);
        }
        // Deliberately collide all keys in the fixed-size table; none may be lost.
        calls = 0;
        result = solve_spread(vector{},
            [](const vector& angle) { return static_cast<std::uint32_t>(angle.x) * 512u; },
            [&](std::uint32_t) -> std::optional<vector> { ++calls; return std::nullopt; });
        assert(!result && calls == 240);

        const auto nan = std::numeric_limits<float>::quiet_NaN();
        calls = 0;
        result = solve_spread(vector{nan, 0, 0}, seed_for,
            [&](std::uint32_t) -> std::optional<vector> { ++calls; return vector{}; });
        assert(!result && calls == 0);
        result = solve_spread(vector{}, seed_for,
            [&](std::uint32_t) -> std::optional<vector> { return vector{nan, 0, 0}; });
        assert(!result);
    }
}

int main()
{
    hitbox_lookup();
    geometry();
    probabilities();
    score_bounds();
    command_seed();
    timestamps();
    solver();
}
