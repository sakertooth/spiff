#include "varint_varlong.hpp"
#include <iostream>

namespace spiff {
    constexpr mc_int segment_bits = 0x7f;
    constexpr mc_int continue_bit = 0x80;

    minecraft_varint::minecraft_varint(mc_int value) : m_value(value), m_bytes(write_varint(value)) {}
    minecraft_varint::minecraft_varint(const std::vector<mc_byte>& bytes) : m_value(read_varint(bytes)), m_bytes(bytes.begin(), bytes.begin() + calculate_varint_varlong_length(bytes)) {}

    mc_int minecraft_varint::value() const {
        return m_value;
    }

    const std::vector<mc_byte>& minecraft_varint::bytes() const {
        return m_bytes;
    }

    mc_int minecraft_varint::read_varint(const std::vector<mc_byte>& bytes) {
        mc_int value = 0;
        mc_int position = 0;
        int byte_read_marker = 0;

        while (true) {
            const auto current_byte = bytes.at(byte_read_marker++);
            value |= (current_byte & segment_bits) << position;

            if ((current_byte & continue_bit) == 0) break;

            position += 7;
            if (position >= 32) throw std::runtime_error("VarInt is too big");
        }

        return value;
    }

    std::vector<mc_byte> minecraft_varint::write_varint(mc_int value) {
        std::vector<mc_byte> result;
        while (true) {
            if ((value & ~segment_bits) == 0) {
                result.push_back(value);
                return result;
            }

            result.push_back((value & segment_bits) | continue_bit);
            value = static_cast<mc_int>(static_cast<mc_uint>(value) >> 7);
        }

        return result;
    }

//  --------------------------------------------------------------------------------------------

    minecraft_varlong::minecraft_varlong(mc_long value) : m_value(value), m_bytes(write_varlong(value)) {}
    minecraft_varlong::minecraft_varlong(const std::vector<mc_byte>& bytes) : m_value(read_varlong(bytes)), m_bytes(bytes.begin(), bytes.begin() + calculate_varint_varlong_length(bytes))  {}

    mc_long minecraft_varlong::value() const {
        return m_value;
    }

    const std::vector<mc_byte>& minecraft_varlong::bytes() const {
        return m_bytes;
    }

    mc_long minecraft_varlong::read_varlong(const std::vector<mc_byte>& bytes) {
        long value = 0;
        int position = 0;
        mc_byte current_byte;

        int byte_read_marker = 0;
        while (true) {
            current_byte = bytes.at(byte_read_marker++);
            value |= (long) (current_byte & segment_bits) << position;

            if ((current_byte & continue_bit) == 0) break;

            position += 7;
            if (position >= 64) throw std::runtime_error{"VarLong is too big"};
        }

        return value;
    }

    std::vector<mc_byte> minecraft_varlong::write_varlong(mc_long value) {
        std::vector<mc_byte> result;
        while (true) {
            if ((value & ~segment_bits) == 0) {
                result.push_back(value);
                return result;
            }

            result.push_back((value & segment_bits) | continue_bit);
            value = static_cast<mc_int>(static_cast<mc_uint>(value) >> 7);
        }

        return result;
    }

//  --------------------------------------------------------------------------------------------

    int calculate_varint_varlong_length(const std::vector<mc_byte>& bytes) {
        int count = 0;

        for (const auto current_byte : bytes) {
            ++count;
            if ((current_byte & continue_bit) == 0) break;
        }

        if ((bytes.back() & continue_bit) != 0) {
            throw std::out_of_range{"Expected more VarInt data but has stopped at the end"};
        }

        return count;
    }
}