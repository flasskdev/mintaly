#include <utilities/mvp_music.hpp>
#include <cassert>
#include <vector>

int main() {
    using namespace std::chrono_literals;
    using resolver = mvp_music::resolver;
    const auto now = resolver::clock::time_point{} + 1s;
    std::vector<std::uint16_t> heard;
    const auto play = [&](std::uint16_t kit) { heard.push_back(kit); };
    const auto run = [](std::optional<resolver::playback> request) {
        if (request) request->play(request->kit);
    };
    resolver state;
    state.set_winner(42, now);
    run(state.submit(7, play, now)); // Event before playback; listener kit must not win.
    assert((heard == std::vector<std::uint16_t>{42}));
    assert(!state.poll(now));

    state.clear();
    heard.clear();
    run(state.submit(7, play, now)); // Engine listener runs first.
    assert(heard.empty());
    assert(!state.poll(now + 1ms));
    state.set_winner(42, now + 2ms);
    run(state.poll(now + 3ms));
    assert((heard == std::vector<std::uint16_t>{42}));
    assert(!state.poll(now + 4ms)); // No duplicate replay.

    state.clear();
    heard.clear();
    run(state.submit(9, play, now));
    assert(!state.poll(now + 99ms));
    run(state.poll(now + 100ms)); // Missing event: original engine choice.
    assert((heard == std::vector<std::uint16_t>{9}));
    state.set_winner(42, now + 101ms);
    assert(!state.poll(now + 102ms)); // Late event does not play twice.

    for (int invalid : {0, -1, 65535, 65536}) {
        state.clear();
        state.set_winner(invalid, now);
        run(state.submit(0xffff, play, now));
        assert(heard.back() == 0xffff);
    }
    state.clear();
    run(state.submit(0, play, now));
    run(state.poll(now + 100ms));
    assert(heard.back() == 0);

    state.clear();
    run(state.submit(7, play, now));
    state.clear(); // Round start / disconnect / map change cancels captured callback.
    assert(!state.poll(now + 100ms));
    assert(state.winner_kit(now) == 0);
    run(state.submit(7, play, now));
    assert(!state.poll(now + 2s)); // Stalled game loop drops stale context.

    state.clear();
    state.set_winner(42, now);
    assert(!state.submit(7, play, now + 26s)); // Expired winner must not leak.
    run(state.poll(now + 26s + 100ms));
    assert(heard.back() == 7);

    state.clear();
    assert(!state.submit(7, play, now));
    run(state.submit(9, play, now + 1ms));
    assert(heard.back() == 7); // Bounded queue preserves displaced request.
    run(state.poll(now + 101ms));
    assert(heard.back() == 9);

    resolver client_a, client_b;
    std::uint16_t heard_a{}, heard_b{};
    for (int winner : {7, 42}) {
        client_a.clear();
        client_b.clear();
        run(client_a.submit(7, [&](std::uint16_t kit) { heard_a = kit; }, now));
        client_b.set_winner(winner, now);
        run(client_b.submit(42, [&](std::uint16_t kit) { heard_b = kit; }, now));
        client_a.set_winner(winner, now + 1ms);
        run(client_a.poll(now + 2ms));
        assert(heard_a == winner && heard_b == winner);
    }
}
