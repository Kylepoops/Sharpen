#include <sharpen/MicroRpcStack.hpp>

sharpen::MicroRpcStack::MicroRpcStack()
    :size_(0)
    ,fields_()
{}

sharpen::MicroRpcStack::MicroRpcStack(Self &&other) noexcept
    :size_(other.size_)
    ,fields_(std::move(other.fields_))
{
    other.size_ = 0;
}

void sharpen::MicroRpcStack::UnsafeCopyTo(char *data) const noexcept
{
    sharpen::Size size{0};
    for (auto begin = this->fields_.begin(), end = this->fields_.end(); begin != end;)
    {
        auto cbegin = begin;
        cbegin->CopyTo(++begin == end, data + size, cbegin->GetRawSize());
        size += cbegin->GetRawSize();
    }
}

void sharpen::MicroRpcStack::CopyTo(sharpen::ByteBuffer &buf, sharpen::Size offset) const
{
    if (buf.GetSize() < this->ComputeSize() + offset)
    {
        buf.ExtendTo(this->ComputeSize() + offset);
    }
    this->UnsafeCopyTo(buf.Data());
}

void sharpen::MicroRpcStack::CopyTo(char *data, sharpen::Size size) const
{
    if (size < this->ComputeSize())
    {
        throw std::length_error("buf too small");
    }
    this->UnsafeCopyTo(data);
}

sharpen::Size sharpen::MicroRpcStack::ComputeSize() const noexcept
{
    sharpen::Size totalSize{0};
    for (auto begin = this->fields_.begin(), end = this->fields_.end(); begin != end; ++begin)
    {
        totalSize += begin->GetRawSize();
    }
    return totalSize;
}

sharpen::MicroRpcStack &sharpen::MicroRpcStack::operator=(const Self &other)
{
    if (this != std::addressof(other))
    {
        Self tmp{other};
        std::swap(tmp,*this);   
    }
    return *this;
}

sharpen::MicroRpcStack &sharpen::MicroRpcStack::operator=(Self &&other) noexcept
{
    if(this != std::addressof(other))
    {
        this->size_ = other.size_;
        this->fields_ = std::move(other.fields_);
        other.size_ = 0;
    }
    return *this;
}