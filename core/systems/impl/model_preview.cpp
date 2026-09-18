#include <pch/pch.hpp>
#include <core/systems/native_preview.hpp>

namespace systems {

bool model_preview::request::same_asset(const request& o) const {
    return type == o.type && def_index == o.def_index && paint_kit == o.paint_kit &&
        music_kit == o.music_kit && team == o.team && model_path == o.model_path &&
        width == o.width && height == o.height;
}

bool model_preview::initialize() {
    std::lock_guard lock(mutex_);
    initialized_ = true;
    capture_available_ = false;
    visible_ = false;
    status_ = "2D Preview Only";
    return true;
}

void model_preview::set_capture_available(bool) {
}

void model_preview::submit(request) {
}

void model_preview::hide() {
}

void model_preview::update() {
}

void model_preview::reset() {
}

void model_preview::shutdown() {
    std::lock_guard lock(mutex_);
    initialized_ = false;
    visible_ = false;
    capture_available_ = false;
}

model_preview::texture_ref model_preview::acquire(ID3D11Device*) {
    return {};
}

std::string model_preview::status() const {
    return "2D Preview Only";
}

void model_preview::capture_resource(std::uintptr_t, char, ID3D11ShaderResourceView*, const char*) {
}

std::uintptr_t model_preview::resource_view_address() {
    return 0;
}

}
