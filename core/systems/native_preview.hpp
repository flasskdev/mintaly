#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <d3d11.h>
#include <wrl/client.h>
#include <array>
#include <atomic>
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
        int screen_width = 1920, screen_height = 1080;
        std::uint32_t background_rgb = 0x0c0d10;
        float x{}, y{};
        float yaw{}, pitch{}; // degrees; catalogue finish uses engine defaults
        bool visible{};
        bool same_asset(const request& other) const;
    };

    struct pose_point {
        float x{}, y{};
        bool valid{};
    };
    struct agent_pose_snapshot {
        std::array<pose_point, 27> bones{};
        float left{}, top{}, right{}, bottom{};
        std::chrono::steady_clock::time_point captured_at{};
        bool valid{};
    };
    using texture_ref = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;

    bool initialize();
    void set_capture_available(bool available);
    void submit(request value);
    void hide();
    [[nodiscard]] bool item_preview_active() const;
    void update();
    [[nodiscard]] bool wants_agent_pose() const noexcept;
    void capture_agent_pose(std::uintptr_t entity);
    [[nodiscard]] agent_pose_snapshot get_agent_pose() const;
    [[nodiscard]] bool is_agent_preview_entity(std::uintptr_t entity) const;
    void reset();
    void shutdown();
    [[nodiscard]] bool is_enemy_preview(int local_team) const;
    [[nodiscard]] bool connected() const;
    [[nodiscard]] texture_ref acquire(ID3D11Device* device);
    [[nodiscard]] std::string status() const;
    // Called only with the real result of the render-system SRV function.
    void capture_resource(std::uintptr_t handle, char alternate_view,
                          ID3D11ShaderResourceView* srv, const char* name = nullptr);
    [[nodiscard]] static std::uintptr_t resource_view_address();

private:
    using clock = std::chrono::steady_clock;
    mutable std::mutex mutex_;
    bool initialized_{}, capture_available_{}, bootstrap_sent_{}, connected_{};
    std::atomic_bool pose_capture_requested_{};
    agent_pose_snapshot agent_pose_{};
    std::uintptr_t pose_entity_{};
    float pose_basis_x_{1.0f}, pose_basis_y_{};
    bool pose_basis_valid_{};
    std::optional<request> wanted_, last_sent_;
    std::string status_ = "Native CS2 agent preview is starting...";
    clock::time_point last_script_{};
};
}
