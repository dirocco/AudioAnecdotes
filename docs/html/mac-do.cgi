#!/usr/bin/sh

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

# really should start this in an xterm equivalent so it can be killed...
# /cygdrive/c/windows/system32/cmd.exe /c start $foo
echo foundit
#echo ./$2 $3
./$2 $3

exit 0
