#include "mesh.h"

//有关网格简化的相关函数

void Mesh::mesh_init(const char* file_name , float percent)
{
	origin = new CSimpleObject;
	origin->LoadFromObj(file_name);
	init_points();                   //初始化节点
	init_faces();                    //初始化面
	finalsize = int(fsize * percent);
	cout<<finalsize<<endl;
}

void Mesh::init_points()
{
	for(int i=0; i<origin->m_nVertices; i++)
	{
		Point temp(origin->m_pVertexList[i]);
		points.push_back(temp);
	}
	vsize = points.size();
}

void Mesh::calculate_args(Face& temp)                //计算每一个面的参数
{
	Vec3f e1 = points[temp.v0].num - points[temp.v1].num;
	Vec3f e2 = points[temp.v1].num - points[temp.v2].num;
	Vec3f e = e1.cross(e2);
	e.Normalize();
	for(int i=0; i<3; i++)
		temp.arg[i] = e[i];
	temp.arg[3] = - e.product(points[temp.v0].num);
	temp.change_Qf();
}

void Mesh::init_faces()
{
	for(int i=0; i<origin->m_nTriangles; i++)
	{
		Face temp(origin->m_pTriangleList[i]);
		calculate_args(temp);
		faces.push_back(temp);
	}
	fsize = faces.size();
	for(int i=0; i<fsize; i++)
	{
		points[faces[i].v0].build_planes(i);           //构造每一个顶点的planes，也就是周围的面
		points[faces[i].v1].build_planes(i);
		points[faces[i].v2].build_planes(i);
	}
	for(int i=0; i<fsize; i++)
	{
		points[faces[i].v0].build_edges(faces[i].v1);  //构造每一个顶点的edges，也就是周围的相邻节点
		points[faces[i].v1].build_edges(faces[i].v2);
		points[faces[i].v2].build_edges(faces[i].v0);
	}
}

void Mesh::calculate_Q()                        //计算每个点的Q矩阵，也就是吧周围所有面的矩阵都加起来
{
	for(int i=0; i<vsize; i++)
		for(int j=0; j<points[i].planes.size(); j++)
			points[i].add_mat(faces[ points[i].planes[j] ].Qf);
}

void Mesh::build_pairqueue()                   //构造pair的优先队列
{
	for(int i=0; i<vsize; i++)
		for(int j=0; j<points[i].edges.size(); j++)
		{
			Pair temp(i , points[i].edges[j]);
			Vec3f mid = (points[i].num + points[points[i].edges[j]].num) / 2;
			temp.set_middle(mid);
			temp.calculate_delta(points[i] , points[ points[i].edges[j] ]);
			pairqueue.push(temp);
		}
}

void Mesh::delete_faces(int x , int y)               //如果吧x,y两个节点删除掉，需要删除掉的面有哪些
{
	int xfnum;
	for(int i=0; i<points[x].planes.size(); i++)
	{
		xfnum = points[x].planes[i];
		if(faces[xfnum].fexist == false)
			continue;
		bool same = false;
		if(faces[xfnum].v0 == y) same = true;
		else if(faces[xfnum].v1 == y) same = true;
		else if(faces[xfnum].v2 == y) same = true;
		if(same)
		{
			faces[xfnum].fexist = false;                //找x,y两个点都有的面，把那个面删掉就行
			fsize--;			
		}
	}
	for(int i=0; i<points[y].planes.size(); i++)          //y已经被删掉了，周围的面原来有某一个节点是y，现在就有某一个节点是x，
														  //而且x周围的面也会加上原先y周围的面
	{
		if(faces[points[y].planes[i]].fexist == true)
		{
			faces[points[y].planes[i]].change_y_to_x(x , y);
			points[x].planes.push_back(points[y].planes[i]);
		}
	}
	points[y].vexist = false;
	vsize--;
	points[y].planes.clear();
}

void Mesh::change_neighbours(int x , int y)
{
	int ynei;
	for(int i=0; i<points[y].edges.size(); i++)      //原来某些节点和y之间有边，现在他们和x之间有边了
	{
		bool flag = 1;
		ynei = points[y].edges[i];
		for(int j=0; j<points[ynei].edges.size(); j++)
			if(points[ynei].edges[j] == x)
			{
				flag = 0;
				break;
			}
		if(flag)
		for(int j=0; j<points[ynei].edges.size(); j++)
			if(points[ynei].edges[j] == y)
				points[ynei].edges[j] = x;
	}
	for(int i=0; i<points[y].edges.size(); i++)          //原来y会和某些节点之间有边，现在x和这些节点之间也有边了
	{
		bool equal = false;
		ynei = points[y].edges[i];
		for(int j=0; j<points[x].edges.size(); j++)
			if(ynei == points[x].edges[j]){
				equal = true;
				break;  }
		if(!equal && points[ynei].vexist == true)
			points[x].build_edges(ynei);
	}
	points[y].edges.clear();
}

void Mesh::change_Qofx(int x)         //重新计算相关节点的Q
{
	points[x].Q.mat_init();                                 //重新计算x周围面的参数
	for(int i=0; i<points[x].planes.size(); i++)
		if(faces[ points[x].planes[i] ].fexist == true)
			calculate_args(faces[points[x].planes[i]]);

	for(int i=0; i<points[x].planes.size(); i++)                        //计算x本身的Q矩阵
		if(faces[ points[x].planes[i] ].fexist == true) 
			points[x].add_mat(faces[ points[x].planes[i] ].Qf);
	int pointnum;
	for(int i=0; i<points[x].edges.size(); i++)                    //计算x周围所有点的Q矩阵
	{
		pointnum = points[x].edges[i];
		points[ pointnum ].Q.mat_init();
		for(int j=0; j<points[pointnum].planes.size(); j++)
			if(faces[ points[pointnum].planes[j]].fexist == true)
				points[ pointnum ].add_mat(faces[ points[pointnum].planes[j]].Qf);
	}	
}

void Mesh::change_pairqueue(int x)                    //改变优先队列
{
	points[x].vchange = current_change;               //先修改变化的节点的vchange值
	for(int i=0; i<points[x].edges.size(); i++)
	{
		if(points[points[x].edges[i]].vexist == true)         //有一些边需要重新插入优先队列
		{
			Pair temp(x , points[x].edges[i]);
			Vec3f mid = (points[x].num + points[points[x].edges[i]].num) / 2;
			temp.set_middle(mid);
			temp.calculate_delta(points[x] , points[points[x].edges[i]]);
			temp.pchange = points[x].vchange + points[ points[x].edges[i] ].vchange;      //最新插入的边pchange值都等于两个顶点的vchange值的和
												//过去的那些边，实际上应该被覆盖掉的那些，pchange < vchange(v1) + vchange(v2) 之后会被淘汰掉
			pairqueue.push(temp);
		}
	}
}

void Mesh::simplify_one()           //单独的一次化简过程
{
	Pair del(0,0);
	while(true)
	{
		del = pairqueue.top();            //先从优先级队列中取出一个出来
		pairqueue.pop();
		if(points[del.v1].vexist && points[del.v2].vexist && (del.v1 != del.v2) && del.pchange == (points[del.v1].vchange + points[del.v2].vchange))
			break;                    //不能是有某一个点不存在了，也不能是这个edge已经收缩成一个点了，
									  //而且还要是最新更新过的，不能是pchage < vchange(v1) + vchange(v2)的那种
	}
	int x = del.v1 , y = del.v2;
	Vec3f mid = del.middle_point;
	points[x].num = points[y].num = mid;    //两个点都取中点
	delete_faces(x , y);                    //删除相关的面
	change_neighbours(x , y);               //改变连结关系
	change_Qofx(x);                         //改变矩阵
	change_pairqueue(x);                    //修改优先级队列
	for(int i=0; i<points[x].edges.size(); i++)
		change_pairqueue(points[x].edges[i]);
}

void Mesh::rebuild()                  //重新构造一遍每一个节点的edges和planes，把里面那些已经不存在的节点和面都删除掉
{
	vector<int> e , p;
	for(int i=0; i<points.size(); i++)
		if(points[i].vexist == true)
		{
			e = points[i].edges;
			points[i].edges.clear();
			for(int j=0; j<e.size(); j++)
				if(points[e[j]].vexist == true)
					points[i].edges.push_back(e[j]);

			p = points[i].planes;
			points[i].planes.clear();
			for(int j=0; j<p.size(); j++)
				if(faces[p[j]].fexist == true)
					points[i].planes.push_back(p[j]);
		}
}

void Mesh::begin_simplify()             //简化过程
{
	int counter = 0;
	build_pairqueue();	
	while(fsize > finalsize)
	{
		counter++;
		current_change++;
		simplify_one();

		if(counter == 2000)   //如果已经做了2000次单独的删除操作，那么edges和planes里面就可能已经积蓄了一些不存在的点或者面了，就要重新rebuild一次
		{
			counter = 0;
			rebuild();
		}
	}
}

void Mesh::get_answer(const char* file_name)            //得到结果，写入obj文件
{
	int verticsize = points.size();
	int* map = new int [verticsize];
	reshape_answer(map);
	ofstream fout(file_name);
	fout<<"# "<<vsize<<" "<<fsize<<endl;
	for(int i=0; i<points.size(); i++)
		if(points[i].vexist)
			fout<<"v "<<points[i].num.x<<" "<<points[i].num.y<<" "<<points[i].num.z<<endl;
	for(int i=0; i<faces.size(); i++)
		if(faces[i].fexist)
			fout<<"f "<<map[faces[i].v0]<<" "<<map[faces[i].v1]<<" "<<map[faces[i].v2]<<endl;
	fout.close();
}

void Mesh::reshape_answer(int* map)           //重新整合结果，把那些删掉的节点/面都跳过
{
	int verticsize = points.size();
	int counter = 1;
	for(int i=0; i<verticsize; i++)
	{
		if(points[i].vexist == false )
			map[i] = -1;
		else
		{
			map[i] = counter;
			counter++;
		}
	}
}

void Mesh::test()           //测试接口
{
	rebuild();
	for(int i=0; i<points.size(); i++)
		if(points[i].vexist == true)
			cout<<i<<" "<<points[i].edges.size()<<" "<<points[i].planes.size()<<endl;
			
		
	for(int i=0; i<points.size(); i++)		
		if(points[i].vexist == true)
		{
			cout<<i<<":"<<endl;
			for(int j=0; j<points[i].edges.size(); j++)
				cout<<points[i].edges[j]<<" "<<points[points[i].edges[j]].vexist<<" ";
			cout<<endl;
			break;
		}
	cout<<endl;
	
}
