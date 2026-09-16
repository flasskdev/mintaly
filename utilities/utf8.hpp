#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace utf8 {
    // Drop malformed sequences instead of splitting a multibyte character.
    inline std::vector<std::string> characters(std::string_view text) {
        std::vector<std::string> result;
        for (std::size_t i = 0; i < text.size();) {
            const auto lead = static_cast<unsigned char>(text[i]);
            const std::size_t length = lead < 0x80 ? 1 :
                (lead >= 0xc2 && lead <= 0xdf ? 2 :
                (lead >= 0xe0 && lead <= 0xef ? 3 :
                (lead >= 0xf0 && lead <= 0xf4 ? 4 : 0)));
            if (!length || i + length > text.size()) { ++i; continue; }
            std::uint32_t code = lead & (length == 1 ? 0x7f : (0x7f >> length));
            bool valid = true;
            for (std::size_t j = 1; j < length; ++j) {
                const auto byte = static_cast<unsigned char>(text[i + j]);
                if ((byte & 0xc0) != 0x80) { valid = false; break; }
                code = (code << 6) | (byte & 0x3f);
            }
            if (!valid || (length == 2 && code < 0x80) || (length == 3 && code < 0x800) ||
                (length == 4 && code < 0x10000) || code > 0x10ffff || (code >= 0xd800 && code <= 0xdfff)) {
                ++i;
                continue;
            }
            result.emplace_back(text.substr(i, length));
            i += length;
        }
        return result;
    }

    inline std::string bounded(std::string_view text, std::size_t max_characters, std::size_t max_bytes) {
        std::string result;
        std::size_t count = 0;
        for (const auto& character : characters(text)) {
            const auto first = static_cast<unsigned char>(character.front());
            if (first < 0x20 || first == 0x7f) continue;
            if (count == max_characters || result.size() + character.size() > max_bytes) break;
            result += character;
            ++count;
        }
        return result;
    }
}
