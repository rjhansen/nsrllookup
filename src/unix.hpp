/* $Id: unix.hpp 123 2012-02-08 16:43:07Z rjh $
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


#ifndef __UNIX_HPP
#define __UNIX_HPP

#include <sys/errno.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>

#ifdef __APPLE__
#define OPERATING_SYSTEM "Mac OS X"
#endif

#ifdef __linux__
#include <cstring>
#include <sys/socket.h>
#define OPERATING_SYSTEM "Linux"
#endif

#ifdef __FreeBSD__
#define OPERATING_SYSTEM "FreeBSD (cool!)"
#include <sys/socket.h>
#include <netinet/in.h>
#endif

class NetworkSocket
{
public:
    NetworkSocket(std::string host, unsigned short int port) :
        sock(-1), buffer("")
    {
        struct sockaddr_in serv_addr;
        struct hostent *server = gethostbyname(host.c_str());

        if (0 == server ||
                0 > (sock = socket(AF_INET, SOCK_STREAM, 0))) {
            perror("");
            throw NetworkError();
        }

        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        memcpy((void*) &serv_addr.sin_addr.s_addr,
               (void*) server->h_addr,
               server->h_length);

        if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            if (ECONNREFUSED == errno) {
                throw ConnectionRefused();
            } else {
                throw NetworkError();
            }
        }
    }

    virtual ~NetworkSocket()
    {
        if (sock > 0)
            shutdown(sock, SHUT_RDWR);
    }

    void write(const std::string& line)
    {
        // This has a wacky edge case if you're sending 2**32 bytes AND
        // have a network error.  Yes, it's a bug.
        if (line.size() != (size_t) send(sock, line.c_str(), line.size(), 0))
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
        int poll_code(poll(&fds, 1, 750));
        std::string rv("");
        std::string::iterator iter(std::find(buffer.begin(),
            buffer.end(), '\n'));

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
                int status = 0;

                status = recv(sock, (void*) &tmpbuf[0], 8191, 0);
                if (0 == status) {
                    close(sock);
                    sock = 0;
                }
                else if (-1 == status) {
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
