#!/bin/sh
set -e
PROJECT=sni-qt

die() {
    echo "ERROR: $*" >&2
    exit 1
}

usage() {
    echo "$0"
    exit 1
}

tag=$(awk 'NR == 1 { print $2 }' ../NEWS)

echo -n "Found version '$tag'. Continue? (y/n) "
read answer
if [ "$answer" != "y" ] ; then
    die "Cancelled"
fi

tarball=$PROJECT-$tag.tar.bz2

[ -e $tarball ] || die "'$tarball' does not exist"

set -v
bzr tag $tag
bzr push
lp-project-upload $PROJECT $tag $tarball "" /dev/null /dev/null
