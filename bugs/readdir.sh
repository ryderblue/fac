#!/bin/sh

set -ev

rm -rf $0.dir
mkdir $0.dir
cd $0.dir
cat > top.bilge <<EOF
| echo *.message > messages
> messages
EOF

touch foo.message
../../bilge

grep foo.message messages

touch bar.message
../../bilge

grep foo.message messages
grep bar.message messages

exit 0