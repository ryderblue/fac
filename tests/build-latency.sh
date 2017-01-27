#!/bin/sh

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

t=`/usr/bin/time -f %e sh -c "for ((i=0; i<10; i=i+1)); do ../../fac > /dev/null && ../../fac -c > /dev/null; done" 2>&1`
if [[ $t > 5 ]]; then
  echo clean build takes too long
  exit 1
fi

exit 0
