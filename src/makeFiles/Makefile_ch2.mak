-include Makefile.inc

PROJ_NAME=ch1

include Makefile_header.mak

#the target binary name
TARGETFILE=$(ODIR_BIN)/a.out

#link flags and lib searching paths
LFLAGS  := -Wall $(DEBUG) -L$(ODIR_LIB) -L$(GST_LIB) -L$(PLTF_LIB)

# link libs
LIBS    := -lgthread-2.0 -lgstvideo-1.0 -lgstbase-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstapp-1.0 -lglib-2.0 -lpng -lz -lv4l2 \
        -lv4l2 -lEGL -lGLESv2 -lX11 \
        -lavcodec -lavformat -lavutil -lswresample -lswscale -llzma -ldl -lm -lpthread -lrt

#-lUtil \
#        -lboost_timer -lboost_filesystem -lboost_system -lboost_date_time -lboost_regex \
#        -lboost_chrono -lpthread -lboost_thread \
#        -lopencv_highgui -lopencv_videoio  -lopencv_imgcodecs -lopencv_imgproc -lopencv_core \
#        -ljetson-utils -lncurses\
#        -lopencv_ml -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_flann \
#        -lopencv_stitching -lopencv_superres -lopencv_videostab \
#       -lavcodec -lavformat -lavutil -lswresample -lswscale -llzma -ldl -lm -lpthread -lrt
#       -lopencv_cudaarithm -lopencv_cudaimgproc -lopencv_cudafeatures2d -lopencv_cudawarping \

OBJS = \
        $(ODIR_OBJ)/ch1_ex1_use_bus_msg.o \
        $(ODIR_OBJ)/ch1_ex2_use_main_loop_v1.o \
        $(ODIR_OBJ)/ch1_ex2_use_main_loop_v2.o \
        $(ODIR_OBJ)/ch1_ex3_get_n_set_src_element_properties.o \
        $(ODIR_OBJ)/ch1_ex5_add_an_element.o \
        $(ODIR_OBJ)/ch1_ex6_use_gst_parse_launch.o \
        $(ODIR_OBJ)/ch1_ex7_cpp_ui.o \
        $(ODIR_OBJ)/Ch1CppExample.o \
        $(ODIR_OBJ)/main.o


default:  directories $(TARGETFILE)

directories:
	mkdir -p $(ODIR_OBJ)
	mkdir -p $(ODIR_LIB)
	mkdir -p $(ODIR_BIN)

#the output binary file name is <$(TARGETFILE)>
$(TARGETFILE)   :       $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) $(LIBS) $(LIBS) -o $(TARGETFILE)


$(ODIR_OBJ)/main.o      :       $(SDIR_PROJ)/main.cpp
	$(CXX) -o $(ODIR_OBJ)/main.o $(CFLAGS) $(SDIR_PROJ)/main.cpp

$(ODIR_OBJ)/ch1_ex1_use_bus_msg.o     :       $(SDIR_PROJ)/ch1_ex1_use_bus_msg.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex1_use_bus_msg.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex1_use_bus_msg.cpp

$(ODIR_OBJ)/ch1_ex2_use_main_loop_v1.o       :       $(SDIR_PROJ)/ch1_ex2_use_main_loop_v1.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex2_use_main_loop_v1.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex2_use_main_loop_v1.cpp

$(ODIR_OBJ)/ch1_ex3_get_n_set_src_element_properties.o       :       $(SDIR_PROJ)/ch1_ex3_get_n_set_src_element_properties.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex3_get_n_set_src_element_properties.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex3_get_n_set_src_element_properties.cpp

$(ODIR_OBJ)/ch1_ex5_add_an_element.o       :       $(SDIR_PROJ)/ch1_ex5_add_an_element.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex5_add_an_element.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex5_add_an_element.cpp

$(ODIR_OBJ)/ch1_ex6_use_gst_parse_launch.o       :       $(SDIR_PROJ)/ch1_ex6_use_gst_parse_launch.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex6_use_gst_parse_launch.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex6_use_gst_parse_launch.cpp

$(ODIR_OBJ)/ch1_ex7_cpp_ui.o       :       $(SDIR_PROJ)/ch1_ex7_cpp_ui.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex7_cpp_ui.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex7_cpp_ui.cpp

$(ODIR_OBJ)/ch1_ex2_use_main_loop_v2.o :       $(SDIR_PROJ)/ch1_ex2_use_main_loop_v2.cpp
	$(CXX) -o $(ODIR_OBJ)/ch1_ex2_use_main_loop_v2.o $(CFLAGS) $(SDIR_PROJ)/ch1_ex2_use_main_loop_v2.cpp

$(ODIR_OBJ)/Ch1CppExample.o :       $(SDIR_PROJ)/Ch1CppExample.cpp
	$(CXX) -o $(ODIR_OBJ)/Ch1CppExample.o $(CFLAGS) $(SDIR_PROJ)/Ch1CppExample.cpp

clean:
	\rm $(ODIR_OBJ)/*.o $(TARGETFILE)

rm_ch1:
	\rm $(TARGETFILE)
