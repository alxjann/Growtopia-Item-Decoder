#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

class Reader {
public:
    template <typename T>
    T read() {
        T val;
        std::memcpy(&val, m_data + m_position, sizeof(T));
        m_position += sizeof(T);
        return val;
    }
    void skipString() { m_position += read<std::uint16_t>(); };
    void skipStrings(std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            skipString();
        }
    }
    void skip(std::size_t length) { m_position += length; }
public:
    Reader(uint8_t* data, std::size_t len) : m_position(0) {
        m_data = new uint8_t[len];
        std::memset(m_data, 0, len);
        std::memcpy(m_data, data, len);
    }
    Reader(void* pData) : m_data(static_cast<std::uint8_t*>(pData)), m_position(0) {}
    ~Reader() = default;

private:
    std::uint8_t* m_data;
    std::size_t m_position;
};