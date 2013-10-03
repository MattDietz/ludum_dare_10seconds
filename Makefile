CC=g++
CFLAGS=-I./include -I./include/config_lib -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio -I./include/config_lib -wALL -DSCOUNDREL_COMMAND_LINE=1
CONFIG_LIB_FLAGS=-I./include/config_lib -c
CONFIG_LIB_SOURCES=src/config_lib/color.cpp src/config_lib/configfile.cpp src/config_lib/configitem_base.cpp
SOURCES=src/player.cpp src/scoundrel.cpp src/entity.cpp src/rect.cpp src/tile.cpp src/camera.cpp src/animation.cpp src/battery.cpp src/collidable.cpp src/game_map.cpp src/level_loader.cpp src/exit_arrow.cpp
CONFIG_LIB_O=obj/color.o obj/configitem_base.o obj/configfile.o

all: color.o configfile.o configitem_base.o
	$(CC) -o ./bin/scoundrel $(CFLAGS) $(SOURCES) $(CONFIG_LIB_O)

color.o: src/config_lib/color.cpp include/config_lib/color.h
	$(CC) $(CONFIG_LIB_FLAGS) -c src/config_lib/color.cpp -o obj/color.o

configfile.o: src/config_lib/configfile.cpp include/config_lib/configfile.h
	$(CC) $(CONFIG_LIB_FLAGS) -c src/config_lib/configfile.cpp -o obj/configfile.o

configitem_base.o: src/config_lib/configitem_base.cpp include/config_lib/configitem_base.h
	$(CC) $(CONFIG_LIB_FLAGS) -c src/config_lib/configitem_base.cpp -o obj/configitem_base.o

