#include <utilities/nickname_animation.hpp>
#include <utilities/skin_options.hpp>
#include <cassert>
#include <limits>

int main() {
    const std::string unicode = "\xd0\x90\xf0\x9f\x98\x80";
    assert(utf8::characters(unicode).size() == 2);
    assert(utf8::bounded(unicode, 2, 5) == unicode.substr(0, 2));
    assert(skin_options::normalize_name_tag(std::string(21, 'a')) == std::string(20, 'a'));
    assert(skin_options::normalize_name_tag("a\nb\rc") == "abc");
    assert(skin_options::normalize_name_tag("").empty());
    assert(utf8::characters("\xc0\xaf\xed\xa0\x80\xf4\x90\x80\x80").empty());
    assert(nickname_animation::frame("abc", 0, 1) == "bc a");
    assert(nickname_animation::frame("abc", 5, 1) == " abc");
    assert(nickname_animation::frame(unicode, 1, 1) == unicode.substr(0, 2));
    for (int type = 0; type < static_cast<int>(nickname_animation::names.size()); ++type) {
        assert(nickname_animation::frame("", type, 0).empty());
        for (std::uint64_t step = 0; step < 64; ++step) {
            const auto text = nickname_animation::frame(unicode, type, step);
            std::string rebuilt;
            for (const auto& c : utf8::characters(text)) rebuilt += c;
            assert(!text.empty() && rebuilt == text);
        }
        assert(!nickname_animation::frame("A", type, std::numeric_limits<std::uint64_t>::max()).empty());
    }
}
