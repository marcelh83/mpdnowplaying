#!/bin/sh

BASEDIR=".."
PROJECT="plasma_applet_org.kde.plasma.mpdnowplaying"
WDIR=`pwd`

XGETTEXT="xgettext --from-code=UTF-8 -C --kde -ki18n"

echo "Extracting messages"
cd ${BASEDIR}
$XGETTEXT `find . -name \*.cpp -o -name \*.h -o -name \*.qml` -o ${WDIR}/${PROJECT}.pot
cd ${WDIR}
echo "Done extracting messages"

echo "Merging translations"
catalogs=`find . -wholename '*/${PROJECT}.po'`
for cat in $catalogs; do
  echo $cat
  msgmerge -o $cat.new $cat ${PROJECT}.pot
  mv $cat.new $cat
done
echo "Done merging translations"       
