#pragma once
#include <string>
#include <string_view>

namespace cosmetic_model {
    [[nodiscard]] inline std::string canonical(std::string_view path) {
        std::string result(path);
        for (auto& c : result) {
            if (c == '\\') c = '/';
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
        }
        if (result.ends_with(".vmdl_c")) result.resize(result.size() - 2);
        return result;
    }
    [[nodiscard]] inline bool matches(std::string_view current, std::string_view requested) {
        return !current.empty() && !requested.empty() && canonical(current) == canonical(requested);
    }
}
