/* $Id: query_server.cc 123 2012-02-08 16:43:07Z rjh $
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

using std::string;
using std::vector;
using std::auto_ptr;
using std::ofstream;

extern string SERVER;
extern bool SCORE_HITS;
extern int PORT;
extern ofstream* HIT_FILE;
extern ofstream* MISSES_FILE;
extern NetworkSocket* GLOBAL_SOCK;

typedef vector<string> VS;

namespace {

bool supports_v2(true);

class BadHandshake : public std::exception {
public:
    const char* what() const throw() {
        return "bad handshake";
    }
};

class BadQuery : public std::exception {
public:
    const char* what() const throw() {
        return "bad query";
    }
};

class MismatchedResultSet : public std::exception {
public:
    const char* what() const throw() {
        return "mismatched result set";
    }
};

void write_output(const vector<string>& buffer, const string& result_line)
{
    auto_ptr<VS> tokens(tokenize(result_line));
    if (tokens->empty())
        return;

    if (tokens->at(0) != "OK") {
        throw BadQuery();
    }
    const string& results = tokens->at(1);

    if (buffer.size() != results.size())
        throw MismatchedResultSet();

    for (size_t idx = 0 ; idx < buffer.size() ; ++idx) {
        bool hit = results.at(idx) == '1' ? true : false;

        if (0 == MISSES_FILE &&
                0 == HIT_FILE &&
                hit == SCORE_HITS) {
            std::cout << buffer.at(idx) << "\n";
        }
        if (hit && HIT_FILE) {
            (*HIT_FILE) << buffer.at(idx) << "\n";
        }
        if ((! hit) && MISSES_FILE) {
            (*MISSES_FILE) << buffer.at(idx) << "\n";
        }
    }
}

void handshake(NetworkSocket* sock, string version)
{
    version = "Version: " + version + "\r\n";
    sock->write(version.c_str());
    auto_ptr<VS> tokens(tokenize(sock->read_line()));
    if (tokens->size() == 0 || tokens->at(0) != "OK") {
        throw BadHandshake();
    }
}

string make_query(const vector<string>& buffer)
{
    string rv = "query";

    if (buffer.size() == 0) {
        throw BadQuery();
    }

    for (size_t idx = 0 ; idx < buffer.size() ; ++idx) {
        auto_ptr<VS> tokens(tokenize(buffer.at(idx)));
        rv += " " + tokens->at(0);
    }

    rv += "\r\n";

    return rv;
}


void query_handler(const vector<string>& buffer)
{
    if (buffer.empty() or 0 == GLOBAL_SOCK)
        return;

    try {
        GLOBAL_SOCK->write(make_query(buffer));
        string line = GLOBAL_SOCK->read_line();
        write_output(buffer, line);
    }
    catch (BadQuery&) {
        std::cerr << "Error: server didn't like our query.\n";
        bomb(-3);
    }
    catch (NetworkError&) {
        std::cerr << "Error: network failure.\n";
        bomb(-4);
    }
    catch (MismatchedResultSet&) {
        std::cerr << "Error: mismatched result set.\n";
        bomb(-5);
    }
    catch (std::exception&) {
        std::cerr << "Error: unknown error (WTF?)\n";
        bomb(-6);
    }
}
}

void end_connection()
{
    try {
        if (0 != GLOBAL_SOCK) {
            GLOBAL_SOCK->write("BYE\r\n");
        }
    }
    catch (std::exception&) {
        // pass: we're closing the connection anyway
    }
}

void query_server(const vector<string>& buffer)
{
    try {
        if (0 == GLOBAL_SOCK) {
            GLOBAL_SOCK = new NetworkSocket(SERVER, PORT);
            handshake(GLOBAL_SOCK, "2.0");
        }
        query_handler(buffer);
    } catch (ConnectionRefused&) {
        std::cerr << "Error: connection refused\n";
        bomb(-1);
    } catch (BadHandshake&) {
        std::cerr << "Error: server handshake failed\n";
        bomb(-2);
    }
}

int query_server_status()
{
    int rv(0);
    try {
        if (0 == GLOBAL_SOCK) {
            GLOBAL_SOCK = new NetworkSocket(SERVER, PORT);
            handshake(GLOBAL_SOCK, "2.0");
        }
        GLOBAL_SOCK->write("STATUS\r\n");
        auto_ptr<VS> tokens(tokenize(GLOBAL_SOCK->read_line()));

        if (tokens->size() and tokens->at(0) == "NOT") {
            std::cerr << "Server does not support status queries.\n";
            rv = -7;
        } else if (tokens->size() < 8) {
            std::cerr << "Server returned an incorrect status string\n";
            rv = -8;
        } else {
            long num_of_hashes(atol(tokens->at(1).c_str()));
            std::string bitlength("???");
            double load[3] = {0.0, 0.0, 0.0};

            if (tokens->at(2) == "MD5") {
                bitlength = "128";
            } else if (tokens->at(2) == "SHA-1") {
                bitlength = "160";
            } else if (tokens->at(2) == "SHA-256") {
                bitlength = "256";
            }

            load[0] = atof(tokens->at(5).c_str());
            load[1] = atof(tokens->at(6).c_str());
            load[2] = atof(tokens->at(7).c_str());

            std::streamsize prec = std::cout.precision();
            std::cout.precision(3);
            std::cout << "Server reports " << num_of_hashes << " " 
                << bitlength << "-bit hashes, load avgs "
                << load[0] << " " << load[1] << " " << load[2]
                << "\n";
            std::cout.precision(prec);
        }
    } catch (ConnectionRefused&) {
        std::cerr << "Error: connection refused\n";
        rv = -1;
    } catch (BadHandshake&) {
        std::cerr << "Error: server handshake failed\n";
        rv = -2;
    }
    return rv;
}

