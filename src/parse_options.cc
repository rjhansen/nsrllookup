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
#include <boost/program_options.hpp>
#include <iostream>
#include <regex>

using boost::program_options::error;
using boost::program_options::notify;
using boost::program_options::options_description;
using boost::program_options::parse_command_line;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using std::array;
using std::cerr;
using std::cout;
using std::fill;
using std::string;
using std::regex;
using std::regex_search;

void parse_options(int argc, char** argv)
{
    const regex valid_port{ "^[0-9]{1,5}$" };
    options_description options { "nsrllookup options" };
	
    options.add_options()("help,h", "Show this help screen")(
        "version,v", "Show version information")(
        "bug-reports,b", "Show bug reporting information")(
        "known,k", "Show only RDS hits")("unknown,u",
        "Show only RDS misses (default)")(
        "server,s", value<string>()->default_value("nsrllookup.com"),
        "nsrlsvr instance to use")(
        "port,p", value<string>()->default_value("9120"), "port to connect on");
    variables_map vm;

    try {
        store(parse_command_line(argc, argv, options), vm);
        notify(vm);
    } catch (error& e) {
        cout << "Error: " << e.what() << "\n";
        bomb(-1);
    }

    if (vm.count("help")) {
        cout << options << "\n";
        bomb(EXIT_SUCCESS);
    }
    if (vm.count("version")) {
        cout << "nsrllookup 1.4.2\n";
        bomb(EXIT_SUCCESS);
    }
    if (vm.count("bug-reports")) {
        cout << "To file a bug report, visit https://github.io/rjhansen/nsrllookup"
             << " (or email rob@hansen.engineering)\n";
        bomb(EXIT_SUCCESS);
    }
    if (vm.count("known") && vm.count("unknown")) {
        cout << "Error: the known and unknown flags are mutually exclusive.\n";
        bomb(-1);
    }
	
    SCORE_HITS = static_cast<bool>(vm.count("known"));
    SERVER = vm["server"].as<string>();
    PORT = vm["port"].as<string>();

    if (!regex_search(PORT, valid_port)) {
        cerr << "Error: '" << PORT << "' is not a valid port.\n";
        bomb(-1);
    }

    const auto port = ::strtol(PORT.c_str(), nullptr, 10);
    if (port < 0 || port > 65535) {
        cerr << "Error: '" << PORT << "' is not a valid port.\n";
        bomb(-1);
    }
}
