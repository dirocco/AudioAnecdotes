#!/bin/sh

# do.cgi is allows executables to be run from a client browser via this
# cgi script on the server. Security is maintained by making all paths
# relative to a designated root directory and dissallowing ..'s
#
# 3 parameters (path, program, and arguments) are passed to do.cgi
# via the QUERY_STRING environment variable
# Parameters are delimited by ampersands
# Within the path slashes are encoded using plus signs
# Within the parameter list, parameters are delimited using underscores
# and a tilde substituted to the root directory AudioAnecdotes/Volume1
# Ex: <a href="do.cgi?bin&caliper&~Content+02Perception+2DiFillipo&clickfusion_-f_0|-f|1|-i|0.7|-m|1">
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

#ifdef DEBUG
touch /tmp/log
chmod 666 /tmp/log
#endif

if [ -f /bin/tr ]; then
   TR=/bin/tr
else
   TR=/usr/bin/tr
fi

if [ -f /bin/sed ]; then
   SED=/bin/sed
else
   SED=/usr/bin/sed
fi

#ifdef DEBUG
echo QUERY_STRING:$QUERY_STRING >> /tmp/log
#endif

# convert %20's back to spaces, and stricly elliminate dotdots,
# and any other dangerous symbols...
foo=`echo $QUERY_STRING   | \
     $SED 's/%20/ /g' | \
     $SED 's/\.\.//g' | \
     $SED 's/;//g'    | \
     $SED 's/\`//g'`

#ifdef DEBUG
echo QUERY_STRING:$foo >> /tmp/log
#endif

IFS='&'
set -- $foo

#ifdef DEBUG
echo path:[$1]  prog:[$2]  args[$3] >> /tmp/log
#endif

# fixup path 
p=`echo $1 | $SED 's/+/\//g'`

#ifdef DEBUG
echo p:[$p] >> /tmp/log
#endif

# root ourselves in the Volume1 directory
cd ../../
root=`/bin/pwd`/
#ifdef DEBUG
echo root:$root >> /tmp/log
#endif

if [ -d $p ]; then
#ifdef DEBUG
   echo $p exists >> /tmp/log
#endif
   cd ./$p
else
#ifdef DEBUG
   echo $p does not exist >> /tmp/log
#endif
   exit -1
fi

# verify that the executable exists
if [ -x $2 ]; then
#ifdef DEBUG
   echo $2 exists >> /tmp/log
#else
   echo
#endif
else
#ifdef DEBUG
   echo $2 does not exist >> /tmp/log
#endif
   exit -1
fi

# fixup arguments 
#ifdef DEBUG
echo pre-arg:$3 >> /tmp/log
#endif
a=`echo $3 \
   | $SED 's/_/ /g'  \
   | $SED 's/+/\//g' \
   | $SED "s:~:$root:g" \
   `

#ifdef DEBUG
echo a:[$a] >> /tmp/log
#endif

#ifdef UNIX

# we should inherit the proper DISPLAY from a (modified) thttpd 
# but in case the user didn't have DISPLAY set...
# set DISPLAY if it isn't already
if [ ${DISPLAY:="null"} = "null" ]; then
   export DISPLAY=:0.0
fi

# probe to see if we can access the xserver by calling xset q
xworked=0

# list of locations where xset may be
list="xset&/usr/bin/X11/xset&/usr/X11R6/bin/xset"
for prog in $list; do
#ifdef DEBUG
   echo trying:$prog >> /tmp/log
#endif
   $prog q
   result=$?
#ifdef DEBUG
   echo tried:$prog $result  >> /tmp/log
#endif

   # 1 is an err, although I receive 0 and 127 when working
   if [ $result -eq 1 ]; then
#ifdef DEBUG
      echo "could not $prog" >> /tmp/log
#else
      echo
#endif
   else
#ifdef DEBUG
      echo $prog worked >> /tmp/log
#endif
      xworked=1
      break  # no need to run others
   fi
done


#ifdef DEBUG
if [ $xworked -eq 1 ]; then
    echo XWORKED >> /tmp/log
else
    echo Xnoworkee >> /tmp/log
fi
#endif

IFS='|'

if [ $xworked -eq 1 ]; then
   list="xterm&/usr/bin/X11/xterm&/usr/X11R6/bin/xterm"
   (
      for prog in $list; do
#ifdef DEBUG
         echo run:$prog -e "./$2 $a" >> /tmp/log
         $prog -e "./$2 $a" 2>> /tmp/log
#else
         $prog -e "./$2 $a"
#endif

	 if [ $? -eq 0 ]; then
	    break;  # no need to run others
	 fi
      done
   )&
else # skip the xterm and just run in the background
   ./$2 $a &
fi
#else

doit=`/bin/cygpath --sysdir`/cmd
if [ -f $doit ]; then
   $doit /c start .\\$2 $a
else
#ifdef DEBUG
   echo "cmd nothere" >> /tmp/log
#endif
   doit=`/bin/cygpath --windir`/command
   $doit /c start .\\$2 $a
fi

#endif 

exit 0
