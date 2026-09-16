#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <utilities/utf8.hpp>

namespace nickname_animation {
    inline constexpr std::array<const char*, 8> names{
        "marquee (scroll)", "typewriter", "dancing wave", "cyber glitch", "star pulse",
        "reverse scroll", "scanner", "pulse dots"
    };

    inline std::string frame(std::string_view name, int type, std::uint64_t step) {
        auto chars = utf8::characters(name);
        if (chars.empty()) return {};
        const auto length = chars.size();
        const auto join = [](const auto& parts) {
            std::string text;
            for (const auto& part : parts) text += part;
            return text;
        };
        switch (std::clamp(type, 0, static_cast<int>(names.size()) - 1)) {
        case 0:
        case 5: {
            chars.emplace_back(" ");
            auto offset = static_cast<std::size_t>(step % chars.size());
            if (type == 5) offset = (chars.size() - offset) % chars.size();
            std::rotate(chars.begin(), chars.begin() + offset, chars.end());
            break;
        }
        case 1: {
            const auto phase = step % (length * 2);
            chars.resize(std::max<std::size_t>(1, phase <= length ? phase : length * 2 - phase));
            break;
        }
        case 2:
            for (std::size_t i = 0; i < length; ++i) {
                if (chars[i].size() != 1) continue;
                auto& c = chars[i][0];
                if ((i + step % 4) % 4 < 2) {
                    if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
                } else if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
            }
            break;
        case 3: {
            constexpr std::string_view glyphs = "01!@#$%^&*<>~";
            for (std::size_t i = 0; i < length; ++i)
                if ((i * 7 + (step % 5) * 3) % 5 == 0 && chars[i] != " ")
                    chars[i] = std::string(1, glyphs[(i + step % glyphs.size()) % glyphs.size()]);
            break;
        }
        case 4: {
            constexpr std::array<std::string_view, 6> left{"[ ", "*[ ", "**[ ", "> ", ">> ", "-= "};
            constexpr std::array<std::string_view, 6> right{" ]", " ]*", " ]**", " <", " <<", " =-"};
            return std::string(left[step % left.size()]) + join(chars) + std::string(right[step % right.size()]);
        }
        case 6: {
            const auto cycle = length > 1 ? 2 * (length - 1) : 1;
            const auto phase = step % cycle;
            const auto index = phase < length ? phase : cycle - phase;
            chars[index] = "[" + chars[index] + "]";
            break;
        }
        case 7: {
            const auto phase = step % 6;
            const std::string dots(phase <= 3 ? phase : 6 - phase, '.');
            return dots + join(chars) + dots;
        }
        }
        return join(chars);
    }
}
