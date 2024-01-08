CC = g++

OPENCV_INCLUDE = /usr/local/Cellar/opencv/4.8.1_5/include/opencv4
OPENCV_LIB_FLAG = -L/usr/local/Cellar/opencv/4.8.1_5/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_face -lopencv_features2d -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_dnn -lopencv_xfeatures2d -lopencv_plot -lopencv_tracking -lopencv_img_hash -lopencv_calib3d -lopencv_bgsegm -lopencv_text -lopencv_datasets -lopencv_dpm -lopencv_flann -lopencv_fuzzy -lopencv_hfs -lopencv_line_descriptor -lopencv_optflow -lopencv_phase_unwrapping -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_structured_light -lopencv_superres -lopencv_surface_matching -lopencv_ximgproc -lopencv_xobjdetect -lopencv_xphoto -lopencv_aruco -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect -lopencv_dnn_superres -lopencv_intensity_transform -lopencv_mcc -lopencv_quality -lopencv_rapid -lopencv_sfm -lopencv_stitching -lopencv_videostab -lopencv_viz

CFLAGS = -std=c++17 -I./libs -I./include -I./points_generator -I$(OPENCV_INCLUDE)
LDFLAGS = -framework OpenGL -framework GLUT -lGLEW -lglfw $(OPENCV_LIB_FLAG)
TARGET = out
SOURCES = main.cpp libs/SimplexNoise.cpp $(wildcard points_generator/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
