CC=g++
CFLAGS=-fPIC
LDFLAGS=-shared
BUILD_DIR=../build/


OBJS=engine.o render/lighting/shadows/shadow.o render/objects/animation.o render/objects/animator.o render/objects/bone.o render/objects/mesh.o render/objects/model.o render/utils/shader.o window/events.o window/window.o

all: engine

engine: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)libengine.so $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm $(OBJS)
