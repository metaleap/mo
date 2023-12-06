CXX = g++
CXXFLAGS = --debug -std=c++20 -march=native

/home/_/.local/bin/mo2d_dbg: mo2d/main.cpp
	$(CXX) -c $(CXXFLAGS) -fPIE -Ilibdeps/imgui -Ilibdeps/imgui-sfml mo2d/main.cpp mo2d/gui/gui.cpp mo2d/game/game.cpp
	$(CXX) $(CXXFLAGS) -Lbin -lsfml-graphics -lsfml-window -lsfml-system -lGL -limgui -lSFMLimgui -o /home/_/.local/bin/mo2d_dbg *.o
	rm -f *.o

build_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -c libdeps/imgui/*.cpp
	$(CXX) $(CXXFLAGS) -shared -o bin/libimgui.so *.o
	rm -f *.o

build_sfml_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -Ilibdeps/imgui -c libdeps/imgui-sfml/imgui-SFML.cpp
	$(CXX) $(CXXFLAGS) -shared -o bin/libSFMLimgui.so *.o
	rm -f *.o
