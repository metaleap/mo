mo2d: bin/mo2d_main.o
	bin/mo2d_main.o

bin/mo2d_game_game.o: mo2d/game/game.cpp, mo2d/game/game.h, mo2d/game/game.cpp, bin/mo2d_game_game.o
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui-sfml mo2d/game/game.cpp -o bin/mo2d_game_game.o

bin/mo2d_gui_gui.o: mo2d/gui/gui.cpp, mo2d/gui/gui.h, mo2d/gui/gui.cpp, bin/mo2d_gui_gui.o
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui-sfml mo2d/gui/gui.cpp -o bin/mo2d_gui_gui.o

bin/mo2d_main.o: mo2d/main.cpp, mo2d/gui/gui.h, mo2d/gui/gui.cpp, bin/mo2d_gui_gui.o, mo2d/game/game.h, mo2d/game/game.cpp, bin/mo2d_game_game.o
	$(CXX) -c $(CXXFLAGS) -Ilibdeps/imgui -Ilibdeps/imgui-sfml mo2d/main.cpp -o bin/mo2d_main.o

