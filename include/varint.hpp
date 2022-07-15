#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace spiff {
    class minecraft_varint {
    public:
        minecraft_varint() = default;
        minecraft_varint(int value);
        minecraft_varint(const std::vector<std::byte>& buf);

        const std::vector<std::byte>& bytes() const;
        int value() const;
    private:
        std::vector<std::byte> m_bytes;
        int m_value = 0;
    };

    class minecraft_varlong {
    public:
        minecraft_varlong() = default;
        minecraft_varlong(long value);
        minecraft_varlong(const std::vector<std::byte>& buf);

        const std::vector<std::byte>& bytes() const;
        long value() const;
    private:
        std::vector<std::byte> m_bytes;
        long m_value = 0;
    };
}