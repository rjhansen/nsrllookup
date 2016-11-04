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

using std::string;
using std::vector;
using std::unique_ptr;
using std::ofstream;

namespace {
NetworkSocket sockobj;

class BadHandshake : public std::exception {
public:
    const char* what() const noexcept { return "bad handshake"; }
};

class BadQuery : public std::exception {
public:
    const char* what() const noexcept { return "bad query"; }
};

class MismatchedResultSet : public std::exception {
public:
    const char* what() const noexcept { return "mismatched result set"; }
};

void write_output(const vector<string>& buffer, const string& result_line)
{
    auto tokens{ tokenize(result_line) };
    if (tokens.empty())
        return;

    if (tokens.at(0) != "OK") {
        throw BadQuery();
    }
    const string& results = tokens.at(1);

    if (buffer.size() != results.size())
        throw MismatchedResultSet();

    for (size_t idx = 0; idx < buffer.size(); ++idx) {
        bool hit = results.at(idx) == '1' ? true : false;

        if ((hit && SCORE_HITS) || (!hit && !SCORE_HITS)) {
            std::cout << buffer.at(idx) << "\n";
        }
    }
}
}

void end_connection()
{
    try {
        if (sockobj.isConnected()) {
            sockobj.write("BYE\r\n");
        }
    } catch (std::exception&) {
        // pass: we're closing the connection anyway
    }
}

void query_server(const vector<string>& buffer)
{
    if (buffer.empty())
        return;

    if (!sockobj.isConnected()) {
        try {
            sockobj.connect(SERVER, PORT);
            sockobj.write("Version: 2.0\r\n");
            auto tokens{ tokenize(sockobj.read_line()) };
            if (tokens.size() == 0 || tokens.at(0) != "OK")
                throw BadHandshake();
        } catch (ConnectionRefused&) {
            std::cerr << "Error: connection refused\n";
            bomb(-1);
        } catch (BadHandshake&) {
            std::cerr << "Error: server handshake failed\n";
            bomb(-1);
        }
    }

    try {
        string q{ "query" };
        for (size_t idx = 0; idx < buffer.size(); ++idx) {
            auto tokens{ tokenize(buffer.at(idx)) };
            q += " " + tokens.at(0);
        }
        q += "\r\n";
        sockobj.write(q);
        write_output(buffer, sockobj.read_line());
    } catch (BadQuery&) {
        std::cerr << "Error: server didn't like our query.\n";
        bomb(-1);
    } catch (NetworkError&) {
        std::cerr << "Error: network failure.\n";
        bomb(-1);
    } catch (MismatchedResultSet&) {
        std::cerr << "Error: mismatched result set.\n";
        bomb(-1);
    } catch (std::exception&) {
        std::cerr << "Error: unknown error (WTF?)\n";
        bomb(-1);
    }
}
