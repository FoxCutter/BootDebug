The standard library implementation is a combination of two open source libraries and
some glue on my part. Someday it would be nice to replace this with a proper implementation
but that day is not today.


The first part came from LIBTINYC, this provides some basic library calls as well as
the support for VC's static initializes. I moved most of the OS specific code into a 
few places so it's easily isolated and removable. Not to mention replaceable at runtime.

The original version can be found at: http://www.wheaty.net/downloads.htm


The second part was the C99-snprintf library. This is an open source version of vsnprintf that 
supports pretty much everything but wide characters and has few dependencies. The only change
I had to make was to add some defines to remove the floating point code.

C99-snprintf can be found at: http://www.jhweiss.de/software/snprintf.html

Everything else are things I have added, though it's far from a complete version of the 
C Standard Library and has nothing of the C++ Library or (regrettably) the STL.


