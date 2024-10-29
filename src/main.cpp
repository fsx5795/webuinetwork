#if defined (WIN32) || defined (__WIN32)
#include <WinSock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <iostream>
#include <webui.hpp>
void get_ips(webui::window::event *e)
{
    char buf[100] = { '\0' };
    std::string ips;
    if (gethostname(buf, sizeof(buf)) == 0) {
        struct hostent *he = gethostbyname(buf);
        if (he != nullptr) {
            for (int i{0}; he->h_addr_list[i] != nullptr; ++i) {
                char *localIp = inet_ntoa(*reinterpret_cast<struct in_addr*>(he->h_addr_list[i]));
                if (localIp != nullptr && strcmp("127.0.0.1", localIp) == 0)
                    continue;
                ips.append(localIp);
                ips.push_back(',');
            }
        }
    }
    if (ips.length() != 0)
        ips.erase(ips.length() - 1);
    e->return_string(ips.c_str());
}
void start_network(webui::window::event *e)
{
    if (e->get_count() == 3) {
        std::string ip = e->get_string(0);
        long long port = e->get_int(1);
        std::string type = e->get_string(2);
        std::cout << ip << " " << port << " " << type << std::endl;
    }
}
int main()
{
    webui::window win;
    win.show("index.html");
    win.bind("getIps", get_ips);
    win.bind("startNetwork", start_network);
    webui::wait();
    return 0;
}