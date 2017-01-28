#!/bin/bash

# This test is for issue #ABC on github.

set -ev

rm -rf $0.dir
mkdir $0.dir
cd $0.dir

cat > top.fac <<EOF
| echo foo > foo
EOF

git init
git add top.fac

t=`/usr/bin/time -f %e sh -c 'i=0; while [ $i -lt 100 ]; do ../../fac > /dev/null && ../../fac -c > /dev/null; i=\`expr $i + 1\`; done' 2>&1`
t=`python -c "print int($t)"`
if expr $t \> 1; then
  echo clean build takes too long
  exit 1
fi

exit 0
