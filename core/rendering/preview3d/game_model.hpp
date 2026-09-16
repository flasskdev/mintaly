#pragma once

#include "geometry.hpp"
#include <core/features/features.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <string_view>
#include <algorithm>

namespace nemesis::preview3d {

    // Helper for procedural weapon geometry synthesis from CS2 item specs
    class mesh_builder {
    public:
        std::vector<vertex> vertices;

        void add_triangle(vertex a, vertex b, vertex c) {
            vec3 ab = b.position - a.position;
            vec3 ac = c.position - a.position;
            vec3 normal = unit(cross(ab, ac));
            if (length(a.normal) < 1e-6f) a.normal = normal;
            if (length(b.normal) < 1e-6f) b.normal = normal;
            if (length(c.normal) < 1e-6f) c.normal = normal;
            vertices.push_back(a);
            vertices.push_back(b);
            vertices.push_back(c);
        }

        void add_quad(vertex a, vertex b, vertex c, vertex d) {
            add_triangle(a, b, c);
            add_triangle(a, c, d);
        }

        void add_box(vec3 center, vec3 half_size, vec4 col = {1,1,1,1}, vec3 normal_hint = {0,0,0}) {
            const float x0 = center.x - half_size.x, x1 = center.x + half_size.x;
            const float y0 = center.y - half_size.y, y1 = center.y + half_size.y;
            const float z0 = center.z - half_size.z, z1 = center.z + half_size.z;

            // +Z front
            vec3 nz{0, 0, 1};
            add_quad(
                vertex{{x0, y0, z1}, nz, {0, 0}, col},
                vertex{{x1, y0, z1}, nz, {1, 0}, col},
                vertex{{x1, y1, z1}, nz, {1, 1}, col},
                vertex{{x0, y1, z1}, nz, {0, 1}, col}
            );
            // -Z back
            vec3 n_z{0, 0, -1};
            add_quad(
                vertex{{x1, y0, z0}, n_z, {0, 0}, col},
                vertex{{x0, y0, z0}, n_z, {1, 0}, col},
                vertex{{x0, y1, z0}, n_z, {1, 1}, col},
                vertex{{x1, y1, z0}, n_z, {0, 1}, col}
            );
            // +X right
            vec3 nx{1, 0, 0};
            add_quad(
                vertex{{x1, y0, z1}, nx, {0, 0}, col},
                vertex{{x1, y0, z0}, nx, {1, 0}, col},
                vertex{{x1, y1, z0}, nx, {1, 1}, col},
                vertex{{x1, y1, z1}, nx, {0, 1}, col}
            );
            // -X left
            vec3 n_x{-1, 0, 0};
            add_quad(
                vertex{{x0, y0, z0}, n_x, {0, 0}, col},
                vertex{{x0, y0, z1}, n_x, {1, 0}, col},
                vertex{{x0, y1, z1}, n_x, {1, 1}, col},
                vertex{{x0, y1, z0}, n_x, {0, 1}, col}
            );
            // +Y top
            vec3 ny{0, 1, 0};
            add_quad(
                vertex{{x0, y1, z1}, ny, {0, 0}, col},
                vertex{{x1, y1, z1}, ny, {1, 0}, col},
                vertex{{x1, y1, z0}, ny, {1, 1}, col},
                vertex{{x0, y1, z0}, ny, {0, 1}, col}
            );
            // -Y bottom
            vec3 n_y{0, -1, 0};
            add_quad(
                vertex{{x0, y0, z0}, n_y, {0, 0}, col},
                vertex{{x1, y0, z0}, n_y, {1, 0}, col},
                vertex{{x1, y0, z1}, n_y, {1, 1}, col},
                vertex{{x0, y0, z1}, n_y, {0, 1}, col}
            );
        }

        void add_tapered_box(vec3 center, vec3 half_bottom, vec3 half_top, float height, vec4 col = {1,1,1,1}) {
            const float y0 = center.y - height * 0.5f;
            const float y1 = center.y + height * 0.5f;

            // 8 corners
            vec3 c[8] = {
                {center.x - half_bottom.x, y0, center.z - half_bottom.z}, // 0: -X -Z
                {center.x + half_bottom.x, y0, center.z - half_bottom.z}, // 1: +X -Z
                {center.x + half_bottom.x, y0, center.z + half_bottom.z}, // 2: +X +Z
                {center.x - half_bottom.x, y0, center.z + half_bottom.z}, // 3: -X +Z
                {center.x - half_top.x, y1, center.z - half_top.z},       // 4: -X -Z
                {center.x + half_top.x, y1, center.z - half_top.z},       // 5: +X -Z
                {center.x + half_top.x, y1, center.z + half_top.z},       // 6: +X +Z
                {center.x - half_top.x, y1, center.z + half_top.z}        // 7: -X +Z
            };

            // Bottom
            add_quad(vertex{c[0], {0,-1,0}, {0,0}, col}, vertex{c[1], {0,-1,0}, {1,0}, col}, vertex{c[2], {0,-1,0}, {1,1}, col}, vertex{c[3], {0,-1,0}, {0,1}, col});
            // Top
            add_quad(vertex{c[7], {0,1,0}, {0,0}, col}, vertex{c[6], {0,1,0}, {1,0}, col}, vertex{c[5], {0,1,0}, {1,1}, col}, vertex{c[4], {0,1,0}, {0,1}, col});
            // Sides
            add_quad(vertex{c[3], {0,0,1}, {0,0}, col}, vertex{c[2], {0,0,1}, {1,0}, col}, vertex{c[6], {0,0,1}, {1,1}, col}, vertex{c[7], {0,0,1}, {0,1}, col});
            add_quad(vertex{c[1], {0,0,-1}, {0,0}, col}, vertex{c[0], {0,0,-1}, {1,0}, col}, vertex{c[4], {0,0,-1}, {1,1}, col}, vertex{c[5], {0,0,-1}, {0,1}, col});
            add_quad(vertex{c[2], {1,0,0}, {0,0}, col}, vertex{c[1], {1,0,0}, {1,0}, col}, vertex{c[5], {1,0,0}, {1,1}, col}, vertex{c[6], {1,0,0}, {0,1}, col});
            add_quad(vertex{c[0], {-1,0,0}, {0,0}, col}, vertex{c[3], {-1,0,0}, {1,0}, col}, vertex{c[7], {-1,0,0}, {1,1}, col}, vertex{c[4], {-1,0,0}, {0,1}, col});
        }

        void add_cylinder(vec3 start, vec3 end, float radius, int segments = 12, vec4 col = {1,1,1,1}) {
            vec3 dir = end - start;
            float h = length(dir);
            if (h < 1e-6f) return;
            vec3 axis = unit(dir);

            vec3 u = std::abs(axis.y) < 0.99f ? unit(cross(axis, {0, 1, 0})) : unit(cross(axis, {1, 0, 0}));
            vec3 v = cross(axis, u);

            for (int i = 0; i < segments; ++i) {
                float a0 = (i * 6.2831853f) / segments;
                float a1 = ((i + 1) * 6.2831853f) / segments;

                vec3 radial0 = u * std::cos(a0) + v * std::sin(a0);
                vec3 radial1 = u * std::cos(a1) + v * std::sin(a1);

                vec3 p0 = start + radial0 * radius;
                vec3 p1 = start + radial1 * radius;
                vec3 p2 = end + radial1 * radius;
                vec3 p3 = end + radial0 * radius;

                // Side
                add_quad(
                    vertex{p0, radial0, {static_cast<float>(i)/segments, 0}, col},
                    vertex{p1, radial1, {static_cast<float>(i+1)/segments, 0}, col},
                    vertex{p2, radial1, {static_cast<float>(i+1)/segments, 1}, col},
                    vertex{p3, radial0, {static_cast<float>(i)/segments, 1}, col}
                );

                // Caps
                add_triangle(vertex{start, axis * -1.0f, {0.5f, 0.5f}, col}, vertex{p1, axis * -1.0f, {0, 0}, col}, vertex{p0, axis * -1.0f, {1, 0}, col});
                add_triangle(vertex{end, axis, {0.5f, 0.5f}, col}, vertex{p3, axis, {0, 0}, col}, vertex{p2, axis, {1, 0}, col});
            }
        }

        void add_sphere(vec3 center, float radius, int rings = 8, int sectors = 12, vec4 col = {1,1,1,1}) {
            for (int r = 0; r < rings; ++r) {
                float phi0 = 3.14159265f * static_cast<float>(r) / rings;
                float phi1 = 3.14159265f * static_cast<float>(r + 1) / rings;
                float y0 = center.y + radius * std::cos(phi0);
                float y1 = center.y + radius * std::cos(phi1);
                float r0 = radius * std::sin(phi0);
                float r1 = radius * std::sin(phi1);

                for (int s = 0; s < sectors; ++s) {
                    float theta0 = 2.0f * 3.14159265f * static_cast<float>(s) / sectors;
                    float theta1 = 2.0f * 3.14159265f * static_cast<float>(s + 1) / sectors;

                    vec3 p00{center.x + r0 * std::sin(theta0), y0, center.z + r0 * std::cos(theta0)};
                    vec3 p01{center.x + r0 * std::sin(theta1), y0, center.z + r0 * std::cos(theta1)};
                    vec3 p10{center.x + r1 * std::sin(theta0), y1, center.z + r1 * std::cos(theta0)};
                    vec3 p11{center.x + r1 * std::sin(theta1), y1, center.z + r1 * std::cos(theta1)};

                    vec3 n00 = unit(p00 - center);
                    vec3 n01 = unit(p01 - center);
                    vec3 n10 = unit(p10 - center);
                    vec3 n11 = unit(p11 - center);

                    if (r == 0) {
                        add_triangle(vertex{p00, n00, {0,0}, col}, vertex{p10, n10, {0,1}, col}, vertex{p11, n11, {1,1}, col});
                    } else if (r == rings - 1) {
                        add_triangle(vertex{p00, n00, {0,0}, col}, vertex{p10, n10, {0,1}, col}, vertex{p01, n01, {1,0}, col});
                    } else {
                        add_quad(vertex{p00, n00, {0,0}, col}, vertex{p10, n10, {0,1}, col},
                                 vertex{p11, n11, {1,1}, col}, vertex{p01, n01, {1,0}, col});
                    }
                }
            }
        }

        void add_curved_blade(vec3 start, vec3 end, float width, float thickness, float curvature, int segments = 8, vec4 col = {1,1,1,1}) {
            vec3 dir = end - start;
            float len = length(dir);
            if (len < 1e-6f) return;
            vec3 fwd = unit(dir);
            vec3 side{0, 0, 1};
            vec3 edge_dir = unit(cross(fwd, side));

            std::vector<vec3> spine(segments + 1), edge(segments + 1);
            for (int i = 0; i <= segments; ++i) {
                float t = static_cast<float>(i) / segments;
                float curve = std::sin(t * 3.14159f) * curvature;
                vec3 center = start + fwd * (t * len) + edge_dir * curve;
                float cur_width = width * (1.0f - t * 0.85f);
                spine[i] = center - edge_dir * (cur_width * 0.3f);
                edge[i] = center + edge_dir * (cur_width * 0.7f);
            }

            for (int i = 0; i < segments; ++i) {
                vec3 p_s0 = spine[i] + side * (thickness * 0.5f);
                vec3 p_s1 = spine[i+1] + side * (thickness * 0.5f);
                vec3 p_e0 = edge[i];
                vec3 p_e1 = edge[i+1];

                vec3 m_s0 = spine[i] - side * (thickness * 0.5f);
                vec3 m_s1 = spine[i+1] - side * (thickness * 0.5f);

                // +Side
                add_triangle(vertex{p_s0, side, {0,0}, col}, vertex{p_e0, side, {1,0}, col}, vertex{p_e1, side, {1,1}, col});
                add_triangle(vertex{p_s0, side, {0,0}, col}, vertex{p_e1, side, {1,1}, col}, vertex{p_s1, side, {0,1}, col});

                // -Side
                add_triangle(vertex{m_s0, side * -1.0f, {0,0}, col}, vertex{m_s1, side * -1.0f, {0,1}, col}, vertex{p_e1, side * -1.0f, {1,1}, col});
                add_triangle(vertex{m_s0, side * -1.0f, {0,0}, col}, vertex{p_e1, side * -1.0f, {1,1}, col}, vertex{p_e0, side * -1.0f, {1,0}, col});

                // Spine back
                add_quad(vertex{p_s0, edge_dir * -1.0f, {0,0}, col}, vertex{p_s1, edge_dir * -1.0f, {1,0}, col},
                         vertex{m_s1, edge_dir * -1.0f, {1,1}, col}, vertex{m_s0, edge_dir * -1.0f, {0,1}, col});
            }
        }

        void append_transformed(const mesh& m, vec3 translation, vec3 rotation_euler, float scale, vec4 col_override = {0,0,0,0}) {
            for (std::size_t i = 0; i + 2 < m.vertices.size(); i += 3) {
                vertex v[3] = { m.vertices[i], m.vertices[i+1], m.vertices[i+2] };
                for (int k = 0; k < 3; ++k) {
                    vec3 p = v[k].position * scale;
                    vec3 n = v[k].normal;
                    if (rotation_euler.x != 0.0f) { p = rotate_x(p, rotation_euler.x); n = rotate_x(n, rotation_euler.x); }
                    if (rotation_euler.y != 0.0f) { p = rotate_y(p, rotation_euler.y); n = rotate_y(n, rotation_euler.y); }
                    if (rotation_euler.z != 0.0f) { p = rotate_z(p, rotation_euler.z); n = rotate_z(n, rotation_euler.z); }
                    v[k].position = p + translation;
                    v[k].normal = unit(n);
                    if (col_override.w > 0.0f) {
                        v[k].color = col_override;
                    }
                }
                add_triangle(v[0], v[1], v[2]);
            }
        }
    };

    // Calculate bounding box and normalize mesh to unit sphere with deterministic fingerprint
    inline void finalize_mesh(mesh& result, int def_index) {
        if (result.vertices.empty()) return;
        vec3 lo = result.vertices.front().position, hi = lo;
        for (const auto& v : result.vertices) {
            lo = {std::min(lo.x, v.position.x), std::min(lo.y, v.position.y), std::min(lo.z, v.position.z)};
            hi = {std::max(hi.x, v.position.x), std::max(hi.y, v.position.y), std::max(hi.z, v.position.z)};
        }
        result.center = (lo + hi) * 0.5f;
        result.radius = 0.0f;
        for (const auto& v : result.vertices)
            result.radius = std::max(result.radius, length(v.position - result.center));

        if (!std::isfinite(result.radius) || result.radius < 1e-9f)
            result.radius = 1.0f;

        for (auto& v : result.vertices)
            v.position = (v.position - result.center) / result.radius;

        // Deterministic fingerprint based on def_index and vertex count
        std::uint64_t fp = 14695981039346656037ull ^ static_cast<std::uint64_t>(def_index * 1000003);
        fp ^= static_cast<std::uint64_t>(result.vertices.size());
        fp *= 1099511628211ull;
        result.fingerprint = fp;
    }

    // Try capturing live weapon mesh directly from in-hand viewmodel CModel
    inline bool try_capture_from_game(int def_index, mesh& out_mesh) {
        if (!systems::g_local.get().is_valid()) return false;
        const auto pawn = systems::g_local.get().pawn;
        if (!pawn) return false;

        // Try HUD model weapon
        const auto arms_handle = memory::safe_read<std::uint32_t>(pawn + SCHEMA("C_CSPlayerPawn", "m_hHudModelArms"_hash)).value_or(0);
        if (!arms_handle) return false;
        const auto arms = systems::g_entities.lookup(arms_handle);
        if (!arms) return false;
        const auto arms_scene_node = memory::safe_read<std::uintptr_t>(arms + SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
        if (!arms_scene_node) return false;

        auto child = memory::safe_read<std::uintptr_t>(arms_scene_node + SCHEMA("CGameSceneNode", "m_pChild"_hash)).value_or(0);
        std::uintptr_t weapon_scene_node = 0;
        while (child && child > 0x10000) {
            const auto owner = memory::safe_read<std::uintptr_t>(child + SCHEMA("CGameSceneNode", "m_pOwner"_hash)).value_or(0);
            if (owner && owner > 0x10000) {
                const auto name = systems::g_entities.get_schema_name(owner);
                if (name && fnv1a::runtime_hash(name) == "C_CS2HudModelWeapon"_hash) {
                    weapon_scene_node = memory::safe_read<std::uintptr_t>(owner + SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
                    break;
                }
            }
            child = memory::safe_read<std::uintptr_t>(child + SCHEMA("CGameSceneNode", "m_pNextSibling"_hash)).value_or(0);
        }

        if (!weapon_scene_node) return false;

        // Query hitboxes / bounds from CModel
        const auto hb_set = systems::g_hitboxes.query(weapon_scene_node, true);
        if (hb_set.count <= 0) return false;

        mesh_builder builder;
        for (int i = 0; i < hb_set.count; ++i) {
            const auto& hb = hb_set.entries[i];
            vec3 min_pt{hb.mins.x, hb.mins.y, hb.mins.z};
            vec3 max_pt{hb.maxs.x, hb.maxs.y, hb.maxs.z};
            vec3 center = (min_pt + max_pt) * 0.5f;
            vec3 size = (max_pt - min_pt) * 0.5f;
            if (hb.radius > 0.0f) {
                size.x += hb.radius; size.y += hb.radius; size.z += hb.radius;
            }
            builder.add_box(center, size);
        }

        if (builder.vertices.empty()) return false;
        out_mesh.vertices = std::move(builder.vertices);
        finalize_mesh(out_mesh, def_index);
        return true;
    }

    // High-fidelity procedural weapon mesh synthesizer covering all CS2 weapon categories
    inline mesh generate_weapon_mesh(int def_index, std::string_view name) {
        mesh_builder mb;

        // Categorize by def_index
        bool is_knife = (def_index >= 500 && def_index <= 526) || def_index == 41 || def_index == 42 || def_index == 59;
        bool is_glove = (def_index >= 5027 && def_index <= 5035);
        bool is_sniper = (def_index == 9 || def_index == 40 || def_index == 38 || def_index == 11);
        bool is_pistol = (def_index == 1 || def_index == 2 || def_index == 3 || def_index == 4 ||
                          def_index == 30 || def_index == 32 || def_index == 36 || def_index == 61 || def_index == 63 || def_index == 64);
        bool is_smg = (def_index == 17 || def_index == 19 || def_index == 23 || def_index == 24 || def_index == 26 || def_index == 33 || def_index == 34);
        bool is_shotgun = (def_index == 25 || def_index == 27 || def_index == 29 || def_index == 35);
        bool is_heavy = (def_index == 14 || def_index == 28);
        bool is_zeus = (def_index == 31);

        if (is_knife) {
            // Detailed knife geometry
            if (def_index == 507) {
                // Karambit: Curved talon blade + pommel ring + ergonomic handle
                // Blade
                mb.add_curved_blade({0.0f, 0.4f, 0.0f}, {0.85f, -0.2f, 0.0f}, 0.22f, 0.035f, 0.35f, 10);
                // Handle
                mb.add_box({-0.45f, 0.25f, 0.0f}, {0.35f, 0.12f, 0.05f});
                // Finger ring pommel at end
                mb.add_cylinder({-0.85f, 0.2f, -0.04f}, {-0.85f, 0.2f, 0.04f}, 0.16f, 16);
            } else if (def_index == 515) {
                // Butterfly: Spear blade + twin handles
                // Blade
                mb.add_box({0.45f, 0.0f, 0.0f}, {0.45f, 0.08f, 0.025f});
                // Pivot swivels
                mb.add_cylinder({0.0f, 0.08f, -0.06f}, {0.0f, 0.08f, 0.06f}, 0.04f, 8);
                mb.add_cylinder({0.0f, -0.08f, -0.06f}, {0.0f, -0.08f, 0.06f}, 0.04f, 8);
                // Twin handles
                mb.add_box({-0.5f, 0.12f, 0.0f}, {0.45f, 0.06f, 0.04f});
                mb.add_box({-0.5f, -0.12f, 0.0f}, {0.45f, 0.06f, 0.04f});
            } else if (def_index == 508) {
                // M9 Bayonet: Broad clip-point blade with saw-teeth + guard + cylindrical grip
                mb.add_box({0.55f, 0.0f, 0.0f}, {0.55f, 0.12f, 0.035f});
                // Guard with barrel ring
                mb.add_box({0.0f, 0.0f, 0.0f}, {0.04f, 0.28f, 0.08f});
                // Handle
                mb.add_cylinder({-0.02f, 0.0f, 0.0f}, {-0.75f, 0.0f, 0.0f}, 0.09f, 12);
                // Pommel
                mb.add_cylinder({-0.75f, 0.0f, 0.0f}, {-0.85f, 0.0f, 0.0f}, 0.11f, 10);
            } else {
                // General Knives (Bayonet, Flip, Gut, Huntsman, Bowie, Daggers, etc.)
                mb.add_box({0.5f, 0.0f, 0.0f}, {0.5f, 0.11f, 0.03f});
                mb.add_box({0.0f, 0.0f, 0.0f}, {0.03f, 0.22f, 0.06f}); // Crossguard
                mb.add_box({-0.45f, 0.0f, 0.0f}, {0.4f, 0.09f, 0.05f}); // Handle
            }
        } else if (is_glove) {
            // Gloves: Palm + wrist + 5 fingers
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.35f, 0.35f, 0.15f}); // Palm
            mb.add_box({0.0f, -0.45f, 0.0f}, {0.32f, 0.15f, 0.16f}); // Cuff
            // 4 fingers
            for (int f = 0; f < 4; ++f) {
                float fx = -0.24f + f * 0.16f;
                mb.add_cylinder({fx, 0.35f, 0.0f}, {fx, 0.85f, 0.0f}, 0.055f, 8);
            }
            // Thumb
            mb.add_cylinder({-0.35f, 0.05f, 0.08f}, {-0.65f, 0.35f, 0.12f}, 0.065f, 8);
        } else if (def_index == 7) {
            // AK-47: Classic receiver, curved magazine, wooden handguard, stock, sights
            // Main receiver
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.65f, 0.18f, 0.10f});
            // Curved top dust cover
            mb.add_cylinder({-0.65f, 0.15f, 0.0f}, {0.45f, 0.15f, 0.0f}, 0.10f, 10);
            // Handguard (wood lower and gas tube upper)
            mb.add_box({0.95f, -0.02f, 0.0f}, {0.32f, 0.14f, 0.09f});
            mb.add_cylinder({0.65f, 0.13f, 0.0f}, {1.35f, 0.13f, 0.0f}, 0.065f, 8);
            // Barrel and slant compensator
            mb.add_cylinder({1.25f, 0.0f, 0.0f}, {2.35f, 0.0f, 0.0f}, 0.045f, 10);
            mb.add_cylinder({2.35f, 0.0f, 0.0f}, {2.50f, 0.0f, 0.0f}, 0.060f, 8); // Muzzle brake
            // Front sight post
            mb.add_box({2.15f, 0.22f, 0.0f}, {0.05f, 0.15f, 0.03f});
            // Rear tangent sight
            mb.add_box({0.60f, 0.22f, 0.0f}, {0.10f, 0.06f, 0.04f});
            // Curved 30-round banana magazine
            mb.add_tapered_box({0.20f, -0.55f, 0.0f}, {0.18f, 0.08f, 0.08f}, {0.16f, 0.08f, 0.07f}, 0.65f);
            // Pistol grip
            mb.add_tapered_box({-0.45f, -0.45f, 0.0f}, {0.12f, 0.06f, 0.06f}, {0.14f, 0.06f, 0.07f}, 0.55f);
            // Wooden buttstock
            mb.add_tapered_box({-1.35f, -0.08f, 0.0f}, {0.70f, 0.15f, 0.07f}, {0.15f, 0.15f, 0.08f}, 0.40f);
        } else if (def_index == 16 || def_index == 60) {
            // M4A4 / M4A1-S: Flat top receiver, buffer tube + crane stock, handguard, barrel/silencer
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.55f, 0.19f, 0.09f}); // Lower/upper receiver
            mb.add_box({0.05f, 0.23f, 0.0f}, {0.45f, 0.04f, 0.05f}); // Picatinny top rail
            // Handguard
            mb.add_cylinder({0.55f, 0.0f, 0.0f}, {1.35f, 0.0f, 0.0f}, 0.11f, 12);
            if (def_index == 60) {
                // M4A1-S: Detachable suppressor
                mb.add_cylinder({1.35f, 0.0f, 0.0f}, {1.65f, 0.0f, 0.0f}, 0.045f, 8); // Barrel
                mb.add_cylinder({1.65f, 0.0f, 0.0f}, {2.75f, 0.0f, 0.0f}, 0.105f, 14); // Suppressor
            } else {
                // M4A4: Barrel + birdcage flash hider + front sight triangle
                mb.add_cylinder({1.35f, 0.0f, 0.0f}, {2.15f, 0.0f, 0.0f}, 0.050f, 8);
                mb.add_box({1.45f, 0.22f, 0.0f}, {0.08f, 0.16f, 0.03f}); // A2 Front sight tower
                mb.add_cylinder({2.15f, 0.0f, 0.0f}, {2.30f, 0.0f, 0.0f}, 0.065f, 8); // Flash hider
            }
            // Magazine (STANAG)
            mb.add_box({0.20f, -0.45f, 0.0f}, {0.14f, 0.35f, 0.065f});
            // A2 Pistol grip
            mb.add_tapered_box({-0.38f, -0.42f, 0.0f}, {0.11f, 0.055f, 0.055f}, {0.13f, 0.055f, 0.065f}, 0.50f);
            // Buffer tube & collapsible crane stock
            mb.add_cylinder({-0.55f, 0.04f, 0.0f}, {-1.35f, 0.04f, 0.0f}, 0.055f, 8);
            mb.add_box({-1.15f, -0.05f, 0.0f}, {0.30f, 0.20f, 0.085f});
        } else if (is_sniper) {
            // AWP / SSG 08 / Auto-snipers: Long chassis, heavy barrel, massive optical scope
            // Chassis / receiver
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.85f, 0.16f, 0.11f});
            // Long bull barrel
            mb.add_cylinder({0.85f, 0.03f, 0.0f}, {2.85f, 0.03f, 0.0f}, 0.065f, 10);
            // Massive muzzle brake
            mb.add_box({2.95f, 0.03f, 0.0f}, {0.15f, 0.09f, 0.09f});
            // High-power sniper scope
            mb.add_cylinder({-0.20f, 0.38f, 0.0f}, {0.60f, 0.38f, 0.0f}, 0.12f, 12);
            // Scope mount rings
            mb.add_box({-0.05f, 0.25f, 0.0f}, {0.06f, 0.10f, 0.07f});
            mb.add_box({0.45f, 0.25f, 0.0f}, {0.06f, 0.10f, 0.07f});
            // Stock with thumbhole and cheek riser
            mb.add_box({-1.15f, -0.08f, 0.0f}, {0.65f, 0.22f, 0.09f});
            mb.add_box({-1.05f, 0.20f, 0.0f}, {0.25f, 0.06f, 0.07f}); // Cheek rest
            // Box mag
            mb.add_box({0.15f, -0.35f, 0.0f}, {0.15f, 0.22f, 0.08f});
            // Pistol grip
            mb.add_box({-0.45f, -0.40f, 0.0f}, {0.12f, 0.22f, 0.07f});
        } else if (is_pistol) {
            // Pistols: Deagle, Glock, USP-S, P250, etc.
            float slide_len = (def_index == 1) ? 0.95f : 0.65f; // Deagle is huge
            float slide_h = (def_index == 1) ? 0.22f : 0.16f;
            float slide_w = (def_index == 1) ? 0.13f : 0.095f;

            // Slide
            mb.add_box({0.20f, 0.10f, 0.0f}, {slide_len * 0.5f, slide_h * 0.5f, slide_w * 0.5f});
            // Frame & trigger guard
            mb.add_box({0.0f, -0.05f, 0.0f}, {0.35f, 0.08f, slide_w * 0.45f});
            // Grip
            mb.add_tapered_box({-0.18f, -0.45f, 0.0f}, {0.14f, 0.08f, slide_w * 0.42f}, {0.16f, 0.08f, slide_w * 0.48f}, 0.48f);
            // Sights
            mb.add_box({0.20f + slide_len * 0.45f, 0.10f + slide_h * 0.55f, 0.0f}, {0.04f, 0.04f, 0.02f});
            mb.add_box({0.20f - slide_len * 0.45f, 0.10f + slide_h * 0.55f, 0.0f}, {0.04f, 0.04f, 0.035f});

            if (def_index == 61) {
                // USP-S detachable suppressor
                mb.add_cylinder({0.20f + slide_len * 0.5f, 0.08f, 0.0f}, {0.20f + slide_len * 0.5f + 1.15f, 0.08f, 0.0f}, 0.085f, 12);
            }
        } else if (is_smg) {
            // SMGs: Compact receiver, short barrel, forward mag / top mag
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.45f, 0.18f, 0.09f});
            mb.add_cylinder({0.45f, 0.0f, 0.0f}, {0.95f, 0.0f, 0.0f}, 0.055f, 8);
            if (def_index == 23) {
                // MP5-SD: Ribbed integral suppressor
                mb.add_cylinder({0.35f, 0.0f, 0.0f}, {1.35f, 0.0f, 0.0f}, 0.095f, 12);
            }
            // Grip & magazine
            mb.add_box({-0.20f, -0.40f, 0.0f}, {0.10f, 0.25f, 0.06f});
            mb.add_box({0.15f, -0.45f, 0.0f}, {0.08f, 0.35f, 0.05f});
        } else if (is_shotgun || is_heavy) {
            // Shotguns / Heavy LMG
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.75f, 0.20f, 0.12f});
            mb.add_cylinder({0.75f, 0.06f, 0.0f}, {2.25f, 0.06f, 0.0f}, 0.075f, 10);
            mb.add_cylinder({0.75f, -0.06f, 0.0f}, {1.85f, -0.06f, 0.0f}, 0.065f, 10); // Magazine tube
            mb.add_box({1.15f, -0.06f, 0.0f}, {0.25f, 0.12f, 0.11f}); // Pump / forend
            mb.add_box({-0.95f, -0.10f, 0.0f}, {0.55f, 0.18f, 0.08f}); // Stock
        } else if (is_zeus) {
            // Zeus x27 taser
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.35f, 0.16f, 0.09f});
            mb.add_box({0.38f, 0.0f, 0.0f}, {0.08f, 0.14f, 0.08f}); // Cartridge
            mb.add_box({-0.12f, -0.32f, 0.0f}, {0.11f, 0.18f, 0.065f}); // Grip
        } else {
            // Fallback general rifle / gun model
            mb.add_box({0.0f, 0.0f, 0.0f}, {0.60f, 0.18f, 0.09f});
            mb.add_cylinder({0.60f, 0.0f, 0.0f}, {1.95f, 0.0f, 0.0f}, 0.05f, 8);
            mb.add_box({0.15f, -0.45f, 0.0f}, {0.12f, 0.30f, 0.06f});
            mb.add_box({-0.35f, -0.40f, 0.0f}, {0.11f, 0.22f, 0.06f});
            mb.add_box({-1.05f, -0.08f, 0.0f}, {0.55f, 0.16f, 0.08f});
        }

        mesh result;
        result.vertices = std::move(mb.vertices);
        finalize_mesh(result, def_index);
        return result;
    }

    inline vec4 get_skin_theme_color(int paint_kit_id, std::string_view name) {
        std::string lower(name);
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return static_cast<char>(::tolower(c)); });

        if (lower.find("asiimov") != std::string::npos) return {0.96f, 0.46f, 0.10f, 1.0f}; // Orange
        if (lower.find("redline") != std::string::npos || lower.find("cyrex") != std::string::npos || lower.find("howl") != std::string::npos) return {0.92f, 0.12f, 0.15f, 1.0f}; // Crimson
        if (lower.find("fade") != std::string::npos || lower.find("case hardened") != std::string::npos) return {0.72f, 0.28f, 0.88f, 1.0f}; // Purple fade
        if (lower.find("dragon lore") != std::string::npos || lower.find("gold") != std::string::npos || lower.find("lore") != std::string::npos) return {0.94f, 0.78f, 0.24f, 1.0f}; // Gold
        if (lower.find("hyper beast") != std::string::npos || lower.find("neon") != std::string::npos || lower.find("fever dream") != std::string::npos) return {0.88f, 0.18f, 0.78f, 1.0f}; // Neon
        if (lower.find("printstream") != std::string::npos || lower.find("whiteout") != std::string::npos || lower.find("mecha") != std::string::npos) return {0.92f, 0.94f, 0.96f, 1.0f}; // Pearlescent White
        if (lower.find("vulcan") != std::string::npos || lower.find("blue phosphor") != std::string::npos || lower.find("frontside misty") != std::string::npos) return {0.18f, 0.65f, 0.98f, 1.0f}; // Electric Blue
        if (lower.find("emerald") != std::string::npos || lower.find("green") != std::string::npos) return {0.12f, 0.85f, 0.38f, 1.0f}; // Emerald
        if (lower.find("doppler") != std::string::npos || lower.find("sapphire") != std::string::npos) return {0.20f, 0.35f, 0.95f, 1.0f}; // Sapphire

        if (paint_kit_id > 0) {
            float hue = std::fmod(static_cast<float>(paint_kit_id * 137.5f), 360.0f);
            float h = hue / 60.0f;
            int i = static_cast<int>(h) % 6;
            float f = h - std::floor(h);
            float q = 1.0f - f;
            float r = 0.5f, g = 0.5f, b = 0.5f;
            switch (i) {
                case 0: r = 1.0f; g = f; b = 0.2f; break;
                case 1: r = q; g = 1.0f; b = 0.2f; break;
                case 2: r = 0.2f; g = 1.0f; b = f; break;
                case 3: r = 0.2f; g = q; b = 1.0f; break;
                case 4: r = f; g = 0.2f; b = 1.0f; break;
                case 5: r = 1.0f; g = 0.2f; b = q; break;
            }
            return {r, g, b, 1.0f};
        }
        return {0.35f, 0.38f, 0.42f, 1.0f}; // Gunmetal steel
    }

    struct agent_palette {
        vec4 uniform_base;
        vec4 uniform_accent;
        vec4 vest;
        vec4 vest_webbing;
        vec4 helmet;
        vec4 visor_goggles;
        vec4 skin_tone;
        vec4 balaclava;
        vec4 boots;
        vec4 knee_pads;
        vec4 belt_gear;
        vec4 gloves_primary;
        vec4 gloves_secondary;
    };

    inline agent_palette get_agent_palette(int team, int agent_def_index, int glove_def_index) {
        agent_palette p;
        if (team == 3) {
            // CT Counter-Terrorist (Tactical SWAT / Navy / SAS)
            p.uniform_base       = {0.14f, 0.17f, 0.24f, 1.0f};
            p.uniform_accent     = {0.11f, 0.13f, 0.18f, 1.0f};
            p.vest               = {0.10f, 0.11f, 0.15f, 1.0f};
            p.vest_webbing       = {0.18f, 0.21f, 0.25f, 1.0f};
            p.helmet             = {0.17f, 0.20f, 0.19f, 1.0f};
            p.visor_goggles      = {0.18f, 0.45f, 0.72f, 0.95f};
            p.skin_tone          = {0.84f, 0.67f, 0.55f, 1.0f};
            p.balaclava          = {0.08f, 0.09f, 0.11f, 1.0f};
            p.boots              = {0.08f, 0.08f, 0.09f, 1.0f};
            p.knee_pads          = {0.14f, 0.15f, 0.17f, 1.0f};
            p.belt_gear          = {0.09f, 0.10f, 0.12f, 1.0f};
            p.gloves_primary     = {0.14f, 0.15f, 0.17f, 1.0f};
            p.gloves_secondary   = {0.26f, 0.28f, 0.32f, 1.0f};
        } else {
            // T Terrorist (Tactical Khaki / Tan / Guerilla / Professional)
            p.uniform_base       = {0.32f, 0.28f, 0.22f, 1.0f};
            p.uniform_accent     = {0.38f, 0.34f, 0.26f, 1.0f};
            p.vest               = {0.20f, 0.18f, 0.14f, 1.0f};
            p.vest_webbing       = {0.26f, 0.16f, 0.10f, 1.0f};
            p.helmet             = {0.55f, 0.14f, 0.14f, 1.0f}; // Beret red
            p.visor_goggles      = {0.85f, 0.70f, 0.15f, 1.0f}; // Aviator gold
            p.skin_tone          = {0.82f, 0.64f, 0.52f, 1.0f};
            p.balaclava          = {0.14f, 0.14f, 0.15f, 1.0f};
            p.boots              = {0.22f, 0.19f, 0.15f, 1.0f};
            p.knee_pads          = {0.18f, 0.16f, 0.13f, 1.0f};
            p.belt_gear          = {0.14f, 0.12f, 0.10f, 1.0f};
            p.gloves_primary     = {0.22f, 0.14f, 0.09f, 1.0f};
            p.gloves_secondary   = {0.85f, 0.75f, 0.25f, 1.0f}; // Gold watch
        }

        // Custom glove coloring if equipped
        if (glove_def_index >= 5027 && glove_def_index <= 5035) {
            if (glove_def_index == 5030) {
                // Sport Gloves (Vice style neon cyan + pink)
                p.gloves_primary = {0.12f, 0.78f, 0.92f, 1.0f};
                p.gloves_secondary = {0.94f, 0.15f, 0.65f, 1.0f};
            } else if (glove_def_index == 5034) {
                // Specialist Gloves (Crimson Web)
                p.gloves_primary = {0.72f, 0.10f, 0.14f, 1.0f};
                p.gloves_secondary = {0.10f, 0.10f, 0.10f, 1.0f};
            } else if (glove_def_index == 5033) {
                // Moto Gloves (Spearmint)
                p.gloves_primary = {0.85f, 0.92f, 0.88f, 1.0f};
                p.gloves_secondary = {0.15f, 0.65f, 0.50f, 1.0f};
            } else if (glove_def_index == 5031) {
                // Driver Gloves (King Snake / Imperial Plaid)
                p.gloves_primary = {0.88f, 0.88f, 0.86f, 1.0f};
                p.gloves_secondary = {0.18f, 0.18f, 0.18f, 1.0f};
            } else if (glove_def_index == 5032) {
                // Hand Wraps (Cobalt Skulls)
                p.gloves_primary = {0.20f, 0.40f, 0.75f, 1.0f};
                p.gloves_secondary = {0.15f, 0.25f, 0.50f, 1.0f};
            } else {
                // Bloodhound / Hydra
                p.gloves_primary = {0.12f, 0.12f, 0.14f, 1.0f};
                p.gloves_secondary = {0.85f, 0.72f, 0.22f, 1.0f};
            }
        }
        return p;
    }

    inline void build_agent_body(mesh_builder& mb, const agent_palette& pal, int team) {
        // 1. BOOTS & FEET (firm tactical stance on floor Y = -0.92)
        const vec4 dark_sole{0.06f, 0.06f, 0.07f, 1.0f};
        const vec3 l_boot_center{-0.19f, -0.90f, 0.06f};
        const vec3 r_boot_center{+0.19f, -0.90f, -0.04f};

        // Soles
        mb.add_box(l_boot_center, {0.065f, 0.020f, 0.13f}, dark_sole);
        mb.add_box(r_boot_center, {0.065f, 0.020f, 0.13f}, dark_sole);

        // Boots leather uppers
        mb.add_tapered_box({l_boot_center.x, -0.82f, l_boot_center.z}, {0.060f, 0.10f}, {0.052f, 0.07f}, 0.14f, pal.boots);
        mb.add_tapered_box({r_boot_center.x, -0.82f, r_boot_center.z}, {0.060f, 0.10f}, {0.052f, 0.07f}, 0.14f, pal.boots);

        // 2. SHINS / LOWER LEGS
        mb.add_cylinder({l_boot_center.x, -0.75f, l_boot_center.z}, {-0.17f, -0.44f, 0.04f}, 0.068f, 10, pal.uniform_base);
        mb.add_cylinder({r_boot_center.x, -0.75f, r_boot_center.z}, {+0.17f, -0.44f, -0.02f}, 0.068f, 10, pal.uniform_base);

        // Hard-shell Tactical Knee Pads on both knees
        mb.add_box({-0.17f, -0.44f, 0.10f}, {0.055f, 0.060f, 0.025f}, pal.knee_pads);
        mb.add_box({+0.17f, -0.44f, 0.04f}, {0.055f, 0.060f, 0.025f}, pal.knee_pads);
        mb.add_box({-0.17f, -0.44f, 0.02f}, {0.068f, 0.015f, 0.068f}, pal.belt_gear); // strap
        mb.add_box({+0.17f, -0.44f, -0.04f}, {0.068f, 0.015f, 0.068f}, pal.belt_gear); // strap

        // 3. THIGHS & UPPER LEGS
        mb.add_cylinder({-0.17f, -0.44f, 0.04f}, {-0.12f, -0.12f, 0.01f}, 0.082f, 10, pal.uniform_base);
        mb.add_cylinder({+0.17f, -0.44f, -0.02f}, {+0.12f, -0.12f, 0.01f}, 0.082f, 10, pal.uniform_base);

        // Tactical Drop-Leg Holster on right thigh
        mb.add_box({+0.21f, -0.26f, -0.01f}, {0.032f, 0.075f, 0.045f}, pal.belt_gear);
        mb.add_box({+0.21f, -0.18f, 0.01f}, {0.022f, 0.035f, 0.022f}, {0.14f, 0.14f, 0.15f, 1.0f}); // Sidearm grip
        mb.add_box({+0.16f, -0.22f, -0.02f}, {0.082f, 0.014f, 0.082f}, pal.belt_gear); // holster thigh strap

        // Cargo utility pocket on left thigh
        mb.add_box({-0.21f, -0.28f, 0.03f}, {0.028f, 0.060f, 0.050f}, pal.uniform_accent);

        // 4. PELVIS & DUTY BELT
        mb.add_tapered_box({0.0f, -0.10f, 0.0f}, {0.18f, 0.11f}, {0.16f, 0.11f}, 0.10f, pal.uniform_base);
        mb.add_box({0.0f, -0.06f, 0.0f}, {0.185f, 0.030f, 0.125f}, pal.belt_gear); // Duty belt
        mb.add_box({0.0f, -0.06f, 0.128f}, {0.032f, 0.024f, 0.008f}, {0.78f, 0.78f, 0.80f, 1.0f}); // Belt buckle

        // 5. TORSO & HEAVY PLATE CARRIER VEST
        mb.add_tapered_box({0.0f, 0.14f, 0.0f}, {0.16f, 0.11f}, {0.20f, 0.12f}, 0.38f, pal.uniform_base); // Shirt
        mb.add_box({0.0f, 0.14f, 0.045f}, {0.16f, 0.165f, 0.090f}, pal.vest); // Front body armor plate
        mb.add_box({0.0f, 0.06f, 0.0f}, {0.185f, 0.090f, 0.120f}, pal.vest); // Cummerbund

        // Padded shoulder straps
        mb.add_box({-0.12f, 0.31f, 0.01f}, {0.042f, 0.055f, 0.105f}, pal.vest_webbing);
        mb.add_box({+0.12f, 0.31f, 0.01f}, {0.042f, 0.055f, 0.105f}, pal.vest_webbing);

        // Triple rifle mag pouches across stomach
        mb.add_box({-0.09f, 0.04f, 0.145f}, {0.032f, 0.060f, 0.022f}, pal.vest_webbing);
        mb.add_box({ 0.00f, 0.04f, 0.148f}, {0.032f, 0.060f, 0.022f}, pal.vest_webbing);
        mb.add_box({+0.09f, 0.04f, 0.145f}, {0.032f, 0.060f, 0.022f}, pal.vest_webbing);

        // Chest identification patch
        mb.add_box({0.0f, 0.23f, 0.138f}, {0.060f, 0.032f, 0.005f}, team == 3 ? vec4{0.18f, 0.42f, 0.75f, 1.0f} : vec4{0.75f, 0.20f, 0.14f, 1.0f});

        // Tactical radio & whip antenna on left shoulder
        mb.add_box({-0.135f, 0.26f, 0.115f}, {0.022f, 0.050f, 0.028f}, {0.10f, 0.10f, 0.11f, 1.0f});
        mb.add_cylinder({-0.135f, 0.31f, 0.115f}, {-0.135f, 0.50f, 0.095f}, 0.0035f, 6, {0.14f, 0.14f, 0.14f, 1.0f});

        // 6. NECK & HEAD
        mb.add_cylinder({0.0f, 0.31f, 0.01f}, {0.0f, 0.42f, 0.01f}, 0.062f, 10, pal.balaclava);
        mb.add_sphere({0.0f, 0.52f, 0.01f}, 0.098f, 8, 12, pal.skin_tone);
        mb.add_cylinder({0.0f, 0.42f, 0.015f}, {0.0f, 0.51f, 0.015f}, 0.090f, 10, pal.balaclava); // Balaclava face mask

        if (team == 3) {
            // CT FAST High-Cut Ballistic Helmet
            mb.add_sphere({0.0f, 0.55f, 0.00f}, 0.112f, 8, 12, pal.helmet);
            mb.add_box({0.0f, 0.56f, 0.110f}, {0.022f, 0.028f, 0.014f}, {0.08f, 0.08f, 0.09f, 1.0f}); // NVG Shroud
            mb.add_box({-0.110f, 0.53f, 0.0f}, {0.009f, 0.018f, 0.050f}, {0.08f, 0.08f, 0.09f, 1.0f}); // ARC Rail L
            mb.add_box({+0.110f, 0.53f, 0.0f}, {0.009f, 0.018f, 0.050f}, {0.08f, 0.08f, 0.09f, 1.0f}); // ARC Rail R
            // Comms headset & boom mic
            mb.add_box({-0.112f, 0.50f, 0.01f}, {0.018f, 0.042f, 0.032f}, pal.helmet);
            mb.add_box({+0.112f, 0.50f, 0.01f}, {0.018f, 0.042f, 0.032f}, pal.helmet);
            mb.add_cylinder({-0.115f, 0.48f, 0.02f}, {-0.035f, 0.46f, 0.09f}, 0.004f, 6, {0.12f, 0.12f, 0.12f, 1.0f}); // Mic
            // Tinted Ballistic Goggles
            mb.add_box({0.0f, 0.53f, 0.095f}, {0.080f, 0.028f, 0.018f}, {0.10f, 0.10f, 0.11f, 1.0f});
            mb.add_box({0.0f, 0.53f, 0.106f}, {0.072f, 0.020f, 0.005f}, pal.visor_goggles);
        } else {
            // T Beret / Cap / Aviator shades
            mb.add_sphere({0.0f, 0.56f, -0.01f}, 0.108f, 8, 12, pal.helmet);
            mb.add_box({-0.05f, 0.58f, 0.092f}, {0.014f, 0.018f, 0.007f}, {0.88f, 0.76f, 0.22f, 1.0f}); // Badge
            // Aviator Sunglasses
            mb.add_box({-0.035f, 0.52f, 0.098f}, {0.024f, 0.020f, 0.005f}, {0.10f, 0.10f, 0.12f, 1.0f});
            mb.add_box({+0.035f, 0.52f, 0.098f}, {0.024f, 0.020f, 0.005f}, {0.10f, 0.10f, 0.12f, 1.0f});
            mb.add_box({0.0f, 0.53f, 0.100f}, {0.012f, 0.004f, 0.003f}, pal.visor_goggles);
        }

        // 7. ARMS & TACTICAL GLOVES (Buy-Menu Low-Ready Stance)
        // Right Arm (Trigger hand)
        mb.add_cylinder({+0.21f, 0.31f, 0.0f}, {+0.19f, 0.11f, 0.10f}, 0.062f, 10, pal.uniform_accent);
        mb.add_box({+0.19f, 0.11f, 0.09f}, {0.038f, 0.042f, 0.028f}, pal.knee_pads); // Elbow pad R
        mb.add_cylinder({+0.19f, 0.11f, 0.10f}, {+0.12f, 0.04f, 0.25f}, 0.056f, 10, pal.uniform_accent);

        // Right Glove & Hand holding weapon grip
        mb.add_box({+0.10f, 0.03f, 0.28f}, {0.032f, 0.032f, 0.038f}, pal.gloves_primary);
        mb.add_box({+0.11f, 0.042f, 0.28f}, {0.030f, 0.014f, 0.032f}, pal.gloves_secondary); // Knuckle plate
        mb.add_box({+0.08f, 0.015f, 0.30f}, {0.022f, 0.028f, 0.028f}, pal.gloves_primary); // Wrapped fingers

        // Left Arm (Support hand)
        mb.add_cylinder({-0.21f, 0.31f, 0.0f}, {-0.17f, 0.10f, 0.12f}, 0.062f, 10, pal.uniform_accent);
        mb.add_box({-0.17f, 0.10f, 0.11f}, {0.038f, 0.042f, 0.028f}, pal.knee_pads); // Elbow pad L
        mb.add_cylinder({-0.17f, 0.10f, 0.12f}, {-0.02f, 0.03f, 0.37f}, 0.056f, 10, pal.uniform_accent);

        // Left Glove & Hand cupping forend
        mb.add_box({-0.02f, 0.03f, 0.39f}, {0.032f, 0.032f, 0.038f}, pal.gloves_primary);
        mb.add_box({-0.02f, 0.016f, 0.39f}, {0.030f, 0.014f, 0.032f}, pal.gloves_secondary); // Knuckle plate
        mb.add_box({-0.01f, 0.042f, 0.40f}, {0.028f, 0.022f, 0.032f}, pal.gloves_primary); // Support fingers
    }

    inline mesh generate_agent_with_weapon(int team, int agent_def_index, int weapon_def_index, int paint_kit_id, int glove_def_index, std::string_view weapon_name, std::string_view skin_name) {
        mesh_builder mb;
        const auto pal = get_agent_palette(team, agent_def_index, glove_def_index);

        // 1. Build complete 3D agent character
        build_agent_body(mb, pal, team);

        // 2. Build 3D weapon and attach in hands
        if (weapon_def_index > 0) {
            mesh wep_mesh = generate_weapon_mesh(weapon_def_index, weapon_name);
            const vec4 skin_col = get_skin_theme_color(paint_kit_id, skin_name);

            // Determine placement based on weapon type
            bool is_knife = (weapon_def_index >= 500 && weapon_def_index <= 526) || weapon_def_index == 41 || weapon_def_index == 42 || weapon_def_index == 59;
            bool is_pistol = (weapon_def_index == 1 || weapon_def_index == 2 || weapon_def_index == 3 || weapon_def_index == 4 ||
                              weapon_def_index == 30 || weapon_def_index == 32 || weapon_def_index == 36 || weapon_def_index == 61 || weapon_def_index == 63 || weapon_def_index == 64);
            bool is_glove = (weapon_def_index >= 5027 && weapon_def_index <= 5035);

            if (!is_glove) {
                if (is_knife) {
                    // Knife held in right hand pointing forward
                    mb.append_transformed(wep_mesh, {+0.10f, 0.04f, 0.32f}, {-0.15f, 0.20f, 0.10f}, 0.30f, skin_col);
                } else if (is_pistol) {
                    // Pistol held in hands pointing forward
                    mb.append_transformed(wep_mesh, {+0.05f, 0.03f, 0.33f}, {-0.08f, -0.15f, 0.04f}, 0.28f, skin_col);
                } else {
                    // Primary rifle / SMG / sniper in low-ready buy-menu stance
                    mb.append_transformed(wep_mesh, {+0.04f, 0.04f, 0.33f}, {-0.10f, -0.22f, 0.06f}, 0.38f, skin_col);
                }
            }
        }

        mesh result;
        result.vertices = std::move(mb.vertices);
        finalize_mesh(result, agent_def_index * 10000 + weapon_def_index * 100 + paint_kit_id);
        return result;
    }

} // namespace nemesis::preview3d
