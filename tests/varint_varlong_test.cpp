#include "varint_varlong.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <exception>

void expects(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error{message};
    }
}

std::string vector_bytes_to_string(const std::vector<spiff::mc_byte>& bytes) {
    std::stringstream ss;
    ss << "{";

    for (std::size_t i = 0; i < bytes.size(); ++i) {
        ss << std::hex << "0x" << bytes[i];

        if (i != bytes.size() - 1) {
            ss << ", ";
        }
    }

    ss << "}\n";
    return ss.str();
}

int main() {
    // Sample from https://wiki.vg/VarInt_And_VarLong
    // std::array<std::pair<int, std::vector<spiff::mc_byte>>, 11> sample_varints = { 
    //     std::make_pair(0, std::vector<spiff::mc_byte>{0x00}), 
    //     std::make_pair(1, std::vector<spiff::mc_byte>{0x01}),
    //     std::make_pair(2, std::vector<spiff::mc_byte>{0x02}), 
    //     std::make_pair(127, std::vector<spiff::mc_byte>{0x7f}), 
    //     std::make_pair(128, std::vector<spiff::mc_byte>{spiff::mc_byte(0x80), 0x01}), 
    //     std::make_pair(255, std::vector<spiff::mc_byte>{0xff, 0x01}), 
    //     std::make_pair(25565, std::vector<spiff::mc_byte>{0xdd, 0xc7, 0x01}),
    //     std::make_pair(2097151, std::vector<spiff::mc_byte>{0xff, 0xff, 0x7f}), 
    //     std::make_pair(2147483647, std::vector<spiff::mc_byte>{0xff, 0xff, 0xff, 0xff, 0x07}), 
    //     std::make_pair(-1, std::vector<spiff::mc_byte>{0xff, 0xff, spiff::mc_byte{0xff}, spiff::mc_byte{0xff}, spiff::mc_byte{0x0f}}), 
    //     std::make_pair(-214783648, std::vector<spiff::mc_byte>{0x80, spiff::mc_byte{0x80}, spiff::mc_byte{0x80}, spiff::mc_byte{0x80}, spiff::mc_byte{0x08}}), 
    // };

    // for (std::size_t i = 0; i < sample_varints.size(); ++i) {
    //     const auto [sample_varint_value, sample_varint_bytes] = sample_varints[i];
    //     const auto result_write = spiff::minecraft_varint{sample_varint_value};
    //     const auto result_read = spiff::minecraft_varint{sample_varint_bytes};

    //     expects(result_write.bytes() == sample_varint_bytes, 
    //         "write_var_int: Result bytes did not equal the sample VarInt bytes: " + vector_bytes_to_string(result_write.bytes()) + " != " + vector_bytes_to_string(sample_varint_bytes));
        
    //     expects(result_write.value() == sample_varint_value, 
    //         "write_var_int: Result value did not equal the sample VarInt value: " + std::to_string(result_write.value()) + " != " + std::to_string(sample_varint_value));

    //     expects(result_read.bytes() == sample_varint_bytes, 
    //         "read_var_int: Result bytes did not equal the sample VarInt bytes: " + vector_bytes_to_string(result_read.bytes()) + " != " + vector_bytes_to_string(sample_varint_bytes));
        
    //     expects(result_read.value() == sample_varint_value, 
    //         "read_var_int: Result value did not equal the sample VarInt value: " + std::to_string(result_read.value()) + " != " + std::to_string(sample_varint_value));

    // }
}