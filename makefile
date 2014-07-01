all:main

OBJECT = Vec3f.o SimpleObject.o mesh.o show.o mesh_simplify.o
OPENGL = -D_STDCALL_SUPPORTED -D_M_IX86 -lglut -lGLU -lGL

main:$(OBJECT)
	@echo linking
	g++ $(OBJECT) -o main $(OPENGL)

mesh_simplify.o:mesh_simplify.cpp mesh.h
	@echo compiling mesh_simplify.cpp
	g++ -c mesh_simplify.cpp
show.o:show.cpp show.h
	@echo compiling show.o
	g++ -c show.cpp $(OPENGL)
mesh.o:mesh.cpp mesh.h point.h face.h
	@echo compiling mesh.cpp
	g++ -c mesh.cpp
Vec3f.o:Vec3f.h Vec3f.cpp
	@echo compiling Vec3f.cpp
	g++ -c Vec3f.cpp 
SimpleObject.o:SimpleObject.h SimpleObject.cpp
	@echo compiling SimpleObject.cpp
	g++ -c SimpleObject.cpp

.PHONY:clean cleanall
clean:
	rm OBJECT
cleanall:
	rm OBJECT main