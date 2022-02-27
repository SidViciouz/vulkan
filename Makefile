CC = g++
INC = -I../moltan/macOS/include -I/opt/homebrew/include -I.
LIB_DIRS = -L../moltan/macOS/lib -L/opt/homebrew/lib
LIBS = -lglfw.3.3 -lvulkan.1
SRCS = vulkan.cpp

vulkan : vulkan.o window.o renderer.o validation.o
	$(CC) $(LIBS) $(INC) $(LIB_DIRS) vulkan.o window.o renderer.o validation.o -o vulkan

vulkan.o : vulkan.cpp
	$(CC) $(LIBS) $(INC) $(LIB_DIRS) -c vulkan.cpp -o vulkan.o

window.o : window/window.cpp
	$(CC) $(LIBS) $(INC) $(LIB_DIRS) -c window/window.cpp -o window.o

renderer.o : renderer/renderer.cpp
	$(CC) $(LIBS) $(INC) $(LIB_DIRS) -c renderer/renderer.cpp -o renderer.o

validation.o : validation/validation.cpp
	$(CC) $(LIBS) $(INC) $(LIB_DIRS) -c validation/validation.cpp -o validation.o

clean :
	rm -rf *.0 vulkan
