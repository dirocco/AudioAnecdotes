#subdirectories of the directory
include make.dirs

#the path to the directory which contains configure for this directory
CONFIGROOT=..

#includes for a configure parent
include ../../include/make-includes/config.make.inc
include ../../include/make-includes/config.parent.make.inc
