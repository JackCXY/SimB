#pragma once


#include <stdio.h>
//#include <tchar.h>
#include <vector>
#include <map>
#include<time.h>
#include<string>


using namespace std;

typedef int *   INT_P;

typedef struct timeval Time;

typedef vector<int> LISTINT;
typedef vector<int>::iterator  ListIntIter;

typedef map<int, int> MAP_INT;
typedef map<int, int> ::iterator MAP_INT_Iter;

typedef map<int, bool> MAP_IB;
typedef map<int, bool> ::iterator MAP_IB_Iter;

typedef vector<INT_P> INT_P_vc;//

typedef vector<MAP_INT *> LIST_MAP_INT_P;
typedef vector<MAP_IB *> LIST_MAP_IB_P;
typedef vector<LIST_MAP_IB_P *> LIST_MAP_IB_P_vc;

typedef map<string,int>MAP_Str;
typedef map<string,int>::iterator MAP_Str_Iter;



typedef map<int, INT_P_vc> MAP_INT_P_vc;
typedef map<int, INT_P_vc> ::iterator MAP_INT_P_vc_Iter;

typedef vector<LISTINT> LISTINT_vc;//

typedef vector<LISTINT *> LIST_INT_P;


typedef map<int, LISTINT> MAP_LISTINT;
typedef map<int, LISTINT> ::iterator MAP_LISTINT_Iter;

typedef map<int, INT_P> MAP_INT_P;
typedef map<int, INT_P> ::iterator MAP_INT_P_Iter;

typedef struct Graph
{
	//INT_P vertexLabel;// the label of each vertex
	//INT_P neiSize;	  // the neighbor size of each vertex
	INT_P * neighbor;// the neighbor list of each vertex
	INT_P * edgeLabel; //the label of edges that incident to each vertex

}Graph;

/*
typedef struct Node
{
	int i,j;//该node坐标
	bool selected;//是否被选
	bool visited;
	INT_P neighbor;//邻居id
	int curSize;// 当前该点覆盖的路径数
	LISTINT neighbor_list;//路径的id
	
}Node;
*/

typedef map<string, bool> MAP_str_bool;
typedef map<string, bool> ::iterator MAP_str_bool_Iter;
