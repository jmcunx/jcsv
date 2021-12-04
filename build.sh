#!/bin/sh
#
# Build script to use a generic
# Makefile for install
#
# Will take a template and generate
# a Makefile for use in building
# the software
#

if test "$1" != ""
then
cat << EOF
build.sh -- generate a Makefile for building on various systems.

    Will use Makefile.template in the  current directory to create
    Makefile.  Should work with any UN*X System, but this has been
    tested on these systems:
        NetBSD
        OpenBSD
        Linux -- Slackware, RHEL and CentOS
        AIX

    You can set these environment variables prior to executing build.sh
    to modify defaults.   This shows all variables and current defaults 
    per OS:

    .--------------------------------------------------------------.
    | Variable   | AIX     | Others   | Meaning                    |
    |------------+---------+----------+----------------------------+
    | CC         | cc      | cc       | c compiler to use          |
    |------------+---------+----------+----------------------------+
    | LOCAL_HOME |         |          | If this is set, it will    |
    |            |         |          | Override the values used   |
    |            |         |          | by REAL_DIR and DESTDIR if |
    |            |         |          | if they are not defined.   |
    |------------+---------+----------+----------------------------+
    | REAL_DIR   | ~/local | /opt/jmc | Expected final location    |
    |------------+---------+----------+----------------------------+
    | DESTDIR    | ~/local | /opt/jmc | Where to copy build to     |
    |------------+---------+----------+----------------------------+
    | DEBUG      |         |          | If set to any value will   |
    |            |         |          | turn on Debug Options      |
    |            |         |          | when creating Makefile     |
    '--------------------------------------------------------------'

    Note, DESTDIR and REAL_DIR  can  be different.  DESTDIR is used
          to allow one to  build an installable  package for use on
          other  phyical machines.   REAL_DIR is were  the software
          expects to be installed.  This is in case the build needs
          to access files for use by the binary.

EOF
exit 2
fi

OS=`uname -s`
export OS

found_dir_set="N"

s01="s/^ZZZZZ//"
s02="s/^ZZZZZ//"
s03="s/^ZZZZZ//"
s04="s/^ZZZZZ//"
s05="s/^ZZZZZ//"
s06="s/^ZZZZZ//"
s07="s/^ZZZZZ//"

if test ! -f Makefile.template
then
    echo "Missing Makefile.template"
    exit 2
fi

if test "$CC" != ""
then
    s01="s/^CC=.*/CC=$CC/"
fi

if test "$REAL_DIR" != ""
then
    found_dir_set="Y"
    s02="s%^REAL_DIR=.*%REAL_DIR=$REAL_DIR%"
    if test "$DESTDIR" = ""
    then
        s03="s%^DESTDIR=.*%DESTDIR=$REAL_DIR%"
    fi
fi

if test "$DESTDIR" != ""
then
    found_dir_set="Y"
    s03="s%^DESTDIR=.*%DESTDIR=$REAL_DIR%"
fi

if test "$DEBUG" != ""
then
    s04='s/^#DEBUG_BUILD/DEBUG_BUILD/'
fi

echo "$PREFIX" | grep "$HOME" > /dev/null 2>&1
if test "$?" = "0"
then
    s05='s/^OWNER=root/OWNER=$(USER)/;s/^WHEEL=wheel/WHEEL=$(USER)/'
fi
if test "$LOCAL_HOME" != ""
then
    if test "$found_dir_set" = "N"
    then
	s06="s%/opt/jmc%$LOCAL_HOME%"
    fi
fi
if test "$DEBUG" != ""
then
    s07="s/^#DEBUG_/DEBUG_/"
fi

case "$OS" in
    "Linux")
	# seemes make differs between Linux Distros
	lmtype="`uname -m`"
	lsys="`uname -srm`"
	sed 's/^#GNU#//' < Makefile.template \
	    | sed "$s01" | sed "$s02" | sed "$s03" | sed "$s04" | sed "$s05" | sed "$s06" \
	    | sed "s/OSDESC=ZZZ/OSDESC=$lsys/;s/mtype=ZZZ/mtype=$lmtype/" > Makefile
	;;
    AIX)
	lsys="`uname -srm`"
	sed 's/^#AIX#//;s/=\/opt\/jmc/=$(HOME)\/local/;s/OWNER=root/OWNER=$(USER)/;s/WHEEL=wheel/WHEEL=$(USER)/' < Makefile.template \
	    | sed "$s01" | sed "$s02" | sed "$s03" | sed "$s04" | sed "$s06" \
	    | sed "s/OSDESC=ZZZ/OSDESC=$lsys/" \
	    | grep -v '$(INSTALLD)' > Makefile
	;;
    *)
	sed 's/^#BSD#//' < Makefile.template \
	    | sed "$s01" | sed "$s02" | sed "$s03" | sed "$s04" | sed "$s05" | sed "$s06" \
	    > Makefile
	;;
esac

fmt_dest=`grep '^DESTDIR=' < Makefile | head -n 1 | sed 's/=/  = /'`
fmt_real=`grep '^REAL_DIR=' < Makefile | head -n 1 | sed 's/=/ = /'`

cat <<EOF

Makefile created, settings:

$fmt_dest
$fmt_real

EOF
