#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace source_resource {
inline std::uint32_t u32(std::span<const std::byte> bytes, std::size_t offset) {
    std::uint32_t value{};
    if (offset <= bytes.size() && bytes.size() - offset >= sizeof(value))
        std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

// Source 2 block offsets are relative to the offset field, not the file header.
inline std::optional<std::span<const std::byte>> data_block(std::span<const std::byte> bytes) {
    if (bytes.size() < 16) return std::nullopt;
    std::uint16_t version{};
    std::memcpy(&version, bytes.data() + 4, sizeof(version));
    const auto count = u32(bytes, 12);
    const std::uint64_t table = 8ull + u32(bytes, 8);
    if (version != 12 || count == 0 || count > 64 || table > bytes.size() ||
        count * 12ull > bytes.size() - table) return std::nullopt;
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto entry = static_cast<std::size_t>(table + i * 12ull);
        if (u32(bytes, entry) != 0x41544144u) continue;
        const std::uint64_t start = entry + 4ull + u32(bytes, entry + 4);
        const auto length = u32(bytes, entry + 8);
        if (start > bytes.size() || length > bytes.size() - start) return std::nullopt;
        return bytes.subspan(static_cast<std::size_t>(start), length);
    }
    return std::nullopt;
}

inline std::string image_key(std::string path) {
    for (auto& c : path) if (c == '\\') c = '/';
    constexpr std::string_view prefix = "panorama/images/";
    if (path.starts_with(prefix)) path.erase(0, prefix.size());
    for (const auto suffix : {std::string_view(".vtex_c"), std::string_view(".vtex")}) {
        if (path.ends_with(suffix)) { path.resize(path.size() - suffix.size()); break; }
    }
    if (path.ends_with(".png")) path.replace(path.size() - 4, 4, "_png");
    if (!path.ends_with("_png")) path += "_png";
    return path;
}
}
