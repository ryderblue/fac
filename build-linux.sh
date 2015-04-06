#!/bin/sh

set -ev

(gcc -std=c11 -c clean.c)

(gcc -std=c11 -c environ.c)

(python3 generate-version-header.py > version-identifier.h)

(gcc -std=c11 -c fac.c)

(gcc -std=c11 -c files.c)

(gcc -std=c11 -c git.c)

(python2 lib/freebsd-syscalls.py > lib/freebsd-syscalls.h)

(python2 lib/linux-syscalls.py > lib/linux-syscalls.h)

(cd lib && gcc -std=c11 -c bigbrotheralt.c)

(cd lib && gcc -std=c11 -c hashset.c)

(cd lib && gcc -std=c11 -c iterablehash.c)

(cd lib && gcc -std=c11 -c listset.c)

(cd lib && gcc -std=c11 -c posixmodel.c)

(cd lib && gcc -std=c11 -c sha1.c)

(gcc -std=c11 -c new-build.c)

(gcc -std=c11 -c targets.c)

(gcc -lpopt -lpthread -lm -o fac fac.o files.o targets.o clean.o new-build.o git.o environ.o lib/listset.o lib/iterablehash.o lib/sha1.o lib/hashset.o lib/posixmodel.o lib/bigbrotheralt.o)

