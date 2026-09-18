#include <utilities/source_resource.hpp>
#include <array>
#include <cassert>
#include <limits>

int main() {
    using namespace source_resource;
    assert(image_key("econ/weapons/ak47") == "econ/weapons/ak47_png");
    assert(image_key("panorama/images/econ/weapons/ak47_png.vtex_c") == "econ/weapons/ak47_png");
    assert(image_key("econ\\weapons\\ak47.png") == "econ/weapons/ak47_png");
    std::array<std::byte, 80> bytes{};
    auto put = [&](std::size_t at, std::uint32_t value) { std::memcpy(bytes.data() + at, &value, 4); };
    put(4, 12); put(8, 24); put(12, 1); // Block table at 32, not 16.
    put(32, 0x41544144); put(36, 12); put(40, 20);
    auto block = data_block(bytes);
    assert(block && block->data() == bytes.data() + 48 && block->size() == 20);
    put(36, std::numeric_limits<std::uint32_t>::max());
    assert(!data_block(bytes));
    put(36, 12); put(40, 40);
    assert(!data_block(bytes));
    put(40, 20); put(12, 65);
    assert(!data_block(bytes));
    assert(!data_block(std::span<const std::byte>(bytes).first(8)));
}
