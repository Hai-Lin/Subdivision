CC = g++
CFLAGS =   -Wall
PROG =obj_hl2656 
SRCS=display_obj.cpp trackball.cpp Vector3D.cpp Mesh.cpp 

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lGL -lGLU -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
