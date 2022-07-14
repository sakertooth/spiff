#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace spiff
{
    constexpr int segment_bits = 0x7F;
    constexpr int continue_bit = 0x80;

    int read_varint(const std::vector<std::byte>& buf) {
        int value = 0;
        int position = 0;
        std::byte current_byte;

        int byte_read_marker = 0;
        while (true) {
            current_byte = buf.at(byte_read_marker++);
            value |= std::to_integer<int>((current_byte & std::byte{segment_bits}) << position);

            if (std::to_integer<int>(current_byte & std::byte{continue_bit}) == 0) {
                break;
            }

            position += 7;
            if (position >= 32) {
                throw std::runtime_error{"VarInt is too big"};
            }
        }

        return value;
    }

    long read_varlong(const std::vector<std::byte>& buf) {
        long value = 0;
        int position = 0;
        std::byte current_byte;

        int byte_read_marker = 0;
        while (true) {
            current_byte = buf.at(byte_read_marker++);
            value |= std::to_integer<long>(current_byte & std::byte{segment_bits}) << position;

            if (std::to_integer<long>(current_byte & std::byte{continue_bit}) == 0) {
                break;
            }

            position += 7;
            if (position >= 64) {
                throw std::runtime_error("VarLong is too big");
            }
        }

        return value;
    }

    std::vector<std::byte> write_varint(int value) {
        std::vector<std::byte> result;
        
        while (true) {
            if ((value & ~segment_bits) == 0) {
                result.push_back(static_cast<std::byte>(value));
                return result;
            }

            result.push_back(static_cast<std::byte>((value & segment_bits) | continue_bit));
            value >>= 7;
        }

        return result;
    }

    std::vector<std::byte> write_varlong(long value) {
        std::vector<std::byte> result;

        while (true) {
            if ((value & ~(static_cast<long>(segment_bits))) == 0) {
                result.push_back(static_cast<std::byte>(value));
                return result;
            }

            result.push_back(static_cast<std::byte>((value & segment_bits) | continue_bit));
            value >>= 7;
        }

        return result;
    }
}