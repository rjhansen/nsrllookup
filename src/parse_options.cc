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
#include <array>
#include <boost/program_options.hpp>
#include <string>

using std::string;
using std::ofstream;
using std::cout;
using std::cerr;
using std::array;
using std::fill;
using boost::program_options::options_description;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::parse_command_line;
using boost::program_options::notify;
using boost::program_options::value;
using boost::program_options::error;

void parse_options(int argc, char** argv)
{
#ifdef WINDOWS
    array<char, 260> filename_buffer;
#else
    array<char, PATH_MAX> filename_buffer;
#endif
    fill(filename_buffer.begin(), filename_buffer.end(), 0);

    options_description options{ "nsrllookup options" };
    options.add_options()("help,h", "Show this help screen")(
        "version,v", "Show version information")(
        "bug-reports,b", "Show bug reporting information")(
        "known,k", "Show only RDS hits")("unknown,u",
        "Show only RDS misses (default)")(
        "server,s", value<string>()->default_value("nsrllookup.com"),
        "nsrlsvr instance to use")(
        "port,p", value<uint16_t>()->default_value(9120), "port to connect on");
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
        cout << "nsrllookup " << PACKAGE_VERSION << "\n";
        bomb(EXIT_SUCCESS);
    }
    if (vm.count("bug-reports")) {
        cout << "To file a bug report, visit "
                "https://github.com/rjhansen/nsrllookup/issues\n";
        bomb(EXIT_SUCCESS);
    }
    if (vm.count("known") && vm.count("unknown")) {
        cout << "Error: the known and unknown flags are mutually exclusive.\n";
        bomb(-1);
    }
    SCORE_HITS = vm.count("known") ? true : false;
    SERVER = vm["server"].as<string>();
    PORT = vm["port"].as<uint16_t>();
}
