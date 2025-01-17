#pragma once
#ifndef _SHARPEN_INETSTREAMCHANNEL_HPP
#define _SHARPEN_INETSTREAMCHANNEL_HPP

#include "IChannel.hpp"
#include "Noncopyable.hpp"
#include "IAsyncReadable.hpp"
#include "IAsyncWritable.hpp"
#include "IFileChannel.hpp"
#include "IEndPoint.hpp"

namespace sharpen
{

    class INetStreamChannel;

    using NetStreamChannelPtr = std::shared_ptr<sharpen::INetStreamChannel>;

    class INetStreamChannel:public sharpen::IChannel,public sharpen::IAsyncWritable,public sharpen::IAsyncReadable
    {
    private:
        using Self = sharpen::INetStreamChannel;
    public:
        
        INetStreamChannel() = default;
        
        virtual ~INetStreamChannel() noexcept = default;
        
        INetStreamChannel(const Self &) = default;
        
        INetStreamChannel(Self &&) noexcept = default;

        virtual void SendFileAsync(sharpen::FileChannelPtr file,sharpen::Uint64 size,sharpen::Uint64 offset,sharpen::Future<void> &future) = 0;
        
        virtual void SendFileAsync(sharpen::FileChannelPtr file,sharpen::Future<void> &future) = 0;

        void SendFileAsync(sharpen::FileChannelPtr file,sharpen::Uint64 size,sharpen::Uint64 offset);

        void SendFileAsync(sharpen::FileChannelPtr file);

        virtual void AcceptAsync(sharpen::Future<sharpen::NetStreamChannelPtr> &future) = 0;

        sharpen::NetStreamChannelPtr AcceptAsync();

        virtual void ConnectAsync(const sharpen::IEndPoint &endpoint,sharpen::Future<void> &future) = 0;

        void ConnectAsync(const sharpen::IEndPoint &endpoint);

        void Bind(const sharpen::IEndPoint &endpoint);

        virtual void Listen(sharpen::Uint16 queueLength);

        void GetLocalEndPoint(sharpen::IEndPoint &endPoint) const;

        void GetRemoteEndPoint(sharpen::IEndPoint &endPoint) const;

        void SetKeepAlive(bool val);

        void SetReuseAddress(bool val);

        int GetErrorCode() const noexcept;

        virtual void PollReadAsync(sharpen::Future<void> &future) = 0;

        virtual void PollWriteAsync(sharpen::Future<void> &future) = 0;

        void PollReadAsync();

        void PollWriteAsync();

        virtual void Cancel() noexcept = 0;
    };

    enum class AddressFamily
    {
        Ip,
        Ipv6
    };

    sharpen::NetStreamChannelPtr MakeTcpStreamChannel(sharpen::AddressFamily af);

    void StartupNetSupport();

    void CleanupNetSupport();
}

#endif
