/* $Id: common.cc 104 2012-01-30 08:07:11Z rjh $
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

#include "common.hpp"
#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::find;
using std::remove;
using std::ofstream;
using std::unique_ptr;
using std::make_unique;

vector<string> tokenize(const string& line, const char delim)
{
    auto begin{ line.begin() };
    auto end{ line.begin() };
    vector<string> rv;

    while (begin != line.end()) {
        end = find(begin + 1, line.end(), delim);
        rv.emplace_back(string(begin, end));
        begin = end + (end == line.end() ? 0 : 1);
    }
    for (size_t idx = 0; idx < rv.size(); ++idx) {
        string& line = rv.at(idx);
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());
    }
    rv.erase(remove(rv.begin(), rv.end(), ""), rv.end());
    return rv;
}

/* This abomination comes to you courtesy of the Win32 API. */
void bomb(uint16_t code)
{
#ifdef WINDOWS
    WSACleanup();
    ExitProcess(code);
#else
    exit(code);
#endif
}
