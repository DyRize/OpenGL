OPTIONS= ../Build/external/libglfw3.a ../Build/external/libGLEW_1130.a $(OPT)
OPTIONS+= -lGL -lGLU -lXrandr -lXext -lX11 -lrt -ldl -lrt -lm -lpthread -lXinerama -lXi -lXxf86vm -lXcursor -ldl -lrt -lm

WARNINGS= -Wall -pedantic -Wextra
INCLUDES= -I ../include/

OPTIONS+= $(WARNINGS)
OPTIONS+= $(INCLUDES)

playground : playground.o shader.o objloader.o texture.o 
	g++ -o playground playground.o shader.o objloader.o texture.o $(OPTIONS)

shader.o : ../common/shader.cpp ../common/shader.hpp
	g++ -c ../common/shader.cpp $(INCLUDES)

objloader.o : ../common/objloader.cpp ../common/objloader.hpp
	g++ -c ../common/objloader.cpp $(INCLUDES)
	

texture.o : ../common/texture.cpp ../common/texture.hpp
	g++ -c ../common/texture.cpp $(INCLUDES)
	
playground.o : playground.cpp ../common/shader.hpp ../common/texture.hpp ../common/objloader.hpp
	g++ -c playground.cpp $(INCLUDES)
	
all : playground


clean :
	rm *.o -fv *~

cleanall : clean
	rm playground -f

