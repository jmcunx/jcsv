#!/bin/sh
#
# generate a Makefile from Makefile.unx
# for a UN*X Type Systems
#

#--- try and find were j_lib2 is located
f_findl()
{
    if test -f "$DESTDIR/$g_lib/libj_lib2.so" -o -f "$DESTDIR/$g_lib/libj_lib2.a"
    then
	g_include="$DESTDIR/include"
	g_libloc="$DESTDIR/$g_lib"
	return
    fi
    if test -f "/usr/$g_lib/libj_lib2.so" -o -f "/usr/$g_lib/libj_lib2.a"
    then
	g_include="/usr/include"
	g_libloc="/usr/$g_lib"
	return
    fi
    if test -f "/usr/local/$g_lib/libj_lib2.so" -o -f "/usr/local/$g_lib/libj_lib2.a"
    then
	g_include="/usr/local/include"
	g_libloc="/usr/local/$g_lib"
	return
    fi
    if test -f "/opt/jmc/$g_lib/libj_lib2.so" -o -f "/opt/jmc/$g_lib/libj_lib2.a"
    then
	g_include="/opt/jmc/include"
	g_libloc="/opt/jmc/$g_lib"
	return
    fi
    if test -f "$HOME/local/$g_lib/libj_lib2.so" -o -f "$HOME/local/$g_lib/libj_lib2.a"
    then
	g_include="$HOME/local/include"
	g_libloc="$HOME/local/$g_lib"
	return
    fi
    if test -f "$HOME/Archive/local/$g_lib/libj_lib2.so" -o -f "$HOME/Archive/local/$g_lib/libj_lib2.a"
    then
	g_include="$HOME/Archive/local/include"
	g_libloc="$HOME/Archive/local/$g_lib"
	return
    fi
    if test -f "/ccload/cdrtest/local/$g_lib/libj_lib2.so" -o -f "/ccload/cdrtest/local/$g_lib/libj_lib2.a"
    then
	g_include="$HOME/Archive/local/include"
	g_libloc="$HOME/Archive/local/$g_lib"
	return
    fi

    echo "E10: cannot find j_lib2"
    exit 2

} # END: f_findl()

#------------------------------------------------------------------------------
# set install location and OS Info
#------------------------------------------------------------------------------
if test "$DESTDIR" = ""
then
    DESTDIR="/usr/local"
    export DESTDIR
fi

if test "$OS" = ""
then
    OS=`uname -s`
    export OS
fi

case "`uname -m`" in
    "x86_64")
	if test "$OS" = "Linux"
	then
	    g_sed_1="^#LINUX64#"
	    g_lib="lib64"
	else
	    g_sed_1="^#LINUX32#"
	    g_lib="lib"
	fi
	;;
    "amd64")
	g_sed_1="^#BSD64#"
	g_lib="lib"
	;;
    *)
	if test "$OS" = "AIX"
	then
	    g_sed_1="^#AIX#"
	    g_lib="lib"
	else
	    echo "E01: $OS not supported"
	    exit 2
	fi
	;;
esac

type ectags > /dev/null 2>&1
if test "$?" -eq "0"
then
    g_sed_2="=ectags"
else
    g_sed_2="=ctags"
fi

case "$CC" in
    "cc")
	g_sed_3="CC=cc"
	;;
    "gcc")
	g_sed_3="CC=gcc"
	;;
    "clang")
	g_sed_3="CC=clang"
	;;
    *)
	g_sed_3="CC=cc"
	;;
esac

f_findl

#------------------------------------------------------------------------------
# create Makefile
#------------------------------------------------------------------------------
sed "s%$g_sed_1%%;s%=ctags%$g_sed_2%;s%CC=cc%$g_sed_3%;s%INCJLIB%$g_include%g;s%JLIBLOC%$g_libloc%g;s%LOCATION%$DESTDIR%" < Makefile.unx > Makefile

cat << EOF

Created Makefile
    OS     $OS
    j_lib2 Include Location:  $g_include
    j_lib2 Library Location:  $g_libloc
    Install Location:         $DESTDIR
    Compiler:                 `echo $g_sed_3 | sed 's/CC=//'`

Review and modify if necessary

EOF

exit 0
