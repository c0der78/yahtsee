#! /bin/sh

brew install dylibbundler

BASEDIR=$(dirname $0)

cd $BASEDIR

if [ ! -e "$BASEDIR/configure" ]; then
	echo "configure script not found at $BASEDIR/configure"
fi

if [ -e Makefile ]; then
	make distclean
fi

rm -rf temp

mkdir temp

cd temp

BASEDIR=$(
  cd $BASEDIR            # At this point, we are sure we're in script's directory
  dirname `pwd -P`
)

mkdir build yahtsee

cd build

$BASEDIR/configure --prefix=$BASEDIR/temp/yahtsee

make install

cd ..

dylibbundler -od -b -x ./yahtsee/bin/yahtsee -d ./yahtsee/lib -p @executable_path/../lib

echo '#!/bin/sh' >> yahtsee/runme
echo 'BASEDIR=$(dirname $0)' >> yahtsee/runme
echo 'BASEDIR=$(' >> yahtsee/runme
echo '  cd $BASEDIR' >> yahtsee/runme
echo '  dirname `pwd -P`/runme' >> yahtsee/runme
echo ')' >> yahtsee/runme
echo 'open -a "Terminal.app" $BASEDIR/bin/yahtsee' >> yahtsee/runme

chmod 755 yahtsee/runme

tar -cjf yahtsee.darwin.tar.bz2 yahtsee

mv yahtsee.darwin.tar.bz2 $BASEDIR

cd $BASEDIR

rm -rf temp

