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
foo=`echo $QUERY_STRING | /bin/sed 's/%20/ /g' | /bin/sed 's/\.\.//g'`
#echo $foo

IFS='&'
set -- $foo
cd ../../
#pwd

doit=`/bin/cygpath --sysdir`/cmd
#echo $doit /c .\\$1\\$2 $3 $4 $5
if [ -f $doit ]; then
   echo foundit
   $doit /c start .\\$1\\$2 $3 $4 $5
else
   echo nothere
   doit=`/bin/cygpath --windir`/command
   $doit /c start .\\$1\\$2 $3 $4 $5
fi

exit 0
