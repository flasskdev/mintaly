#include <utilities/cosmetic_config.hpp>
#include <cassert>
#include <limits>

int main() {
    using nlohmann::json;
    using namespace cosmetic_config;
    assert(integer(json(65534), 0, 65534) == 65534);
    for (const auto& invalid : {json(-1), json(65535), json(1.5), json("12"), json(true), json(nullptr),
             json(std::numeric_limits<std::uint64_t>::max())})
        assert(integer(invalid, 0, 65534) == 0);
    assert(field(json::object(), "ct", 0, 32767) == 0);
    assert(field(json{{"ct", 65537}}, "ct", 0, 32767) == 0);
    assert(retain_team(3, 0) == 3);
    assert(retain_team(2, 1) == 2);
    assert(retain_team(3, 2) == 2);
    assert(retain_team(0, 3) == 3);

    json profile = {
        {"selected_ct", 2}, {"selected_t", 3},
        {"entries", json::array({nullptr,
            {{"model_path", ""}},
            {{"name", "CT"}, {"model_path", "characters/ct.vmdl"}, {"team", 3}},
            {{"name", "T"}, {"model", "characters/t.vmdl"}, {"team", 2}}})}
    };
    const auto decoded = decode_custom_agents(profile);
    assert(decoded.entries.size() == 2);
    assert(decoded.selected_ct == 0 && decoded.selected_t == 1);
    assert(decoded.entries[1].model_path == "characters/t.vmdl");

    profile["selected_ct"] = 3; // Wrong-side selection is not carried over.
    profile["selected_t"] = 200;
    auto invalid = decode_custom_agents(profile);
    assert(invalid.selected_ct == -1 && invalid.selected_t == -1);
    profile["selected_ct"] = 1; // Removed entry must not select its successor.
    invalid = decode_custom_agents(profile);
    assert(invalid.selected_ct == -1);

    json saved = {{"selected_ct", decoded.selected_ct}, {"selected_t", decoded.selected_t}, {"entries", json::array()}};
    for (const auto& e : decoded.entries)
        saved["entries"].push_back({{"name", e.name}, {"model_path", e.model_path}, {"team", e.team}});
    const auto roundtrip = decode_custom_agents(json::parse(saved.dump()));
    assert(roundtrip.entries == decoded.entries);
    assert(roundtrip.selected_ct == decoded.selected_ct && roundtrip.selected_t == decoded.selected_t);
    assert(decode_custom_agents(json::array()).entries.empty());
    assert(decode_custom_agents(json{{"entries", "invalid"}}).selected_ct == -1);
    profile["entries"][2]["team"] = 1;
    assert(decode_custom_agents(profile).entries.size() == 1);
}
