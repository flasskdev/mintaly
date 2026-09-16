#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace utilities {

    // External synchronization is required. Entry provides index and type.
    // Dense storage retains insertion/swap-removal order; snapshots preserve
    // that exact order rather than iterating a hash table or sorting entities.
    template <typename Entry, std::size_t Capacity, std::size_t TypeCount>
    class indexed_cache
    {
        static_assert(Capacity <= std::numeric_limits<std::uint32_t>::max());
        static_assert(std::is_nothrow_copy_assignable_v<Entry>);

    public:
        void reserve(std::size_t count) { m_entries.reserve(count); }
        [[nodiscard]] bool empty() const { return m_entries.empty(); }
        [[nodiscard]] const std::vector<Entry>& entries() const { return m_entries; }

        [[nodiscard]] const Entry* find(std::size_t index) const
        {
            if (index >= Capacity || m_positions[index] == 0)
                return nullptr;
            return &m_entries[m_positions[index] - 1];
        }

        bool insert(const Entry& entry)
        {
            const auto index = static_cast<std::size_t>(entry.index);
            if (index >= Capacity || m_positions[index] != 0)
                return false;
            m_entries.push_back(entry);
            m_positions[index] = static_cast<std::uint32_t>(m_entries.size());
            m_snapshot_valid.fill(false);
            return true;
        }

        bool erase(std::size_t index)
        {
            if (index >= Capacity || m_positions[index] == 0)
                return false;
            const auto position = static_cast<std::size_t>(m_positions[index] - 1);
            if (position + 1 != m_entries.size())
            {
                m_entries[position] = m_entries.back();
                m_positions[static_cast<std::size_t>(m_entries[position].index)] =
                    static_cast<std::uint32_t>(position + 1);
            }
            m_entries.pop_back();
            m_positions[index] = 0;
            // Swap-removal may reorder a different type as well.
            m_snapshot_valid.fill(false);
            return true;
        }

        void clear()
        {
            m_entries.clear();
            m_positions.fill(0);
            for (auto& snapshot : m_snapshots)
                snapshot.clear();
            m_snapshot_valid.fill(true);
        }

        // Read-only fast path, safe under the caller's shared lock.
        [[nodiscard]] const std::vector<Entry>* snapshot_if_ready(std::size_t type) const
        {
            if (type >= TypeCount)
                return &m_empty;
            return m_snapshot_valid[type] ? &m_snapshots[type] : nullptr;
        }

        // Rebuild only after mutation, under the caller's exclusive lock.
        [[nodiscard]] const std::vector<Entry>& snapshot(std::size_t type)
        {
            if (const auto ready = snapshot_if_ready(type))
                return *ready;
            auto& result = m_snapshots[type];
            result.clear();
            for (const auto& entry : m_entries)
                if (static_cast<std::size_t>(entry.type) == type)
                    result.push_back(entry);
            m_snapshot_valid[type] = true;
            return result;
        }

    private:
        std::vector<Entry> m_entries;
        std::array<std::uint32_t, Capacity> m_positions{};
        std::array<std::vector<Entry>, TypeCount> m_snapshots{};
        std::array<bool, TypeCount> m_snapshot_valid{};
        const std::vector<Entry> m_empty{};
    };
}
