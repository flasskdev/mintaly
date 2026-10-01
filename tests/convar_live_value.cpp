#include <utilities/addresses/interfaces.hpp>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>

int main()
{
    static_assert(sizeof(void*) == 8, "The observed engine layout is x64");
    static_assert(offsetof(c_convar, m_value) == 0x58);
    static_assert(offsetof(c_convar, _value_tail) == 0x60);
    static_assert(offsetof(c_convar, m_flags) == 0x30);
    static_assert(std::is_trivially_copyable_v<c_convar>);

    c_convar value{};
    value.m_type = 7; // Observed float ConVar type in the runtime capture.
    value.m_value.fl = 0.0f;
    auto* bytes = reinterpret_cast<unsigned char*>(&value);
    const std::uint32_t legacy_word = 0xffffffffu;
    std::memcpy(bytes + 0x48, &legacy_word, sizeof(legacy_word));
    const float unchanged_tail = 1.0f;
    std::memcpy(bytes + 0x60, &unchanged_tail, sizeof(unchanged_tail));

    // Reproduce both captures, then check non-default and zero server settings.
    for (float current : {1.0f, 0.75f, 0.25f, 0.0f, 2.5f}) {
        std::memcpy(bytes + 0x58, &current, sizeof(current));
        assert(value.get<float>() == current);
        float tail{};
        std::memcpy(&tail, bytes + 0x60, sizeof(tail));
        assert(tail == 1.0f);
    }
    // Do not hide a genuinely invalid current value with a fallback constant.
    value.m_value.fl = std::numeric_limits<float>::quiet_NaN();
    assert(std::isnan(value.get<float>()));

    // Local union accessor regression only, not runtime proof for other types.
    value.m_value.i1 = true;
    assert(value.get<bool>());
    value.m_value.i16 = -12;
    assert(value.get<short>() == -12);
    value.m_value.i32 = 800;
    assert(value.get<int>() == 800);
    value.m_value.i64 = 0x123456789ll;
    assert(value.get<int64_t>() == 0x123456789ll);
}
