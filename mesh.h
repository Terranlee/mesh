#ifndef __MESH_H__
#define __MESH_H__
#include "point.h"
#include "face.h"
#include "SimpleObject.h"
#include "memory.h"
#include <fstream>
#include <queue>

//网格类

class Mesh
{
private:
	CSimpleObject* origin;          //原始数据，直接调用SimpleObject的接口获得

	vector<Point> points;          //全部的点
	vector<Face> faces;            //全部的面
	priority_queue<Pair> pairqueue;        //边的优先级队列

	int finalsize;
	int vsize;
	int fsize;
	float args[mat_size];
	int current_change;           //当前的change值，用来更新优先级队列

	void init_points();           //初始化过程的函数
	void init_faces();
	void calculate_args(Face&);
	void build_pairqueue();
	void rebuild();              //重新构造一遍每个point的edges和planes
	
	void simplify_one();                          //单次删除操作，需要调用下面的四个函数
		void delete_faces(int , int);                  //删除面
		void change_neighbours(int , int);             //改变连结关系
		void change_Qofx(int);                         //改变矩阵
		void change_pairqueue(int);                    //改变优先级队列
	void reshape_answer(int*);

public:
	Mesh() {
		finalsize = 0;
		vsize = 0;
		fsize = 0;
		current_change = 0;
		}
	void mesh_init(const char* , float );
	void calculate_Q();
	void begin_simplify();
	void get_answer(const char*);
	void test();
};

#endif
