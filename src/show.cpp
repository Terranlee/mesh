#include "show.h"

//用opengl显示的程序

float scale = 1.0;

float movex = 0.0;       //旋转，平移和缩放的参数
float movey = 0.0;
float movez = 0.0;

static float xmove = 0.0;
static float ymove = 0.0;
static float zmove = 0.0;

float ratex = 0.0;
float ratey = 0.0;
float ratez = 0.0;

float nummax = -1000000.0;

static float xrot = 1.0; 
static float yrot = 1.0; 
static float zrot = 1.0; 

CSimpleObject* input;
Vec3f* norm;
Vec3f* normp;

Show::Show(const char* file_name)          //初始化过程
{
	input = new CSimpleObject;
	input->LoadFromObj(file_name);
    for(int i=0; i<input->m_nVertices; i++)         //首先先读入所有的信息
    {
        if(input->m_pVertexList[i].x > nummax)
            nummax = input->m_pVertexList[i].x;
        if(input->m_pVertexList[i].y > nummax)
            nummax = input->m_pVertexList[i].y;
        if(input->m_pVertexList[i].z > nummax)
            nummax = input->m_pVertexList[i].z;
    }
    nummax /= 2;

    for(int i=0; i<input->m_nVertices; i++)         //每个obj的大小不太一样，放缩到大概相同的大小
    {
        input->m_pVertexList[i].x /= nummax;
        input->m_pVertexList[i].y /= nummax;
        input->m_pVertexList[i].z /= nummax;
    }
    
    norm = new Vec3f[input->m_nTriangles];
    Vec3f p0 , p1 , p2 , e1 , e2 , e;
    Array<int , 3> face;
    for(int i=0; i<input->m_nTriangles; i++)           //计算各个面的法向量
    {
        face = input->m_pTriangleList[i];
        p0 = input->m_pVertexList[face[0]];
        p1 = input->m_pVertexList[face[1]];
        p2 = input->m_pVertexList[face[2]];
        e1 = p0 - p1;
        e2 = p0 - p2;
        e = e1.cross(e2);
        e.Normalize();
        norm[i] = e;
    }
    normp = new Vec3f[input->m_nVertices];
    for(int i=0; i<input->m_nTriangles; i++)           //根据面的法向量计算出顶点的法向量
    {
        face = input->m_pTriangleList[i];
        p0 = input->m_pVertexList[face[0]];
        p1 = input->m_pVertexList[face[1]];
        p2 = input->m_pVertexList[face[2]];
        normp[face[0]] += norm[i];
        normp[face[1]] += norm[i];
        normp[face[2]] += norm[i];
    }
    for(int i=0; i<input->m_nVertices; i++)
        normp[i].Normalize();
}

void positions()                        //画出各个点的位置
{
	Array<int , 3> face;
	Vec3f p0 , p1 , p2;
    glBegin(GL_TRIANGLES);               //都是三角面片
    for(int i=0; i < input->m_nTriangles; i++)
    {
        face = input->m_pTriangleList[i];
        p0 = input->m_pVertexList[face[0]];
        p1 = input->m_pVertexList[face[1]];
        p2 = input->m_pVertexList[face[2]];
        glNormal3f(normp[face[0]].x , normp[face[0]].y , normp[face[0]].z);           //先设定法向量，在设定点的坐标
        glVertex3f(p0.x , p0.y , p0.z);
        glNormal3f(normp[face[1]].x , normp[face[1]].y , normp[face[1]].z);
        glVertex3f(p1.x , p1.y , p1.z);
        glNormal3f(normp[face[2]].x , normp[face[2]].y , normp[face[2]].z);
        glVertex3f(p2.x , p2.y , p2.z);
    }
    glEnd();
}

void set_material(void)                 //设定物体的材质
{
    GLfloat earth_mat_ambient[]  = {0.0f, 0.5f, 0.5f, 1.0f};
    GLfloat earth_mat_diffuse[]  = {0.0f, 0.5f, 0.5f, 1.0f};
    GLfloat earth_mat_specular[] = {0.0f, 0.7f, 0.7f, 1.0f};
    GLfloat earth_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat earth_mat_shininess = 0.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
}

void set_light(void)                    //设定光照模型
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	GLfloat sun_light_position[] = {0.0f, 5.0f, -5.0f, 0.0f};
    GLfloat sun_light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat sun_light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat sun_light_specular[] = {0.1f, 0.1f, 0.1f, 0.1f};

    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE , GL_TRUE);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, float(640) / float(480) , 1, 20.0);        //设定透视

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,5.0,-5.0,0.0,0.0,0.0,0.0,0.0,1.0);                //设定观察角度

    set_light();
    
    glScalef(scale , scale , scale);                                //进行平移或者旋转
    glRotatef(xrot, 1, 0, 0);
    glRotatef(yrot, 0, 1, 0);
    glRotatef(zrot, 0, 0, 1);
    glTranslatef(xmove , ymove , zmove);

    set_material();
    positions();

    xrot = xrot + ratex;
    yrot = yrot + ratey;
    zrot = zrot + ratez;
    
    xmove = xmove + movex;
    ymove = ymove + movey;
    zmove = zmove + movez;

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if(h==0) h = 1;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, -1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init(int width, int height )
{
    if(height == 0) height = 1;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glShadeModel(GL_SMOOTH);
}

void keyboard(unsigned char key, int w, int h)              //键盘事件，控制旋转，缩放等等
{
    switch (key){
        case 'q': glutFullScreen();
                  break;
        case 'Q': glutReshapeWindow(640, 480);
                  glutPositionWindow(400, 100);
                  break;
        case 'z': scale = scale + 1;
                  break;
        case 'x': scale = scale - 1;
                  break;
        case 's': ratex = ratey = ratez = 0.0;
                  movex = movey = movez = 0.0;
                  break;
        case 'i':
                  ratex = 1.0;
                  ratez = ratey = 0.0;
                  break;
        case 'k':
                  ratex = -1.0;
                  ratez = ratey = 0.0;
                  break;
        case 'j':
                  ratey = -1.0;
                  ratex = ratez = 0.0;
                  break;
        case 'l':
                  ratey = 1.0;
                  ratex = ratez = 0.0;
                  break;
        case 't': movey = 0.02;
                  movex = movez = 0.0;
                  break;
        case 'g': movey = -0.02;
                  movex = movez = 0.0;
                  break;
        case 'f': movex = 0.02;
                  movey = movez = 0.0;
                  break;
        case 'h': movex = -0.02;
                  movey = movez = 0.0;
                  break;
        case 27: exit(0);
    }
}

void Show::opengl_init()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100 , 100);
	glutInitWindowSize(640 , 480);
	glutCreateWindow("mesh_simplification");
}

void Show::opengl_begin()
{
	glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    init(640, 480);
    glutMainLoop();
}
