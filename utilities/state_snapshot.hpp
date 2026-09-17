#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace utilities {

    // Allocation-free for ordinary prediction snapshots, with an unbounded
    // overflow path for larger layouts. Saved addresses must remain writable.
    template <std::size_t EntryCapacity = 160, std::size_t ByteCapacity = 4096>
    class state_snapshot
    {
    public:
        state_snapshot() = default;
        ~state_snapshot() { restore(); }
        state_snapshot(const state_snapshot&) = delete;
        state_snapshot& operator=(const state_snapshot&) = delete;

        void save_raw(std::uintptr_t address, std::size_t size)
        {
            append(address, reinterpret_cast<const void*>(address), size);
        }

        void restore() noexcept
        {
            for (auto i = m_count; i > 0; --i)
            {
                const auto& saved = i <= EntryCapacity ? m_entries[i - 1]
                    : m_extra_entries[i - 1 - EntryCapacity];
                const auto* bytes = saved.external ? m_extra_bytes.data() : m_bytes.data();
                std::memcpy(reinterpret_cast<void*>(saved.address), bytes + saved.offset, saved.size);
            }
            m_count = 0;
            m_used = 0;
            m_extra_entries.clear();
            m_extra_bytes.clear();
        }

    protected:
        void append(std::uintptr_t address, const void* source, std::size_t size)
        {
            if (size == 0) return;
            const auto external = size > ByteCapacity - m_used;
            const auto offset = external ? m_extra_bytes.size() : m_used;
            if (external)
            {
                if (size > m_extra_bytes.max_size() - offset)
                    throw std::length_error("prediction snapshot is too large");
                m_extra_bytes.resize(offset + size);
            }
            auto* bytes = external ? m_extra_bytes.data() : m_bytes.data();
            std::memcpy(bytes + offset, source, size);
            const entry saved{address, offset, size, external};
            // Commit counts only after a potentially throwing allocation.
            if (m_count < EntryCapacity) m_entries[m_count] = saved;
            else m_extra_entries.push_back(saved);
            ++m_count;
            if (!external) m_used += size;
        }

    private:
        struct entry
        {
            std::uintptr_t address;
            std::size_t offset;
            std::size_t size;
            bool external;
        };
        // No value initialization: only explicitly saved bytes are ever read.
        std::array<entry, EntryCapacity> m_entries;
        std::array<std::uint8_t, ByteCapacity> m_bytes;
        std::vector<entry> m_extra_entries;
        std::vector<std::uint8_t> m_extra_bytes;
        std::size_t m_count{};
        std::size_t m_used{};
    };
}
