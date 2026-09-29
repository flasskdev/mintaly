#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <utilities/diag.hpp>

// Temporary telemetry only. No thresholds, trace layouts or firing decisions
// are changed. Counters are shared by translation units and safe for scan workers.
namespace utilities::rage_scan_diagnostics {
    enum class event : std::size_t
    {
        entry,
        gun_calls,
        targets,
        no_targets,
        scan_empty,
        scan_nonempty,
        scan_hits,
        selection,
        selected,
        fire_calls,
        attack_set,
        prepare_calls,
        prepare_invalid,
        prepare_fov,
        prepare_geometry,
        prepared_points,
        points,
        invalid_hitbox,
        fov,
        budget,
        penetration_failed,
        damage,
        headgroup,
        accepted,
        pen_calls,
        no_weapon_damage,
        tls_failed,
        invalid_filter,
        no_trace_hits,
        contacts,
        damage_exhausted,
        contact_mismatch,
        no_target_hit,
        geometry_miss,
        pen_success,
        count
    };

    inline constexpr std::array names{
        "entry",
        "gun_calls",
        "targets",
        "no_targets",
        "scan_empty",
        "scan_nonempty",
        "scan_hits",
        "selection",
        "selected",
        "fire_calls",
        "attack_set",
        "prepare_calls",
        "prepare_invalid",
        "prepare_fov",
        "prepare_geometry",
        "prepared_points",
        "points",
        "invalid_hitbox",
        "fov",
        "budget",
        "penetration_failed",
        "damage",
        "headgroup",
        "accepted",
        "pen_calls",
        "no_weapon_damage",
        "tls_failed",
        "invalid_filter",
        "no_trace_hits",
        "contacts",
        "damage_exhausted",
        "contact_mismatch",
        "no_target_hit",
        "geometry_miss",
        "pen_success"
    };
    static_assert(names.size() == static_cast<std::size_t>(event::count));
    inline std::array<std::atomic<std::uint64_t>, names.size()> counters{};

    inline void mark(event id, std::uint64_t amount = 1) noexcept
    {
        counters[static_cast<std::size_t>(id)].fetch_add(amount, std::memory_order_relaxed);
    }

    inline void report_group(const char* group,
        const std::array<std::uint64_t, names.size()>& snapshot,
        std::initializer_list<event> fields)
    {
        char line[1024]{};
        const auto prefix = std::snprintf(line, sizeof(line), "[rage-diag:%s]", group);
        if (prefix < 0 || static_cast<std::size_t>(prefix) >= sizeof(line)) return;
        auto used = static_cast<std::size_t>(prefix);
        for (const auto field : fields)
        {
            const auto index = static_cast<std::size_t>(field);
            const auto available = sizeof(line) - used;
            const auto written = std::snprintf(line + used, available, " %s=%llu",
                names[index], static_cast<unsigned long long>(snapshot[index]));
            if (written < 0 || static_cast<std::size_t>(written) >= available) break;
            used += static_cast<std::size_t>(written);
        }
        diag::write(diag::level::debug, line);
    }

    // Called by the existing owner-thread, five-second performance reporter.
    // Counts are events in the interval, not unique targets or shots on the server.
    inline void report()
    {
        std::array<std::uint64_t, names.size()> snapshot{};
        for (std::size_t i = 0; i < snapshot.size(); ++i)
            snapshot[i] = counters[i].exchange(0, std::memory_order_relaxed);
        report_group("pipeline", snapshot, {
            event::entry, event::gun_calls, event::targets, event::no_targets, event::scan_empty, event::scan_nonempty, event::scan_hits, event::selection, event::selected, event::fire_calls, event::attack_set
        });
        report_group("scan", snapshot, {
            event::prepare_calls, event::prepare_invalid, event::prepare_fov, event::prepare_geometry, event::prepared_points, event::points, event::invalid_hitbox, event::fov, event::budget, event::penetration_failed, event::damage, event::headgroup, event::accepted
        });
        report_group("penetration", snapshot, {
            event::pen_calls, event::no_weapon_damage, event::tls_failed, event::invalid_filter, event::no_trace_hits, event::contacts, event::damage_exhausted, event::contact_mismatch, event::no_target_hit, event::geometry_miss, event::pen_success
        });
    }
}
