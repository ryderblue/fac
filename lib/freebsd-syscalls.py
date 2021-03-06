#!/usr/bin/python2

import gentables, master

with open('lib/freebsd/syscalls.master') as f:
    sysnames = master.parse(f.read())

gentables.tables(sysnames, '_freebsd')
