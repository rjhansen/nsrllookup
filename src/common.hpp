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

#ifndef COMMON_HPP
#define COMMON_HPP

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class NetworkError : public std::exception {
public:
    const char* what() const noexcept { return "network error"; }
};
class EOFException : public std::exception {
public:
    const char* what() const noexcept { return "eof exception"; }
};
class ConnectionRefused : public std::exception {
public:
    const char* what() const noexcept { return "connection refused"; }
};

void bomb(int code);
void parse_options(int argc, char** argv);
std::vector<std::string> tokenize(const std::string& line,
    const char delim = ' ');
void query_server(const std::vector<std::string>& buffer);
void end_connection();

extern std::string SERVER;
extern bool SCORE_HITS;
extern uint16_t PORT;

#ifdef _WIN32
#include "win32.hpp"
#else
#include "unix.hpp"
#endif

#endif
