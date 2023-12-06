CXX = g++
CXXFLAGS = --debug -std=c++20 -march=native

/home/_/.local/bin/mo2d_dbg: mo2d/main.cpp
	$(CXX) -c $(CXXFLAGS) -fPIE -Ilibdeps/imgui -Ilibdeps/imgui-sfml mo2d/main.cpp -o /home/_/tmp/tmp.o
	$(CXX) /home/_/tmp/tmp.o $(CXXFLAGS) -Llib -lsfml-graphics -lsfml-window -lsfml-system -lGL -limgui -lSFMLimgui -o /home/_/.local/bin/mo2d_dbg

build_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -c libdeps/imgui/*.cpp
	$(CXX) $(CXXFLAGS) -shared -o lib/libimgui.so *.o
	rm -f *.o

build_sfml_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -Ilibdeps/imgui -c libdeps/imgui-sfml/imgui-SFML.cpp
	$(CXX) $(CXXFLAGS) -shared -o lib/libSFMLimgui.so *.o
	rm -f *.o
