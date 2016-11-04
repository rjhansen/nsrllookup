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
using std::unique_ptr;
using std::ofstream;

extern string SERVER;
extern bool SCORE_HITS;
extern int PORT;


typedef vector<string> VS;

namespace {
NetworkSocket sockobj;

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
    unique_ptr<VS> tokens(tokenize(result_line));
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

        if ((hit and SCORE_HITS) or (not hit and not SCORE_HITS)) {
            std::cout << buffer.at(idx) << "\n";
        }
    }
}

string make_query(const vector<string>& buffer)
{
    string rv = "query";

    if (buffer.size() == 0) {
        throw BadQuery();
    }

    for (size_t idx = 0 ; idx < buffer.size() ; ++idx) {
        unique_ptr<VS> tokens(tokenize(buffer.at(idx)));
        rv += " " + tokens->at(0);
    }

    rv += "\r\n";

    return rv;
}


void query_handler(const vector<string>& buffer)
{
    if (buffer.empty() || not sockobj.isConnected())
        return;

    try {
        sockobj.write(make_query(buffer));
        string line = sockobj.read_line();
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
        if (sockobj.isConnected()) {
            sockobj.write("BYE\r\n");
        }
    }
    catch (std::exception&) {
        // pass: we're closing the connection anyway
    }
}

void query_server(const vector<string>& buffer)
{    
    try {
        if (not sockobj.isConnected()) {
            sockobj.connect(SERVER, PORT);
            sockobj.write("Version: 2.0\r\n");
            unique_ptr<VS> tokens(tokenize(sockobj.read_line()));
            if (tokens->size() == 0 || tokens->at(0) != "OK") {
                throw BadHandshake();
            }
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

