CXX = g++
CXXFLAGS = --debug -std=c++20 # -march=native -fPIE

/home/_/.local/bin/mo2d_dbg: mo2d/main.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/SFML/include mo2d/main.cpp -o /home/_/tmp/tmp.o
	$(CXX) /home/_/tmp/tmp.o $(CXXFLAGS) -Llibdeps/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lMesaLib-devel -o /home/_/.local/bin/mo2d_dbg

build_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -c libdeps/imgui/*.cpp
	$(CXX) $(CXXFLAGS) -shared -o lib/imgui.so *.o
	rm -f *.o

build_sfml_imgui:
	$(CXX) $(CXXFLAGS) -fPIC -Ilibdeps/SFML/include -Ilibdeps/imgui -c libdeps/imgui-sfml/imgui-SFML.cpp
	$(CXX) $(CXXFLAGS) -shared -o lib/imguiSFML.so *.o
	rm -f *.o
