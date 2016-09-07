#pragma once

#include "head.h"
//#include <tchar.h>
#include <sstream>
#include <cstring>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define MAXN 10000

#define maxChildren 10

class GraphReader
{
public:
	int tau;//threshold

	int GN; //number of database graphs
	INT_P vd, ed;//number of vertices and edges of database graphs
	Graph * graph;//database graphs

	INT_P * graphVertexId;//[graphId][vertexId]=the position in the mapping
	INT_P * graphVertexLabel;// [graphId][pos]
	INT_P * graphVertexNeiSize;// [graphId][position]
	INT_P * * graphVertexNeighbor;//[graphId][position][]
	INT_P * * graphEdgeLabel;//[graphId][pos][]

	INT_P * graphPos;//[graphId][pos]=vertexId

	int vq,eq;//number of vertices and edges of the query graph
	INT_P QVertexLabel;//[pos]=label
	INT_P QVertexNeiSize;//[pos]=neighbor size
	INT_P * QVertexNeighbor;// [pos][]
	INT_P * QEdgeLabel;//[pos][]

	INT_P QVertexId;///[vertexId]=the position of vertex
	INT_P QPos;//[pos]=vertexId

	//MAP_INT QEdgeId;//[v1v2] = edgeId,  v1*200+v2 (v1<v2)
	//INT_P QEdgeIdLabel;//[edgeId] = edge label;

	bool * QVertexUsed;//[vertexId]= whether the vertex has been used
	bool * * QEdgeUsed;//[vertexId][vertexId] = whether the edge has been used

	bool * QVertexVisited;//[vertexId],  but note that when they are used in graphEditDistance() and LocalFilter(), they are [pos_i]
	bool * * QEdgeVisited;//[vertexId][vertexId] but note that when they are used in graphEditDistance() and LocalFilter(), they are [pos_i][pos_i]

	bool * GVertexVisited;//[vertexId]
	bool * * GEdgeVisited;//[vertexId][vertexId]

	
	//statistics for database graphs
	MAP_INT * graphStatLabel;//[graphId]= the map for label where key=label, value=the position for this label in Statistic
	MAP_IB * * graphStatNeiVertex;//[graphId][]=label map of neighbor vertex, key = label of neighbor vertex, value = 0;
	MAP_IB * * graphStatNeiEdge;  //[graphId][]=label map of neighbor edge, key = label of neighbor edge, value=0;

	MAP_IB * graphContainEdgeLabel;//[graphId] = the map for edge label in graphId, key = edgeLabel, value = 0

	
	INT_P match_q;
	INT_P match_d;

	INT_P match_q_edgeLabel;//the corresponding position stores the edge label if the the corresponding position is edge
                            //if corresponding [pos] in match_q is vertex, [pos] stores the corresponding edge label

	INT_P match_q_pre;// stores the incident vertex to the element(vertex or edge) in [pos]
	INT_P match_d_pre;

	INT_P match_pos;//[vertexId]  stores the position in match_q of vertexId

	//int cur_f;//current first position in match_q is to be filled
	int cur_f_global;
	int cur_need;//cur_need: the current vertex pair need to be verified

	int match_last;// the last position to be verified;

	void  DGraphReader(char fileName_G []);
	void  QGraphReader(char fileName_Q []);

	void outputDGraph();
	void outputQueryGraph();

	int GlobalFilter(int graphId);
	void graphStatistic();

	bool partition(int graphId);
	bool match(int graphId, int max_block_size, int cur_f);

	int graphEditDistance(int graphId);
	void breadthFirstFilling(int vn, int * arr, int * arr_pos, int * VertexId, int * VertexNeiSize, int ** VertexNeighbor, bool * VertexVisited);
	
	int LocalFilter(int graphId);
	int ged_gx_compute(LISTINT & matchLS_d, int * arr_pos, int neiSize, int * Neighbor, int *VertexId, int *EdgeLabel, bool * VertexVisited);
	
	int greedyMaximumMatch(int graphId);


	//int findMaximalCliqueMain(LISTINT * Dvertex, int v_z, LISTINT & maximumClique);
	//int findMaximalClique(LISTINT * Dvertex, int v_z, int current_i, LISTINT & maximalClique, INT_P vertexCount, LISTINT & prevCandidates, LISTINT & prevSub, LISTINT &maximumClique, int cur_size);

	int mcs_wangdong(int graphId);


	//for index
	INT_P * graphBranchRoot;//[graphId][brachPos]=Rootlabel
	INT_P * graphBranchCode;//[graphId][branchPos]=branch code
	INT_P * graphBranchCount;//[graphId][branchPos]=branch count
	INT_P	graphBranchSize;//[graphId]=branch size

	INT_P QBranchRoot;//[brachPos]=Rootlabel
	INT_P QBranchCode;
	INT_P QBranchCount;
	int QBranchSize;

	
	MAP_Str * map_branch;//[rootLabel]=map;    the key of map=edgeLabel+" "+edgeLabel(branch);   the value of map=branch code

	//for tree
	/*
	INT_P   nodeParent;//[nodeId]=parent node id
	INT_P * nodeChildren;//[nodeId][childPos]=children id

	INT_P * nodeBranchRoot;//[nodeId][branchPos]=Rootlabel
	INT_P * nodeBranchCode;//[nodeId][branchPos]=branch code
	INT_P * nodeBranchCount;//[nodeId][branchPos]=branch count
	INT_P	nodeBranchSize;//[nodeId]=branch size, considering all the branches with same root label as one
	*/
	LISTINT nodeParent;
	LIST_INT_P nodeChildren;

	LIST_INT_P nodeBranchRoot;
	LIST_INT_P nodeBranchCode;
	LIST_INT_P nodeBranchCount;
	//LISTINT nodeBranchSize;
	LISTINT nodeSize;//[nodeId]=branch size, considering m branches with same root label as m

	void DGraphReaderIndex(char fileName_G []);
	void QGraphReaderIndex(char fileName_Q []);

	void BuildInex(char fileName_G []);
	void QIndex(char fileName_Q []);

	void BuildTree();
	int branchDistance(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, INT_P branchRoot_b, 
		INT_P branchCode_b, INT_P branchCount_b, int branchSize_b);//aSize = vertex number

	int branchDistanceList(int aSize, LISTINT & branchRoot_a, LISTINT & branchCode_a, LISTINT & branchCount_a, int branchSize_a, int bSize, LISTINT & branchRoot_b, 
		LISTINT & branchCode_b, LISTINT & branchCount_b, int branchSize_b);

	int branchDistanceINT_List(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, LISTINT & branchRoot_b, 
		LISTINT & branchCode_b, LISTINT & branchCount_b, int branchSize_b);

	int directedDistanceINT_List(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, LISTINT & branchRoot_b,
		LISTINT & branchCode_b, LISTINT & branchCount_b, int branchSize_b);


	LISTINT rootNode;
	int score[ maxChildren+1 ][ maxChildren+1];

	int depth;
	int depth2;
	int filter_branch;

	void insert(int g_i);
	void split(int node);
	void splitMul(int node);
	void splitMul_sub(int node);
	int binaryInsert(int left, int right, LISTINT &branchRoot, LISTINT &branchCode, LISTINT &branchCount, int label, int branch_id, int count);//return the index position should be inserted

	void searchIndex( LISTINT * children, 	LISTINT & result );
	void searchNode(LISTINT * children, int nodeId, int g_i);//return the nodeId contains graph g_i


	//for index(partition)
	LIST_MAP_INT_P nodeStatLabel;//[nodeId]= the map for label where key=label, value=the position for this label in Statistic
	LIST_MAP_IB_P  nodeStatNeiVertex;//[nodeId][]=label map of neighbor vertex, key = label of neighbor vertex, value = 0;
	LIST_MAP_IB_P  nodeStatNeiEdge;  //[nodeId][]=label map of neighbor edge, key = label of neighbor edge, value=0;

	LIST_MAP_IB_P nodeContainEdgeLabel;//[nodeId] = the map for edge label in graphId, key = edgeLabel, value = 0

	LISTINT rootNode_Partition;
	LISTINT nodeParent_Partition;
	LIST_INT_P nodeChildren_Partition;

	void BuildTree_Partition();
	double jaccardPartition(MAP_INT * statLabel_a, MAP_IB * statNeiVertex_a, MAP_IB *statNeiEdge_a, MAP_IB * containEdgeLabel_a, 
							MAP_INT * statLabel_b, MAP_IB * statNeiVertex_b, MAP_IB *statNeiEdge_b, MAP_IB * containEdgeLabel_b);

	double score_partiton[ maxChildren+1 ][ maxChildren+1];
	void insert_Partition(int g_i);
	void combine(int g_i, int nodeId);

	void splitMul_Partition(int node);
	void splitMul_sub_Partition(int node);

	void searchNode_Partition(LISTINT * children, int nodeId, int g_i);
	void searchIndex_Partition( LISTINT * children, 	LISTINT & result );
	int  distanceFisrtsThreeSteps(int node);
	int filter_partition;


	int T;



	MAP_INT_P DLabelMap;//因为不需要遍历，所以没有把key置成INT即数组的形式
	INT_P QLabel;
	
	LISTINT resultList;

	INT_P *  q_neighbor_label;
	INT_P *  q_neighbor_label_count;


};
