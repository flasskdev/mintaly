#include <utilities/state_snapshot.hpp>
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace {
    template <std::size_t Entries, std::size_t Bytes>
    void restoration()
    {
        std::array<std::uint8_t, 64> data{};
        for (std::size_t i = 0; i < data.size(); ++i)
            data[i] = static_cast<std::uint8_t>(i);
        const auto original = data;
        const auto address = reinterpret_cast<std::uintptr_t>(data.data());
        utilities::state_snapshot<Entries, Bytes> snapshot;
        snapshot.save_raw(address, data.size());
        data.fill(91);
        // Overlap with a differently aged snapshot: reverse order is essential.
        snapshot.save_raw(address + 3, 17);
        data.fill(42);
        for (int i = 0; i < 200; ++i)
        {
            snapshot.save_raw(address + i % 64, 1);
            data[i % 64] = static_cast<std::uint8_t>(i);
        }
        snapshot.restore();
        assert(data == original);
        data.fill(7);
        snapshot.restore(); // An explicit restore is idempotent.
        for (const auto byte : data) assert(byte == 7);
        snapshot.save_raw(address, data.size());
        data.fill(8);
        snapshot.restore(); // Reuse after overflow and restore.
        for (const auto byte : data) assert(byte == 7);
        snapshot.save_raw(0, 0);
    }
}

int main()
{
    restoration<160, 4096>();
    restoration<1, 4>();
    restoration<0, 0>();
    restoration<256, 0>();
    restoration<0, 4096>();
    int value = 17;
    try
    {
        utilities::state_snapshot<> snapshot;
        snapshot.save_raw(reinterpret_cast<std::uintptr_t>(&value), sizeof(value));
        value = 99;
        throw std::runtime_error("callback failure");
    }
    catch (const std::runtime_error&) {}
    assert(value == 17);
    {
        utilities::state_snapshot<> outer;
        outer.save_raw(reinterpret_cast<std::uintptr_t>(&value), sizeof(value));
        value = 23;
        {
            utilities::state_snapshot<> inner;
            inner.save_raw(reinterpret_cast<std::uintptr_t>(&value), sizeof(value));
            value = 42;
        }
        assert(value == 23);
    }
    assert(value == 17);
}
