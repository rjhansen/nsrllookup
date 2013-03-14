/* $Id: win32.hpp 105 2012-01-30 08:08:50Z rjh $
 * Copyright (c) 2012, Robert J. Hansen <rjh@secret-alchemy.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#if _WIN32 || __WIN32__ || __WINDOWS__ || _win32
#ifndef __WIN32_HPP
#define __WIN32_HPP
#define WINDOWS

#undef UNICODE

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#define OPERATING_SYSTEM "Windows"
#define PACKAGE_VERSION "1.2"

class NetworkSocket
{
public:
    NetworkSocket(std::string host, unsigned short int port)
        : sock(socket(AF_INET, SOCK_STREAM, 0)),
          buffer(""),
          temporary_pool(HeapAlloc(GetProcessHeap(),
                                   HEAP_ZERO_MEMORY,
                                   8192)),
          connected(false)
    {
        sockaddr_in server;
        sockaddr_in *sockaddr_ipv4 = 0;
        addrinfo* result, *ptr;
        addrinfo hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        if (INVALID_SOCKET == sock) {
            std::cerr << "Error: couldn't create a socket!\n";
            throw NetworkError();
        }

        if (0 != GetAddrInfo(host.c_str(), NULL, &hints, &result)) {
            std::cerr << "Error: DNS failure (couldn't look up server).\n";
            throw NetworkError();
        }


        for(ptr = result; ptr != NULL ; ptr = ptr->ai_next) {
            if (ptr->ai_family == AF_INET) {
                CopyMemory(&server, ptr->ai_addr, sizeof(server));
                break;
            }
        }
        if (NULL == ptr) {
            std::cerr << "Error: DNS failure (server not found).\n";
            throw NetworkError();
        }

        freeaddrinfo(result);

        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if (SOCKET_ERROR == connect(sock,
                                    (sockaddr*) &server,
                                    sizeof(server))) {
            throw ConnectionRefused();
        }
        connected = true;
    }

    virtual ~NetworkSocket()
    {
        HeapFree(GetProcessHeap(), 0, temporary_pool);
        closesocket(sock);
    }

    void write(const std::string& line)
    {
        if (SOCKET_ERROR == send(sock, line.c_str(), line.size(), 0)) {
            std::cerr << "Error: couldn't send a packet to the server.\n";
            bomb(0);
        }
    }

    void write(const char* buf)
    {
        std::string line(buf);
        write(line);
    }

    std::string read_line()
    {
        std::string::iterator siter = std::find(buffer.begin(),
                                                buffer.end(),
                                                '\n');
        std::string rv("");
        int bytes_read(0);
        fd_set fds;
        timeval tv;
        int sel_call(0);

        tv.tv_sec = 0;
        tv.tv_usec = 500000;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);

        if (! connected && buffer != "") {
            rv = buffer;
            rv.erase(std::remove(rv.begin(), rv.end(), '\n'), rv.end());
            rv.erase(std::remove(rv.begin(), rv.end(), '\r'), rv.end());
            buffer = "";
            return rv;
        }
        if (! connected && buffer == "") {
            throw EOFException();
        }

        while (connected && siter == buffer.end())
        {
            sel_call = select(0, &fds, NULL, &fds, &tv);
            if (0 == sel_call)
                continue;
            if (SOCKET_ERROR == sel_call) {
                connected = false;
                throw NetworkError();
            }

            ZeroMemory(temporary_pool, 8192);

            bytes_read = recv(sock, (char*) temporary_pool, 8192, 0);
            if (SOCKET_ERROR == bytes_read || 0 == bytes_read) {
                connected = false;
                break;
            }
            buffer += std::string((const char*) temporary_pool,
                                  (const char*) temporary_pool + bytes_read);
            siter = std::find(buffer.begin(), buffer.end(), '\n');
        }
        siter = std::find(buffer.begin(), buffer.end(), '\n');
        rv = std::string(buffer.begin(), siter);
        if (siter != buffer.end())
            siter += 1;
        buffer = std::string(siter, buffer.end());
        return rv;
    }

private:
    SOCKET sock;
    std::string buffer;
    void* temporary_pool;
    bool connected;
};

#endif
#endif
