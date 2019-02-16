# nsrllookup

The latest release is [1.4.1](https://github.com/rjhansen/nsrllookup/archive/1.4.1.tar.gz).  

## Windows binaries

A prebuilt, Authenticode-signed [Win64 binary](https://github.com/rjhansen/nsrllookup/releases/download/1.4.0-1/nsrllookup-1.4-win64.zip) is available.  (You may need the [MSVC 2015 runtime](https://www.microsoft.com/en-us/download/details.aspx?id=52685) if you don't already have it installed.)

## News

1.4.1 was released in February 2019.  Users should notice no differences.  Under the hood the code has been tweaked to work better with older versions of `Boost.Asio`, such as those found in Ubuntu 18.04.

1.4 was released in December 2018.  Users should notice few differences, but under the hood the network stack has been replaced with `Boost.Asio` for better performance and reliability.

1.3 was released on November 5, 2016.  Users shouldn't notice any major differences, but under the hood the codebase has been overhauled for C++14 conformance and the build system has been replaced with a new CMake-based one.

## What's nsrllookup?
NIST publishes a giant compendium of MD5 hashes of known pieces of software.  This volume, the National Software Reference Library Reference Data Set (NSRL RDS), is invaluable for computer forensics.  Often when faced with a needle-in-a-haystack problem, it's a tremendous help to be able to immediately categorize data into "NIST knows about this" and "it's unknown to NIST."

Unfortunately, the NSRL RDS is a couple of gigs in size and doesn't have any good querying tools.  This is where nsrlsvr and nsrllookup come into play -- or collectively, "the nsrlquery tools."

### nsrlsvr
[nsrlsvr](https://rjhansen.github.io/nsrlsvr) lets a system administrator stand up a server to support NSRL queries.  On top of that, an admin can add additional hashes to the NSRL RDS list.  For instance, if you have a set of images you're particularly interested in finding, you can compute hashes of the images and add them to the dataset.  Now you can query nsrlsvr to find out "is it known to either NIST or our own local corpus?"

### nsrllookup
nsrllookup is how end-users interact with nsrlsvr.  It's designed to be a well-behaved command-line application, and works hand-in-glove with tools like [md5deep](http://md5deep.sourceforge.net).  For instance, to hash everything in `/mount/evil-data` and compare it against the NSRL RDS using the publicly-accessible nsrlsvr instance at nsrllookup.com, printing out those things that are unknown to NIST:

`md5deep -r /mount/evil-data | nsrllookup`

### How do I get help?

`nsrllookup --help` or `man nsrllookup`.

I hope you like nsrllookup.  If you have bugs, suggestions or praise, please feel free to [email me](mailto:rjh@sixdemonbag.org?subject=nsrllookup%201.4.0-1).
