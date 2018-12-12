/* Copyright (c) 2012-18, Robert J. Hansen <rob@hansen.engineering>
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
#include <boost/asio.hpp>
#include <iostream>
#include <regex>
#include <set>

using boost::asio::ip::tcp;
using std::cerr;
using std::regex;
using std::regex_search;
using std::set;
using std::string;
using std::vector;

namespace {
auto resprx = regex("^OK [01]+$");
boost::asio::io_context iocontext;
tcp::resolver resolver { iocontext };
tcp::socket sock { iocontext };

std::string readsock()
{
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, "\n");
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    data.erase(--data.end());
    if (*(data.end() - 1) == '\r') {
        data.erase(--data.end());
    }
    return data;
}

void sendsock(const std::string& str)
{
    const std::string msg = str + "\r\n";
    boost::asio::write(sock, boost::asio::buffer(msg));
}
}

set<string> query_server(const vector<string>& buffer)
{
    constexpr size_t MAX_SENT = 512;
    set<string> rv;
    if (buffer.empty())
        return rv;
    try {
        boost::asio::connect(sock, resolver.resolve(SERVER, PORT));
    } catch (boost::system::system_error& e) {
        cerr << "Could not connect to " << SERVER << " " << PORT << ".\n";
        bomb(-1);
    }

    try {
        sendsock("Version: 2.0");
        auto resp = readsock();
        if (resp != "OK") {
            cerr << "0: " << resp << "\n";
            bomb(-1);
        }

        vector<string> queries(MAX_SENT);

        for (auto iter = buffer.cbegin(); iter != buffer.cend();) {
            string q { "query" };
            queries.clear();
            while (iter != buffer.cend() && queries.size() < MAX_SENT) {
                q += " " + *iter;
                queries.emplace_back(*iter);
                ++iter;
            }

            sendsock(q);
            resp = readsock();

            if (!regex_search(resp, resprx) || resp.size() - 3 != queries.size()) {
                cerr << "Error: malformed response from server";
                bomb(-1);
            }

            for (size_t idx = 3; idx < resp.size(); idx += 1) {
                auto scorechar = SCORE_HITS ? '1' : '0';
                if (resp.at(idx) == scorechar) {
                    rv.insert(queries.at(idx - 3));
                }
            }
        }

        sock.close();
        return rv;
    } catch (boost::system::system_error& e) {
        cerr << "IO error communicating with " << SERVER << " " << PORT << ".\n";
        bomb(-1);
    }
    return set<string>();
}
