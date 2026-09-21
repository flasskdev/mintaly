#pragma once
#include <atomic>
#include <string_view>

namespace safe_mode {
    // Session-level choice: loading a gameplay profile must not disable this mode.
    inline std::atomic<bool> enabled{true};

    [[nodiscard]] inline bool active() noexcept {
        return enabled.load(std::memory_order_relaxed);
    }

    [[nodiscard]] constexpr bool restricted(std::string_view category, std::string_view name) noexcept {
        if (category == "ragebot" || category.starts_with("ragebot - ") ||
            category == "anti aim" || category == "peek assistance" ||
            category == "autos" || category == "other 'bots'" ||
            category == "zeusbot" || category == "knifebot") return true;
        if ((category == "legitbot" || category.starts_with("legitbot - ")) && name == "aimbot") return true;
        if (category == "movement")
            return name == "airstrafe" || name == "air strafer" || name == "quick stop" ||
                name == "fastladder" || name == "edgebug" || name == "slowwalk";
        return (category == "name changer" && name == "clantag") ||
            (category == "misc" && (name == "kill say" || name == "chat spam")) ||
            (category == "autobuy" && name == "auto buy") ||
            (category == "removals" && (name == "remove smoke" || name == "flash alpha")) ||
            (category == "trajectory" && name == "super toss");
    }
}
