#!/bin/sh

set -ev

(clang -std=c99 -c fac.c)

(clang -std=c99 -c files.c)

(clang -std=c99 -c targets.c)

(clang -std=c99 -c clean.c)

(clang -std=c99 -c new-build.c)

(clang -std=c99 -c git.c)

(clang -std=c99 -c environ.c)

(cd lib && clang -std=c99 -c listset.c)

(cd lib && clang -std=c99 -c iterablehash.c)

(cd lib && clang -std=c99 -c sha1.c)

(cd lib && clang -std=c99 -c hashset.c)

(cd lib && clang -std=c99 -c posixmodel.c)

(python2 lib/linux-syscalls.py > lib/linux-syscalls.h)

(cd lib && clang -std=c99 -c bigbrother.c)

(clang -lpopt -lpthread -o fac fac.o files.o targets.o clean.o new-build.o git.o environ.o lib/listset.o lib/iterablehash.o lib/sha1.o lib/hashset.o lib/posixmodel.o lib/bigbrother.o)

