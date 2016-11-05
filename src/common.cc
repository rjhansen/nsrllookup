/* Copyright (c) 2012-16, Robert J. Hansen <rob@hansen.engineering>
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
        rv.at(idx).erase(remove(rv.at(idx).begin(),
                             rv.at(idx).end(),
                             '\r'),
            rv.at(idx).end());
        rv.at(idx).erase(remove(rv.at(idx).begin(),
                             rv.at(idx).end(),
                             '\n'),
            rv.at(idx).end());
    }
    rv.erase(remove(rv.begin(), rv.end(), ""), rv.end());
    return rv;
}

/* This abomination comes to you courtesy of the Win32 API. */
void bomb(int code)
{
#ifdef WINDOWS
    WSACleanup();
    ExitProcess(code);
#else
    exit(code);
#endif
}
