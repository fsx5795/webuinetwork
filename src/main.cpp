#if defined (WIN32) || defined (__WIN32)
#include <WinSock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <iostream>
#include <sstream>
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
        if (type.compare("tcp") == 0) {
            int sd = socket(AF_INET, SOCK_STREAM, 0);
            if (sd < 0) {
                perror("socket()");
                return;
            }
            struct sockaddr_in saddr, caddr;
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &saddr.sin_addr);
            if (bind(sd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof saddr) < 0) {
                perror("bind");
                return;
            }
            if (listen(sd, 10) < 0) {
                perror("listen()");
                return;
            }
            socklen_t caddrLen = sizeof saddr;
            while (true) {
                int csd = accept(sd, reinterpret_cast<struct sockaddr*>(&caddr), &caddrLen);
                if (csd < 0) {
                    perror("accept()");
                    return;
                }
                int val{1};
                if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val) < 0) {
                    perror("setsocket()");
                    return;
                }
                char ipstr[30];
                inet_ntop(AF_INET, &caddr.sin_addr, ipstr, 30);
                char buf[BUFSIZ] = { '\0' };
                recv(csd, buf, BUFSIZ, 0);
                buf[strlen(buf) - 1] = '\'';
                buf[strlen(buf)] = ')';
                buf[strlen(buf)] = ';';
                std::stringstream ss;
                ss << "displayMessage('" << ipstr << ":" << ntohs(caddr.sin_port) << " " << buf;
                e->get_window().run(ss.str());
                close(csd);
            }
        }
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
