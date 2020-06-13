#pragma once

#include <algorithm>
#include <cassert>
#include <vector>
#include <string>
#include <string_view>
    
namespace zysoft
{
namespace net
{

class Buffer 
{
public:
    static const std::size_t PrependBytes = 8;
    static const std::size_t InitSize = 1024;

public:
    Buffer(std::size_t n = InitSize)
        : buffer_(PrependBytes + n)
        , read_pos_(PrependBytes)
        , write_pos_(PrependBytes)
    {
        assert(ReadableBytes() == 0);
        assert(WritableBytes() == n);
    }

    ~Buffer() = default;
    Buffer(const Buffer& rhs) = default;
    Buffer& operator=(const Buffer& rhs) = default;
    Buffer(Buffer&& rhs) = default;
    Buffer& operator=(Buffer&& rhs) = default;

    bool Empty() const
    {
        return read_pos_ == write_pos_;
    }

    void Clear()
    {
        read_pos_ = PrependBytes;
        write_pos_ = PrependBytes;
    }

    std::size_t ReadableBytes() const
    {
        return write_pos_ - read_pos_;
    }

    std::size_t WritableBytes() const
    {
        return buffer_.size() - write_pos_;
    }

    const char* ReadablePtr() const
    {
        return Data() + read_pos_;
    }

    void Consume(std::size_t len)
    {
        assert(len <= ReadableBytes());
        if (len < ReadableBytes()) {
            read_pos_ += len;
        } else {
            Clear();
        }
    }

    void Append(const std::string& s)
    {
        Append(s.data(), s.size());
    }

    void Append(std::string_view& s)
    {
        Append(s.data(), s.size());
    }

    void Append(const void* data, std::size_t len)
    {
        if (!data || len == 0)
            return;
        PrepareWritableBytes(len);
        const char* p = static_cast<const char*>(data);
        std::copy(p, p + len, Data() + write_pos_);
        write_pos_ += len;
    }

    void Shrink()
    {
        std::size_t readable = ReadableBytes();
        std::copy(Data() + read_pos_, Data() + write_pos_, Data() + PrependBytes);
        read_pos_ = PrependBytes;
        write_pos_ = read_pos_ + readable;
        buffer_.resize(write_pos_);
        buffer_.shrink_to_fit();
    }

    void Swap(Buffer& rhs)
    {
        std::swap(buffer_, rhs.buffer_);
        std::swap(read_pos_, rhs.read_pos_);
        std::swap(write_pos_, rhs.write_pos_);
    }

private:
    char* Data()
    {
        return buffer_.data();
    }

    const char* Data() const
    {
        return buffer_.data();
    }

    void MakeSpace(std::size_t len)
    {
        if (read_pos_ - PrependBytes + WritableBytes() < len) {
            buffer_.resize(write_pos_ + len);
        } else {
            std::size_t readable = ReadableBytes();
            std::copy(Data() + read_pos_, Data() + write_pos_, Data() + PrependBytes);
            read_pos_ = PrependBytes;
            write_pos_ = read_pos_ + readable;
        }
    }

    void PrepareWritableBytes(std::size_t len)
    {
        if (WritableBytes() < len) {
            MakeSpace(len);
        }
        assert(WritableBytes() >= len);
    }

private:
    std::vector<char> buffer_;
    std::size_t read_pos_;
    std::size_t write_pos_;
};

} // namespace net
} // namespace zysoft

