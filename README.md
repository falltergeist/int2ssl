int2ssl [![Build Status](https://travis-ci.org/falltergeist/int2ssl.svg?branch=develop)](https://travis-ci.org/falltergeist/int2ssl) [![Version](https://img.shields.io/github/release/falltergeist/int2ssl.svg)](https://github.com/falltergeist/int2ssl/releases/latest) [![Discord](https://img.shields.io/discord/401990446747877376.svg)](https://discord.gg/jxs6WRq)
==========

Script decompiler for Fallout 1/2.

Originally implemented by **Anchorite** (2005-2009).

Expanded for full *sfall* support by *Nirran* and *phobos2077* (2014-2015).

Rewritten for multiple platform support by *alexeevdv* (2015).

Installation:
=============

Ubuntu:
-------
```bash
sudo add-apt-repository ppa:falltergeist/falltergeist
sudo apt-get update
sudo apt-get install int2ssl
```

Building from source:
=====================

Dependencies:
-------------

- CMake (>= 3.13)

Build:
------

```bash
mkdir build && cd build && cmake .. && make
```

If you want cross-compile on Linux for Windows, you'll need to install MinGW
toolchain and use special toolchain file:

```bash
sudo apt-get install g++-mingw-w64-x86-64
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-MinGW64.cmake
make
```

Usage:
======

```
Usage: ./int2ssl [options] [-s value] file.int [file.ssl]
Example: ./int2ssl -d-1-a-b-e-s3 random.int

Options
  -d: dump file
  -1: input file is Fallout 1 script
  -a: ignore wrong number of arguments
  -b: insert omitted arguments backward
  -s: use Space instead of tab to indent
  -e: stop decompiling on error
  --: end of options
```
