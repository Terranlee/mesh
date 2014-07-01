#ifndef __FACE_H__
#define __FACE_H__

//面和边的两个类

#include "point.h"
#include "SimpleObject.h"

class Face
{
public:
	bool fexist;              //这个面是否被删掉了
	int v0 , v1 , v2;         //一个三角型面中的三个顶点编号
	float arg[4];             //ax+by+cz+d=0中的四个参数
	Matrix Qf;                //这个面的Q矩阵
	Face(const Array<int,3>& temp)
	{
		v0 = temp[0];
		v1 = temp[1];
		v2 = temp[2];
		fexist = true;
		Qf.mat_init();
	}
	void change_y_to_x(int x , int y)         //把原先面中有y点的，现在都改成x点
	{
		if(v0 == y) v0 = x;
		if(v1 == y) v1 = x;
		if(v2 == y) v2 = x;
	}
	void change_Qf()
	{
		Qf.calculate_mat(arg);
	}
	int find_different(int x , int y)        //寻找面中那个既不是x又不是y的点
	{
		if(v0 != x && v0 != y) return v0;
		if(v1 != x && v1 != y) return v1;
		if(v2 != x && v2 != y) return v2;
	}
};

class Pair            //每一条边
{
public:
	int v1 , v2;
	float delta;
	bool pexist;
	Vec3f middle_point;          //中点坐标
	int pchange;                 //这一条边的生成时间，如果和两个节点的生成时间之和相等的话，就说明这是一条最新更新过的边

	Pair(const int arg1 , const int arg2) { v1 = arg1 ; v2 = arg2; delta = 0; pchange = 0; }
	void set_middle(const Vec3f& temp) { middle_point = temp; }
	friend bool operator< (const Pair& a , const Pair& b) { return a.delta > b.delta; }
	
	void calculate_delta(const Point& v1 , const Point& v2)
	{
		Matrix Qmat = v1.Q;
		Qmat += v2.Q;
		float temp[4];
		for(int i=0; i<3; i++)
			temp[i] = middle_point[i];
		temp[3] = 1.0;
		for(int i=0; i<mat_size; i++)
			for(int j=0; j<mat_size; j++)
				delta += Qmat.mat[i][j] * temp[i] * temp[j];
	}
};

#endif
