# Building fac

$docnav

Before building fac, you need to install its prerequisites.  This
consists of a C compiler (either gcc or clang), python2, and python3.
On a Debian-based system, you can install all of this with

    apt-get install build-essential git python python3

(Yes, it is a little silly using both python2 and python3 in the build
process... I should probably switch entirely to python3.)

You can obtain the fac source code using git clone:

    git clone git://github.com/droundy/fac.git

To build fac (assuming you have just cloned fac, and do not have an
older version of fac) just run

    sh build/linux.sh

This should build fac on an x86-64 or 32-bit x86 linux system.  You
can then build an optimized version of fac by running

    ./fac fac

To use fac, you can copy the fac binary into some location in your
path.

You may also wish to build the documentation (by running `./fac`), but
this will require a few more packages: sass, graphviz,
python-markdown, and help2man.

## Installing on debian

If you simply want to install fac on a Debian machine, you can
download a [.deb package](fac-latest.deb) and install it with the
following commands:

    # wget https://physics.oregonstate.edu/~roundyd/fac/fac-latest.deb
    # dpkg -i fac-latest.deb

Or alternatively, you could download [a statically linked executable by
clicking this link](fac).
