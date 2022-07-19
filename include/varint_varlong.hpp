#pragma once
#include <stdexcept>
#include <vector>
#include "mctypes.hpp"

namespace spiff {

    class minecraft_varint {
    public:
        minecraft_varint() = default;
        explicit minecraft_varint(mc_int value);
        explicit minecraft_varint(const std::vector<mc_byte>& bytes);

        mc_int value() const;
        const std::vector<mc_byte>& bytes() const;
    private:
        mc_int read_varint(const std::vector<mc_byte>& bytes);
        std::vector<mc_byte> write_varint(mc_int value);
        
    private:
        mc_int m_value = 0;
        std::vector<mc_byte> m_bytes;
    };

    class minecraft_varlong {
    public:
        minecraft_varlong() = default;
        explicit minecraft_varlong(mc_long value);
        explicit minecraft_varlong(const std::vector<mc_byte>& bytes);

        mc_long value() const;
        const std::vector<mc_byte>& bytes() const;
    private:
        mc_long read_varlong(const std::vector<mc_byte>& bytes);
        std::vector<mc_byte> write_varlong(mc_long value);
        
    private:
        mc_long m_value = 0;
        std::vector<mc_byte> m_bytes;
    };

    int calculate_varint_varlong_length(const std::vector<mc_byte>& bytes);
}