# DO NOT EDIT: auto-generated by makemake.go via build_*.sh

CXX = g++
CXXFLAGS = --debug -std=c++20 -march=native


bin/mo2d: bin/mo2d_gui_gui.o bin/mo2d_liveview_liveview.o bin/mo2d_main.o
	$(CXX) $(CXXFLAGS) -Lbin -limgui -limgui-sfml -lsfml-graphics -lsfml-window -lsfml-system -lGL bin/mo2d_main.o bin/mo2d_gui_gui.o bin/mo2d_liveview_liveview.o -o bin/mo2d

bin/mo_mapgen: bin/mo_mapgen_main.o
	$(CXX) $(CXXFLAGS) -Lbin -lnoise bin/mo_mapgen_main.o -o bin/mo_mapgen

clean:
	rm -f bin/*.o
	rm -f bin/mo2d
	rm -f bin/mo_mapgen
# NOTE on clean: all bin/*.so files _stay_! they're rarely or never updated 3rd-party deps.


bin/mo_mo.o: mo/mo.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui/backends -Ilibdeps/imgui/misc/cpp -Ilibdeps/imgui/misc/freetype -Ilibdeps/imgui/misc/single_file -Ilibdeps/imgui-sfml -Ilibdeps/libnoise/include/noise -Ilibdeps/libnoise/include/noise/model -Ilibdeps/libnoise/include/noise/module mo/mo.cpp -o bin/mo_mo.o

bin/mo2d_gui_gui.o: mo2d/gui/gui.cpp mo2d/gui/gui.h mo2d/gui/gui.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui/backends -Ilibdeps/imgui/misc/cpp -Ilibdeps/imgui/misc/freetype -Ilibdeps/imgui/misc/single_file -Ilibdeps/imgui-sfml -Ilibdeps/libnoise/include/noise -Ilibdeps/libnoise/include/noise/model -Ilibdeps/libnoise/include/noise/module mo2d/gui/gui.cpp -o bin/mo2d_gui_gui.o

bin/mo2d_liveview_liveview.o: mo2d/liveview/liveview.cpp mo2d/liveview/liveview.h mo2d/liveview/liveview.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui/backends -Ilibdeps/imgui/misc/cpp -Ilibdeps/imgui/misc/freetype -Ilibdeps/imgui/misc/single_file -Ilibdeps/imgui-sfml -Ilibdeps/libnoise/include/noise -Ilibdeps/libnoise/include/noise/model -Ilibdeps/libnoise/include/noise/module mo2d/liveview/liveview.cpp -o bin/mo2d_liveview_liveview.o

bin/mo2d_main.o: mo2d/main.cpp mo2d/gui/gui.h mo2d/gui/gui.cpp mo2d/liveview/liveview.h mo2d/liveview/liveview.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui/backends -Ilibdeps/imgui/misc/cpp -Ilibdeps/imgui/misc/freetype -Ilibdeps/imgui/misc/single_file -Ilibdeps/imgui-sfml -Ilibdeps/libnoise/include/noise -Ilibdeps/libnoise/include/noise/model -Ilibdeps/libnoise/include/noise/module mo2d/main.cpp -o bin/mo2d_main.o

bin/mo_mapgen_main.o: mo_mapgen/main.cpp
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui/backends -Ilibdeps/imgui/misc/cpp -Ilibdeps/imgui/misc/freetype -Ilibdeps/imgui/misc/single_file -Ilibdeps/imgui-sfml -Ilibdeps/libnoise/include/noise -Ilibdeps/libnoise/include/noise/model -Ilibdeps/libnoise/include/noise/module mo_mapgen/main.cpp -o bin/mo_mapgen_main.o

