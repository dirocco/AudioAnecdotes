##############################################################################
# 
# config.mak for root of tree
#
# By: H. Gregory Silber
#

#subdirectories of the directory
SUBDIRS=Example \
	David_Thiel \
	Perry_Cook

#the path to the directory which contains configure for this directory
CONFIGROOT=.

#includes for a configure parent
include ../include/make-includes/config.make.inc
include ../include/make-includes/config.parent.make.inc
