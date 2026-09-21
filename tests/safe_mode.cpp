#include <utilities/safe_mode.hpp>
#include <cassert>
#include <utility>

int main() {
    using namespace std::literals;
    constexpr std::pair<std::string_view, std::string_view> blocked[]{
        {"ragebot", "enabled"}, {"ragebot - rifles", "silent"},
        {"anti aim", "anti aim"}, {"peek assistance", "quick peek"},
        {"other 'bots'", "zeusbot"}, {"other 'bots'", "knifebot"}, {"autos", "auto scope"},
        {"movement", "airstrafe"}, {"movement", "air strafer"},
        {"movement", "quick stop"}, {"movement", "fastladder"},
        {"movement", "edgebug"}, {"movement", "slowwalk"},
        {"legitbot", "aimbot"}, {"legitbot - rifles", "aimbot"},
        {"legitbot - ak47", "aimbot"}, {"name changer", "clantag"},
        {"misc", "kill say"}, {"misc", "chat spam"}, {"autobuy", "auto buy"},
        {"removals", "remove smoke"}, {"removals", "flash alpha"}, {"trajectory", "super toss"}
    };
    for (const auto& [category, name] : blocked) assert(safe_mode::restricted(category, name));
    assert(!safe_mode::restricted("movement", "bhop"));
    assert(!safe_mode::restricted("movement", "edgejump"));
    assert(!safe_mode::restricted("legitbot - rifles", "triggerbot"));
    assert(!safe_mode::restricted("legitbot - rifles", "standalone rcs"));
    assert(!safe_mode::restricted("trajectory", "projectile trajectory"));
    assert(!safe_mode::restricted("watermark", "watermark"));
    assert(safe_mode::active());
    safe_mode::enabled.store(false);
    assert(!safe_mode::active());
    safe_mode::enabled.store(true);
    assert(safe_mode::active());
}
