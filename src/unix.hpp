/* Copyright (c) 2012-2016, Robert J. Hansen <rob@hansen.engineering>
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

#ifndef UNIX_HPP
#define UNIX_HPP

#include <netdb.h>
#include <poll.h>
#include <sys/errno.h>
#include <unistd.h>

#ifdef __APPLE__
#endif

#ifdef __linux__
#include <cstring>
#include <sys/socket.h>
#endif

#ifdef __FreeBSD__
#include <netinet/in.h>
#include <sys/socket.h>
#endif

class NetworkSocket {
public:
    NetworkSocket()
        : sock{ -1 }
        , buffer{ "" }
    {
    }

    void connect(std::string host, uint16_t port)
    {
        struct sockaddr_in serv_addr;
        struct hostent* server = gethostbyname(host.c_str());

        if (nullptr == server || 0 > (sock = socket(AF_INET, SOCK_STREAM, 0))) {
            perror("");
            throw NetworkError();
        }

        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        memcpy(static_cast<void*>(&serv_addr.sin_addr.s_addr),
            static_cast<void*>(server->h_addr),
            static_cast<size_t>(server->h_length));

        if (::connect(sock, reinterpret_cast<sockaddr*>(&serv_addr),
                sizeof(serv_addr))
            < 0) {
            if (ECONNREFUSED == errno) {
                throw ConnectionRefused();
            } else {
                throw NetworkError();
            }
        }
    }

    bool isConnected() const { return (sock > 0); }

    void disconnect()
    {
        if (sock > 0)
            close(sock);
        sock = -1;
    }

    ~NetworkSocket() { disconnect(); }

    void write(const std::string& line)
    {
        // This has a wacky edge case if you're sending 2**32 bytes AND
        // have a network error.  Yes, it's a bug.
        if (line.size() != static_cast<size_t>(send(sock,
                               line.c_str(),
                               line.size(),
                               0)))
            throw NetworkError();
    }

    void write(const char* buf)
    {
        std::string line(buf);
        write(line);
    }

    std::string read_line()
    {
        pollfd fds = { sock, POLLIN, 0 };
        int poll_code{ poll(&fds, 1, 750) };
        std::string rv{ "" };
        auto iter{ std::find(buffer.begin(), buffer.end(), '\n') };

        while (0 == poll_code)
            poll_code = poll(&fds, 1, 750);
        if (-1 == poll_code)
            throw NetworkError();
        else if (fds.revents & POLLERR || fds.revents & POLLHUP)
            throw NetworkError();
        else if (fds.revents & POLLIN) {
            if (sock == 0 and buffer == "") {
                throw EOFException();
            }
            while (sock != 0 and iter == buffer.end()) {
                std::vector<char> tmpbuf(8192, '\0');
                ssize_t status = 0;

                if (0 == (status = recv(sock,
                              static_cast<void*>(&tmpbuf[0]),
                              tmpbuf.size() - 1,
                              0))) {
                    close(sock);
                    sock = 0;
                } else if (-1 == status) {
                    throw NetworkError();
                }
                buffer += std::string(&tmpbuf[0]);
                iter = std::find(buffer.begin(), buffer.end(), '\n');
            }

            rv = std::string(buffer.begin(), iter);
            rv.erase(std::remove(rv.begin(), rv.end(), '\r'), rv.end());

            if (iter != buffer.end())
                iter += 1;
            buffer = std::string(iter, buffer.end());
        }
        return rv;
    }

private:
    int sock;
    std::string buffer;
};

#endif
