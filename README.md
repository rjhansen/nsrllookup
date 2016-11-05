# nsrllookup

## Licensing
ISC.  Share and enjoy.

## What is it?
It's a command-line tool that allows you to quickly and efficiently triage
files by MD5 hashes.

### Why would I want to do that?
Digital forensics has a big problem with needles and haystacks.  nsrllookup
is a tool that can be used to significantly cut down the size of the haystack,
making it easier to search for needles.

## How does it work?
The National Institute of Standards and Technology (NIST) maintains the
National Software Reference Library (NSRL).  The NSRL is a library of every
major piece of software released in the world dating back more than twenty
years.

NIST also publishes MD5 hashes of every file in the NSRL.  This is called the
Reference Data Set (RDS).

When looking at an unknown file, a good place to begin is to compute its MD5
hash and compare it against the RDS.  If the hash value is found in the RDS,
your file is probably boring.  (At the very least, it's commonplace enough to
have an RDS entry.)  This means your file is probably hay and not a needle.

### Can you give me an example?

Imagine you're using [md5deep](https://github.com/jessek/hashdeep/) to
compute the hashes of a large collection of files.  To get a list of files
that do _not_ match the RDS, you could do the following:

```
md5deep -r /path/to/collection > all_hashes.txt
nsrllookup < all_hashes.txt > rds_misses.txt
```

This would produce two files: `all_hashes.txt` containing the names of
all files and their hashes, and `rds_misses.txt` containing the hash
values of those files which did not appear in the NSRL RDS.

## Building

### Before you begin
You'll need:

1. [cmake](http://www.cmake.org) 3.5 or later
2. A _good_ C++14 compiler.  GCC 5.0 or later, and/or Clang 3.5 and later, should do well.
3. [boost](http://www.boost.org) 1.60 or later

### UNIX and OS X

```
cmake -D CMAKE_BUILD_TYPE=Release .
make
sudo make install
```

is all you should need to do.

### Windows

If you're compiling from a Cygwin or MSYS2 environment, see the UNIX
instructions above.  If you're doing a native compile, run `cmake-gui.exe`
to configure CMake parameters.  In the resulting solution file you'll probably
have to manually set Boost's include and lib dirs, but it's not hard to get
it all set up.

### Cross-compiling

Fedora 24 with the `mingw64` packages installed can cross-compile for
Windows.

```
cmake -D CMAKE_TOOLCHAIN_FILE=mingw64.cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_EXE_LINKER_FLAGS_RELEASE="-static -static-libgcc -static-libstdc++" .
make
mingw-strip src/nsrllookup.exe
```

The resulting executable has static linkage against everything but ws2_32, kernel32, and msvcrt.  (Since those are all standard parts of every Windows install, there's really no point in statically linking them.)
