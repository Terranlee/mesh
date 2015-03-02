#ifndef __SHOW_H__
#define __SHOW_H__
#include <GL/glut.h>
#include <iostream>
#include "Vec3f.h"
#include "SimpleObject.h"

using namespace std;
using namespace SimpleOBJ;

extern CSimpleObject* input;

class Show
{
public:
	Show(const char*);
	void opengl_init();
	void opengl_begin();
};

void keyboard(unsigned char, int, int);
void init(int , int);
void reshape(int, int);
void display(void);
void positions();

void set_matetial(void);
void set_light(void);

#endif