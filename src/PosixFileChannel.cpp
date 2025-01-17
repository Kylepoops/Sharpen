#include <sharpen/PosixFileChannel.hpp>

#ifdef SHARPEN_HAS_POSIXFILE

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cassert>

#include <sharpen/SystemError.hpp>
#include <sharpen/EventLoop.hpp>

sharpen::PosixFileChannel::PosixFileChannel(sharpen::FileHandle handle)
    :MyBase()
{
    assert(handle != -1);
    this->handle_ = handle;
}

void sharpen::PosixFileChannel::WriteAsync(const sharpen::Char *buf,sharpen::Size bufSize,sharpen::Uint64 offset,sharpen::Future<sharpen::Size> &future)
{
    if (!this->IsRegistered())
    {
        throw std::logic_error("should register to a loop first");
    }
    sharpen::FileHandle fd = this->handle_;
    this->loop_->RunInLoop([buf,bufSize,offset,&future,fd]() mutable
    {
        ssize_t r = ::pwrite(fd,buf,bufSize,offset);
        if (r < 0)
        {
            future.Fail(sharpen::MakeLastErrorPtr());
            return;
        }
        future.Complete(r);
    });
}
        
void sharpen::PosixFileChannel::WriteAsync(const sharpen::ByteBuffer &buf,sharpen::Size bufferOffset,sharpen::Uint64 offset,sharpen::Future<sharpen::Size> &future)
{
    if (buf.GetSize() < bufferOffset)
    {
        throw std::length_error("buffer size is wrong");
    }
    this->WriteAsync(buf.Data() + bufferOffset,buf.GetSize() - bufferOffset,offset,future);
}

void sharpen::PosixFileChannel::ReadAsync(sharpen::Char *buf,sharpen::Size bufSize,sharpen::Uint64 offset,sharpen::Future<sharpen::Size> &future)
{
    if (!this->IsRegistered())
    {
        throw std::logic_error("should register to a loop first");
    }
    sharpen::FileHandle fd = this->handle_;
    this->loop_->RunInLoop([buf,bufSize,offset,&future,fd]() mutable
    {
        ssize_t r = ::pread(fd,buf,bufSize,offset);
        if (r < 0)
        {
            future.Fail(sharpen::MakeLastErrorPtr());
            return;
        }
        future.Complete(r);
    });
}
        
void sharpen::PosixFileChannel::ReadAsync(sharpen::ByteBuffer &buf,sharpen::Size bufferOffset,sharpen::Uint64 offset,sharpen::Future<sharpen::Size> &future)
{
    if (buf.GetSize() < bufferOffset)
    {
        throw std::length_error("buffer size is wrong");
    }
    this->ReadAsync(buf.Data() + bufferOffset,buf.GetSize() - bufferOffset,offset,future);
}

void sharpen::PosixFileChannel::OnEvent(sharpen::IoEvent *event)
{
    //do nothing
    (void)event;
}

sharpen::Uint64 sharpen::PosixFileChannel::GetFileSize() const
{
    struct stat buf;
    int r = ::fstat(this->handle_,&buf);
    if (r == -1)
    {
        sharpen::ThrowLastError();
    }
    return buf.st_size;
}

void sharpen::PosixFileChannel::Register(sharpen::EventLoop *loop)
{
    this->loop_ = loop;
}

sharpen::FileMemory sharpen::PosixFileChannel::MapMemory(sharpen::Size size,sharpen::Uint64 offset)
{
    void *addr = ::mmap64(nullptr,size,PROT_READ|PROT_WRITE,MAP_SHARED,this->handle_,offset);
    if(addr == MAP_FAILED)
    {
        sharpen::ThrowLastError();
    }
    return {addr,size};
}

#endif