#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <d3d11.h>
#include <wrl/client.h>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>

namespace systems {
// Render-thread value snapshots only. Panorama calls run from update(), on the
// existing frame-stage thread. No live entity or inventory is modified.
class model_preview {
public:
    enum class kind { weapon, knife, gloves, agent, music };
    struct request {
        kind type = kind::weapon;
        int def_index{}, paint_kit{}, music_kit{}, team = 3;
        std::string model_path;
        int width = 512, height = 512;
        float yaw{}, pitch{}; // degrees; catalogue finish uses engine defaults
        bool same_asset(const request& other) const;
    };
    using texture_ref = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;

    bool initialize();
    void set_capture_available(bool available);
    void submit(request value);
    void hide();
    void update();
    void reset();
    void shutdown();
    [[nodiscard]] texture_ref acquire(ID3D11Device* device);
    [[nodiscard]] std::string status() const;
    // Called only with the real result of the render-system SRV function.
    void capture_resource(std::uintptr_t handle, char alternate_view,
                          ID3D11ShaderResourceView* srv, const char* name = nullptr);
    [[nodiscard]] static std::uintptr_t resource_view_address();

private:
    using clock = std::chrono::steady_clock;
    mutable std::mutex mutex_;
    bool initialized_{}, capture_available_{}, visible_{};
    std::optional<request> wanted_;
    std::uint64_t generation_{}, issued_generation_{};
    std::string session_, expected_name_;
    texture_ref texture_;
    clock::time_point last_submit_{}, changed_at_{}, last_capture_{};
    std::string status_ = "Native preview is starting...";
    // These two members are owned exclusively by the frame-stage thread.
    void* root_panel_{};
    clock::time_point last_script_{}, last_bootstrap_{};
};
}
