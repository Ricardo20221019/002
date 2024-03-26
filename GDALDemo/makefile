CXX ?= g++

GDAL_INCLUDE_PATH = /usr/include/gdal
GDAL_LIB_PATH = /usr/lib
SQLITE3_INCLUDE_PATH = /home/chenlang/QtSDK/sqlite-autoconf-3360000/build/include
SQLITE3_LIB_PATH = /home/chenlang/QtSDK/sqlite-autoconf-3360000/build/lib

GDALFindShortestPath: main.cpp  ShortestPath/shortestpath.cpp  
	$(CXX) -std=c++11 -I$(GDAL_INCLUDE_PATH) -I$(SQLITE3_INCLUDE_PATH) $^  -o GDALFindShortestPath -L$(GDAL_LIB_PATH) -lgdal -L$(SQLITE3_LIB_PATH) -lsqlite3

clean:
	rm  -r GDALFindShortestPath
