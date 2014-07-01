#include "mesh.h"
#include "show.h"
#include <cstdlib>
using namespace std;

int main(int argc , char** argv)
{
	float percent = atof(argv[3]);
	Mesh m;
	m.mesh_init(argv[1] , percent);
	m.calculate_Q();
	m.begin_simplify();
	m.get_answer(argv[2]);
	
	Show w(argv[2]);
	glutInit(&argc, argv);
	w.opengl_init();
	w.opengl_begin();

	return 0;
}