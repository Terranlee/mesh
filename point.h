#ifndef __POINT_H__
#define __POINT_H__

//有关点和矩阵的类

#include <vector>
#include <iostream>
#include "Vec3f.h"
using namespace std;
using namespace SimpleOBJ;

const int mat_size = 4;

class Matrix
{
public:
	float mat[mat_size][mat_size];
	Matrix() {}
	void mat_init()
	{
		for(int i=0; i<mat_size; i++)
			for(int j=0; j<mat_size; j++)
				mat[i][j] = 0;
	}
	Matrix operator += (const Matrix&b)               //矩阵，加法
	{
		for(int i=0; i<mat_size; i++)
			for(int j=0; j<mat_size; j++)
				mat[i][j] += b.mat[i][j];
	}
	void calculate_mat(const float* args)            //根据四个参数算矩阵
	{
		for(int i=0; i<mat_size; i++)
			for(int j=0; j<mat_size; j++)
				mat[i][j] = args[i] * args[j];
	}
};

class Point
{
public:
	bool vexist;                  //点是不是还存在
	int vchange;                  //vchange值配合优先级队列的pchange值可以判断哪条边被更新了
	Matrix Q;
	vector<int> planes;           //顶点的planes，储存周围的面的编号
	vector<int> edges;            //顶点的edges，储存周围的点的编号
	Vec3f num;                    //顶点的坐标
	Point(const Vec3f& temp){
		num = temp;
		Q.mat_init();
		vexist = true;
		vchange = 0;
	}
	void build_planes(int p) { planes.push_back(p); }
	void build_edges(int e) { edges.push_back(e); }
	void add_mat(const Matrix& temp)
	{
		Q += temp;
	}
};

#endif
