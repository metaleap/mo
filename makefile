CXX = g++
CXXFLAGS = --debug -std=c++20 -march=native -fPIE

builds/mo2d_dbg: mo2d/main.cpp
	$(CXX) $(CXXFLAGS) mo2d/main.cpp  -lsfml-graphics -lsfml-window -lsfml-system -o builds/mo2d_dbg
