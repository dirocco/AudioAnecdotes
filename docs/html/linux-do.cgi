#!/bin/sh

# do.cgi is allows executables to be run from a client browser via this
# cgi script on the server. Security is maintained by making all paths
# relative to a designated root directory and dissallowing ..'s
#
# NOTE: we have a problem that when run from thttpd as a shell script
#  some xservers won't allos us to connect to the :0.0 display
#  we probe and detect this and just don't run our command in an
#  xterm in this case.

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

foo=`echo $QUERY_STRING | /bin/sed 's/%20/ /g' | /bin/sed 's/\.\.//g'`
IFS='&'
set -- $foo

# root ourselves in the root directory
cd ../../

# we should realy inherit the proper DISPLAY however thttpd apparently
# exec's cgi scripts w/o including the environment (should fix thttpd)

# set DISPLAY if it isn't already
if [ ${DISPLAY:="null"} = "null" ]; then
   export DISPLAY=:0.0
fi

# probe to see if we can access the xserver by calling xset q
xworked=0

# list of locations where xset may be
list="xset&/usr/bin/X11/xset&/usr/X11R6/bin/xset"
for prog in $list; do
   $prog q 2>> /tmp/log

   if [ $? -eq 0 ]; then
      echo $prog worked >> /tmp/log
      xworked=1;
      break;  # no need to run others
   else
      echo "could not $prog" >> /tmp/log
   fi

done


echo HI >> /tmp/log

if [ $xworked -eq 1 ]; then
    echo XWORKED >> /tmp/log
else
    echo Xnoworkee >> /tmp/log
fi

if [ $xworked -eq 1 ]; then
   list="xterm&/usr/bin/X11/xterm&/usr/X11R6/bin/xterm"
   (
      for prog in $list; do
	 $prog -e ./$1/$2 $3 $4 $5 $6 $7 $8 $9 2>> /tmp/log

	 if [ $? -eq 0 ]; then
	    break;  # no need to run others
	 fi
      done
   )&
else # skip the xterm and just run in the background
   ./$1/$2 $3 $4 $5 $6 $7 $8 $9 &
fi

echo Ho >> /tmp/log
exit 0
