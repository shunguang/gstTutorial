-include Makefile.inc

#-----------------------------------------------------------------------
# Todo: generic for different users
#
# env variables need to defined in /home/nvidia/.bashrc 
#
# export APP_ROOT=/home/wus1/projects/gstTutorial
# export CUDA_INC=/usr/local/cuda-10.0/targets/aarch64-linux/include
# export CUDA_LIB=/usr/local/cuda-10.0/targets/aarch64-linux/lib
# ...
#----------------------------------------------------------------------
#build intermediat output paths

APP_ROOT=/home/wus1/projects/gstTutorial
SDIR_ROOT=$(APP_ROOT)/src
SDIR_PROJ=$(APP_ROOT)/src/$(PROJ_NAME)

ODIR_ROOT=$(APP_ROOT)/build-linux
ODIR_OBJ=$(ODIR_ROOT)/$(PROJ_NAME)
ODIR_LIB=$(ODIR_ROOT)/libs
ODIR_BIN=$(ODIR_ROOT)/bin

#include and lib paths of the platform
PLTF_INC=/usr/include
PLTF_LIB=/usr/lib

# BOOST_INC=/usr/include
# BOOST_LIB=/usr/lib
# CV_INC=/usr/include/opencv4
# CV_LIB=/usr/lib
# CUDA_INC=/usr/local/cuda-11.4/targets/aarch64-linux/include
# CUDA_LIB=/usr/local/cuda-11.4/targets/aarch64-linux/lib

# use 
# sudo find / -name glibconfig.h
# sudo find / -name gst.h
# to find where your gst headers and libs are installed

# this is an exmaple of potential arm machine
#I_GST_INC=-I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include
#GST_LIB=/usr/lib/aarch64-linux-gnu/gstreamer-1.0

# this is an exmaple of potential x86 machine
I_GST_INC=-I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
GST_LIB=/usr/lib/x86_64-linux-gnu/gstreamer-1.0

CXX = /usr/bin/g++
DEBUG = -DNDEBUG -g
#DEBUG = -DDEBUG -g

#include flags
CFLAGS  := -Wall -c $(DEBUG) -DqDNGDebug=1 -D__xlC__=1 -DNO_FCGI_DEFINES=1 -DqDNGUseStdInt=0 -DUNIX_ENV=1 -D__LITTLE_ENDIAN__=1 -DqMacOS=0 -DqWinOS=0 -std=gnu++11 \
        -I$(SDIR_PROJ) -I$(SDIR_ROOT) $(I_GST_INC) -I$(PLTF_INC)

TARGETFILE=$(ODIR_LIB)/$(PROJ_NAME).a

$(info $$SDIR_PROJ=[${SDIR_PROJ}])
$(info $$ODIR_ROOT=[${ODIR_ROOT}])
$(info $$ODIR_OBJ=[${ODIR_OBJ}])
$(info $$ODIR_LIB=[${ODIR_LIB}])
