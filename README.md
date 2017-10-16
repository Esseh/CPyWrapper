A simple wrapper to get around a strange reference count error and safely call python functions from C++. Mainly for personal use.

Compiled in mingw, Python 2.7 is used. As long as you match the proper python size with proper C++ compiler size it should compile fine(ie: 32bit python with 32 bit C++.)
The cannibalized library can be found in any Python installation in the libs folder.