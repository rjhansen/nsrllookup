/* $Id: parse_options.cc 104 2012-01-30 08:07:11Z rjh $
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
using std::ofstream;

extern string SERVER;
extern bool SCORE_HITS;
extern bool REPORT_STATUS;
extern int PORT;
extern ofstream* HIT_FILE;
extern ofstream* MISSES_FILE;

namespace {
string PORTSTRING("9120");

void show_version()
{
    std::cerr <<
              "nsrllookup for " OPERATING_SYSTEM " version " PACKAGE_VERSION "\n\n"
              "The latest version can be found at:\n"
      "        http://rjhansen.github.com/nsrllookup\n\n";
    bomb(0);
}

void show_help()
{
    std::cerr <<
              "nsrllookup for " OPERATING_SYSTEM " version " PACKAGE_VERSION "\n"
              "\n"
              "nsrllookup [-hvukS] [-U FILE] [-K FILE] [-s SERVER] [-p PORT]\n"
              "\n"
              "        -h: display this help message\n"
              "        -v: display version information\n"
              "        -u: show only unknown hashes (default)\n"
              "        -k: show only known hashes\n"
              "        -S: display server status (if possible)\n"
              "        -U FILE: write unknown hashes to FILE\n"
              "        -K FILE: write known hashes to FILE\n"
              "        -s SERVER: connect to a specified nsrlquery server\n"
              "        -p PORT: connect on a specified port\n"
              "\n"
#ifndef WINDOWS
              "Alternately, you may want to read the manpage: 'man nsrllookup'\n"
              "may have useful information.\n\n"
#endif
              "The latest version can be found at:\n"
              "        http://rjhansen.github.com/nsrllookup\n\n";
    bomb(0);
}

void validate_port()
{
    PORT = 0;

    for (string::const_iterator iter = PORTSTRING.begin() ;
            iter != PORTSTRING.end() ; ++iter) {
        if (*iter < '0' || *iter > '9') {
            std::cerr << "invalid port: must be in range 0 - 65535";
            bomb(0);
        }
        PORT = (PORT * 10) + (*iter - '0');
    }
    if (PORT <= 0 || PORT > 65535) {
        std::cerr << "invalid port: must be in range 0 - 65535";
        bomb(0);
    }
}
}

void parse_options(int argc, char** argv)
{
    int kucount = 0;

    for (int i = 1 ; i < argc ; ++i) {
        string option(argv[i]);
        if (option.size() < 2 || '-' != option[0] || "-h" == option) {
            show_help();
        }
        else if ("-S" == option) {
            REPORT_STATUS = true;
        }
        else if ("-k" == option) {
            SCORE_HITS = true;
            kucount |= 1;
        }
        else if ("-u" == option) {
            SCORE_HITS = false;
            kucount |= 2;
        }
        else if ("-K" == option) {
            if (argc - 1 == i) {
                show_help();
            }
            HIT_FILE = new ofstream(argv[i+1]);
            i += 1;
        }
        else if ("-U" == option) {
            if (argc - 1 == i) {
                show_help();
            }
            MISSES_FILE = new ofstream(argv[i+1]);
            i += 1;
        }
        else if ("-s" == option) {
            if (argc - 1 == i) {
                show_help();
            }
            SERVER = string(argv[i+1]);
            i += 1;
        }
        else if ("-p" == option) {
            if (argc - 1 == i) {
                show_help();
            }
            PORTSTRING = string(argv[i+1]);
            i += 1;
        }
        else if ("-v" == option) {
            show_version();
        }
        else {
            show_help();
        }
    }
    if (3 == kucount) {
        std::cerr <<
                  "Error: the -k and -u flags cannot be used simultaneously (it would be\n"
                  "the same as printing the input file to standard output).  If you need\n"
                  "both known and unknown hashes parsed out in a single pass, use the\n"
                  "-K and -U flags together to print them into separate files.\n"
                  "\n"
                  "Run this program again with the '-h' flag to see usage options.\n";
        bomb(0);
    }

    validate_port();

}
