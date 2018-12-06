
GL_LIBS = `pkg-config --static --libs glfw3 glew`
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3` 

CC = g++ -std=c++11
EXE = assign3_part2
OBJS = Main.o Mesh.o Shader.o tiny_obj_loader.o Skybox.o Lighting.o FastNoise.o ProceduralGenerator.o Floor.o
OPTS = -L"/usr/lib" external_files/irrklang/libIrrKlang.so

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS) $(OPTS)

Main.o: Main.cpp external_files/InputState.h SegmentRandomiser.h
	$(CC) $(CPPFLAGS) -c Main.cpp

Shader.o : external_files/Shader.cpp external_files/Shader.hpp
	$(CC) $(CPPFLAGS) -c external_files/Shader.cpp

Mesh.o: Mesh.h Mesh.cpp	
	$(CC) $(CPPFLAGS) -c Mesh.cpp
	
Lighting.o: Lighting.h Lighting.cpp	
	$(CC) $(CPPFLAGS) -c Lighting.cpp
	
tiny_obj_loader.o: external_files/tiny_obj_loader.cc external_files/tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c external_files/tiny_obj_loader.cc external_files/tiny_obj_loader.h
	
Skybox.o: Skybox.cpp Skybox.h
	$(CC) $(CPPFLAGS) -c Skybox.cpp
	
ProceduralGenerator.o: ProceduralGenerator.h ProceduralGenerator.cpp
	$(CC) $(CPPFLAGS) -c ProceduralGenerator.cpp
	
FastNoise.o: external_files/FastNoise.h external_files/FastNoise.cpp
	$(CC) $(CPPFLAGS) -c external_files/FastNoise.cpp
	
Floor.o: Floor.cpp Floor.h
	$(CC) $(CPPFLAGS) -c Floor.cpp

clean:
	rm -f *.o $(EXE)$(EXT) external_files/tiny_obj_loader.h.gch
