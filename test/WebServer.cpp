#include <cstdio>
#include <string>
#include <sharpen/EventEngine.hpp>
#include <sharpen/INetStreamChannel.hpp>
#include <sharpen/IpEndPoint.hpp>
#include <sharpen/CtrlHandler.hpp>
#include <cstring>
#include <sharpen/ProcessInfo.hpp>
#include <sharpen/HttpServer.hpp>

class TestHttpServer:public sharpen::HttpServer
{
private:
protected:
    void OnNewMessage(sharpen::NetStreamChannelPtr channel,const sharpen::HttpRequest &req,sharpen::HttpResponse &res) override
    {
        const char content[] = "hello world";
        res.Header()["Content-Type"] = "text/plain";
        res.Header()["Content-Length"] = std::to_string(sizeof(content) - 1);
        res.Body().CopyFrom(content,sizeof(content) - 1);
    }
public:
    explicit TestHttpServer(const sharpen::IEndPoint &endpoint)
        :sharpen::HttpServer(sharpen::AddressFamily::Ip,endpoint,sharpen::EventEngine::GetEngine(),"IIS/Linux")
    {}

    ~TestHttpServer() noexcept = default;
};

void WebTest(sharpen::Size num)
{
    sharpen::StartupNetSupport();
    sharpen::EventEngine &engine = sharpen::EventEngine::SetupEngine(num);
    sharpen::IpEndPoint addr;
    addr.SetAddrByString("0.0.0.0");
    addr.SetPort(8080);
    TestHttpServer server(addr);
    char ip[21];
    std::memset(ip,0,sizeof(ip));
    addr.GetAddrSring(ip,sizeof(ip));
    std::printf("now listen on %s:%d\n",ip,addr.GetPort());
    std::printf("use ctrl + c to stop\n");
    sharpen::RegisterCtrlHandler(sharpen::CtrlType::Interrupt,[]()
    {
        std::puts("stop now\n");
        sharpen::EventEngine::GetEngine().Stop();
        std::puts("cleanup network support\n");
        sharpen::CleanupNetSupport();
    });
    engine.LaunchAndRun([&server]()
    {
        server.RunAsync();
    });
}

int main(int argc, char const *argv[])
{
    std::printf("run in %u cores machine\nprocess id: %u\n",std::thread::hardware_concurrency(),sharpen::GetProcessId());
    sharpen::Size num = std::thread::hardware_concurrency();
    if (argc > 1)
    {
        num = std::atoi(argv[1]);
    }
    WebTest(num);
    return 0;
}