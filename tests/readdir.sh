#!/bin/sh

set -ev

rm -rf $0.dir
mkdir $0.dir
cd $0.dir

cat > top.fac <<EOF
| echo *.message > messages
> messages
EOF

touch foo.message

git init
git add top.fac foo.message

../../fac

ls -lhd .

sleep 2

grep foo.message messages

touch bar.message
../../fac

ls -lhd .

grep foo.message messages
grep bar.message messages

exit 0
