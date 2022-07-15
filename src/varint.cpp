#include "varint.hpp"

#include <cstddef>
#include <vector>

namespace spiff {
    constexpr int segment_bits = 0x7F;
    constexpr int continue_bit = 0x80;

    minecraft_varint::minecraft_varint(int value) : m_value(value)  {
        //Write in Varint      
        while (true) {
            if ((value & ~segment_bits) == 0) {
                m_bytes.push_back(static_cast<std::byte>(value));
                break;
            }

            m_bytes.push_back(static_cast<std::byte>((value & segment_bits) | continue_bit));
            value >>= 7;
        }
    }

    minecraft_varint::minecraft_varint(const std::vector<std::byte>& bytes) {
        //Read in VarInt
        int position = 0;
        std::byte current_byte;

        int byte_read_marker = 0;
        while (true) {
            current_byte = bytes[byte_read_marker++];
            m_value |= std::to_integer<int>((current_byte & std::byte{segment_bits})) << position;

            if (std::to_integer<int>(current_byte & std::byte{continue_bit}) == 0) {
                break;
            }

            position += 7;
            if (position >= 32) {
                throw std::runtime_error{"VarInt is too big"};
            }
        }
        
        m_bytes = std::vector<std::byte> {bytes.begin(), bytes.begin() + byte_read_marker};
    }

    const std::vector<std::byte>& minecraft_varint::bytes() const {
        return m_bytes;
    }

    int minecraft_varint::value() const {
        return m_value;
    }

    minecraft_varlong::minecraft_varlong(long value) : m_value(value) {
        //Write in VarLong
        while (true) {
            if ((value & ~(static_cast<long>(segment_bits))) == 0) {
                m_bytes.push_back(static_cast<std::byte>(value));
                break;
            }

            m_bytes.push_back(static_cast<std::byte>((value & segment_bits) | continue_bit));
            value >>= 7;
        }
    }

    minecraft_varlong::minecraft_varlong(const std::vector<std::byte>& bytes) {
        //Read in VarLong
        int position = 0;
        std::byte current_byte;

        int byte_read_marker = 0;
        while (true) {
            current_byte = bytes[byte_read_marker++];
            m_value |= std::to_integer<long>(current_byte & std::byte{segment_bits}) << position;

            if (std::to_integer<long>(current_byte & std::byte{continue_bit}) == 0) {
                break;
            }

            position += 7;
            if (position >= 64) {
                throw std::runtime_error("VarLong is too big");
            }
        }

        m_bytes = std::vector<std::byte> {bytes.begin(), bytes.begin() + byte_read_marker};
    }

    const std::vector<std::byte>& minecraft_varlong::bytes() const {
        return m_bytes;
    }
    
    long minecraft_varlong::value() const {
        return m_value;
    }
}