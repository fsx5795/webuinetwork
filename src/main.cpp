#if defined (WIN32) || defined (_WIN32)
#include <WinSock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <threads.h>
#include <iostream>
#include <sstream>
#include <webui.hpp>
static void do_work(std::tuple<int, const char*, struct sockaddr_in, webui::window::event*>*);
void get_ips(webui::window::event *e)
{
    char buf[100] = { '\0' };
    std::string ips;
    if (gethostname(buf, sizeof(buf)) == 0) {
        /*
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
        */
        struct addrinfo hints, *res;
        memset(&hints, '\0', sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        int ret = getaddrinfo(buf, nullptr, &hints, &res);
        if (ret != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
            return;
        }
        char host[256];
        for (struct addrinfo *tmp = res; tmp != nullptr; tmp = tmp->ai_next) {
            getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof host, nullptr, 0, NI_NUMERICHOST);
            if (strcmp("127.0.0.1", host) == 0)
                continue;
            ips.append(host);
            ips.push_back(',');
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
            #if defined (WIN32) || defined (_WIN32)
                if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&val), sizeof val) < 0) {
            #else
                if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val) < 0) {
            #endif
                    perror("setsocket()");
                    return;
                }
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &caddr.sin_addr, ipstr, INET_ADDRSTRLEN);
                thrd_t thr;
                std::tuple<int, const char*, struct sockaddr_in, webui::window::event*> para{csd, ipstr, caddr, e};
                thrd_create(&thr, (thrd_start_t)do_work, static_cast<void*>(&para));
                thrd_detach(thr);
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
static void do_work(std::tuple<int, const char*, struct sockaddr_in, webui::window::event*> *para)
{
    char buf[BUFSIZ];
    while (true) {
        memset(buf, '\0', BUFSIZ);
        if (recv(std::get<0>(*para), buf, BUFSIZ, 0) == 0)
            break;
        buf[strlen(buf) - 1] = '\'';
        buf[strlen(buf)] = ')';
        buf[strlen(buf)] = ';';
        std::stringstream ss;
        ss << "displayMessage('" << std::get<1>(*para) << ":" << ntohs(std::get<2>(*para).sin_port) << " " << buf;
        std::get<3>(*para)->get_window().run(ss.str());
    }
    //close(std::get<0>(*para));
    _close(std::get<0>(*para));
 }
