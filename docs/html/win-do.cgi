#!/bin/sh

echo "Server: thttpd/2.20c 21nov01"
echo "Content-type: text/plain"
echo "Date: "`date -R`
echo "Expires: "`date -R`
echo "Pragma: no-cache"
echo "Last-modified: "`date -R -r do.cgi`
echo "Accept-Ranges: bytes"
echo "Connection: close"
echo "Content-length: 7"
echo "Location: "$HTTP_REFERER
echo

#echo $QUERY_STRING
foo=`echo $QUERY_STRING | /bin/sed 's/&/ /g' | /bin/sed 's/\.\.//g'`
#echo $foo

set -- $foo
cd ../../Content/0$1
#pwd

doit=`/bin/cygpath --sysdir`/cmd
#echo $doit ./$2 $3
if [ -f $doit ]; then
   echo foundit
   $doit /c start .\\$2 $3
else
   echo nothere
   doit=`/bin/cygpath --windir`/command
   $doit /c start .\\$2 $3
fi

exit 0
