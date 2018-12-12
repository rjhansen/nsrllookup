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
#include <array>
#include <cctype>
#include <regex>
#include <set>

using std::array;
using std::set;
using std::vector;
using std::string;
using std::find;
using std::fill;
using std::remove;
using std::ofstream;

using std::cin;
using std::unique_ptr;
using std::regex_search;
using std::regex;
using std::transform;

string SERVER;
bool SCORE_HITS{ false };
uint16_t PORT{ 9120 };

int main(int argc, char* argv[])
{
#ifdef WINDOWS
    WSAData wsad;
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsad)) {
        std::cerr << "Error: could not initialize Winsock.\n\n"
                     "You're running a very old version of Windows.  nsrllookup "
                     "won't work\n"
                     "on this system.\n";
        bomb(-1);
    }
#endif
    SERVER = "nsrllookup.com";
    set<string> hashes;
    regex valid_line{ "^[A-F0-9]{32}",
        std::regex_constants::icase | std::regex_constants::optimize };

    parse_options(argc, argv);

    string line;
    while (cin) {
        getline(cin, line);
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        if (regex_search(line, valid_line)) {
            hashes.insert(string(line.begin(), line.begin() + 32));
        }
    }

    query_server(hashes.cbegin(), hashes.cend());

    end_connection();

    return EXIT_SUCCESS;
}
