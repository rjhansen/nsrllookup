/* $Id: nsrllookup.cc 123 2012-02-08 16:43:07Z rjh $
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

using std::vector;
using std::string;
using std::find;
using std::remove;
using std::ofstream;
using std::cin;
using std::unique_ptr;

string SERVER("nsrllookup.com");
bool SCORE_HITS(false); // score misses
bool REPORT_STATUS(false); // report server status
int PORT(9120);
ofstream* HIT_FILE(0);
ofstream* MISSES_FILE(0);
NetworkSocket* GLOBAL_SOCK(0);

namespace {
bool is_valid(const string& line)
{
    unique_ptr<vector<string> > tokens(tokenize(line));
    size_t token_count(tokens->size());

    if (token_count < 1)
        return false;
    
    string& hash(tokens->at(0));
    size_t hash_length(hash.size());

    if (hash_length != 32 && hash_length != 40 && hash_length != 64)
        return false;

    string::iterator siter(hash.begin());

    for (siter = hash.begin() ; siter != hash.end() ; ++siter)
        if (! ((*siter >= '0' && *siter <= '9') ||
            (*siter >= 'A' && *siter <= 'F') ||
            (*siter >= 'a' && *siter <= 'f')))
            return false;

    return true;
}
}


int main(int argc, char* argv[])
{
#ifdef WINDOWS
    WSAData wsad;
    if (0 != WSAStartup(MAKEWORD(2, 0), &wsad)) {
        std::cerr <<
                  "Error: could not initialize Winsock.\n\n"
                  "You're running a very old version of Windows.  nsrllookup won't work\n"
                  "on this system.\n";
        bomb(-1);
    }
#endif

    vector<string> buffer;

    parse_options(argc, argv);

    if (REPORT_STATUS) {
        int rv = query_server_status();
        end_connection();
        bomb(rv);
    }
    
    try {
        vector<char> buf(4096, '\0');
        while (cin) {
#ifdef WINDOWS
            ZeroMemory(&buf[0], 4096);
#else
            memset(&buf[0], 0, 4096);
#endif
            cin.getline(&buf[0], 4096);
            buf.erase(remove(buf.begin(), buf.end(), '\r'), buf.end());
            buf.erase(remove(buf.begin(), buf.end(), '\n'), buf.end());

            string line(&buf[0]);

            if (is_valid(line)) {
                buffer.push_back(line);
                if (buffer.size() >= 4096) {
                    query_server(buffer);
                    buffer.clear();
                }
            }
        }
    } catch (EOFException&) {
        // pass: this is entirely expected.  Uh, well, maybe.  It should
        // actually be removed, I think...
    }

    if (REPORT_STATUS || 0 < buffer.size()) {
        query_server(buffer);
        buffer.clear();
    }
    end_connection();
    
    bomb(0);
    return 0;
}
