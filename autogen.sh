#! /bin/sh
#
#  Initialize the automake/autoconf build framework for the script virtual
# machine.
#
# Written by Ron R Wills

aclocal
automake -a -c --foreign
autoconf
