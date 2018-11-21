#!/bin/sh

if test "$1" = "update"
then
  # Update generated configuration files
  if command -v autoreconf >/dev/null
  then
    autoreconf --force --install --verbose --warnings=all
  else
    echo "Update failed, please install autoconf first"
    exit 1
  fi
else
  # Set up an m4 environment
  aclocal

  # Generate configure from configure.ac
  autoconf

  # reate a template header (config.h.in) for configure
  autoheader

  # generates a custom version of the generic libtool script
  libtoolize --copy

  # Generate Makefile.in from Makefile.am
  automake --add-missing --copy
fi

echo
echo 'run "./configure --prefix=/usr && make"'
echo

# Generate Makefile from Makefile.in
#./configure

# Use Makefile to build and test a tarball to distribute
#make distcheck
#make distclean

