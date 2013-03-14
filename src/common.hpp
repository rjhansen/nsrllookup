/* $Id: common.hpp 105 2012-01-30 08:08:50Z rjh $
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

#ifndef __COMMON_HPP
#define __COMMON_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <exception>

void bomb(int code);
int query_server_status();

class NetworkError : public std::exception {
public:
    const char* what() const throw() {
        return "network error";
    }
};
class EOFException : public std::exception {
public:
    const char* what() const throw() {
        return "eof exception";
    }
};
class ConnectionRefused : public std::exception {
public:
    const char* what() const throw() {
        return "connection refused";
    }
};

void parse_options(int argc, char** argv);
std::vector<std::string>* tokenize(const std::string& line,
                                   const char delim = ' ');
void query_server(const std::vector<std::string>& buffer);
void end_connection();


#ifdef _WIN32
#include "win32.hpp"
#else
#include "unix.hpp"
#endif

#endif
