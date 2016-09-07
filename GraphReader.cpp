
#include "GraphReader.h"


//#include <sys/time.h>


void GraphReader::DGraphReader(char fileName_G [])
{
	FILE *stream = fopen( fileName_G, "r" ); 
  
	fscanf( stream, "%d\n",&GN);

	
	vd = new int[GN];
	ed = new int[GN];

	graphVertexId = new INT_P[GN];
	graphVertexLabel = new INT_P[GN];
	graphVertexNeiSize = new INT_P[GN];
	graphVertexNeighbor = new INT_P *[GN];
	graphEdgeLabel = new INT_P *[GN];

	graphPos = new INT_P[GN];

	graphStatLabel = new MAP_INT[GN];
	graphContainEdgeLabel = new MAP_IB[GN];

	char ch1;
	int v1,v2,v3;


	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter map_ib_iter;
	for(int g_i=0; g_i<GN; g_i++)
	{
		fscanf(stream, "%c %d %d %d\n", &ch1, &v3,&v1, &v2);

		graphVertexLabel[g_i] = new int[v1];
		graphVertexNeiSize[g_i]= new int[v1];
		
		graphVertexNeighbor[g_i] = new INT_P[v1];
		graphEdgeLabel[g_i] = new INT_P[v1];

		graphVertexId[g_i] = new int[v1];
		graphPos[g_i] = new int[v1];

		vd[g_i] = v1;
		ed[g_i] = v2;
		
		LISTINT * nei_list = new LISTINT[v1];
		LISTINT * neiLabel_list = new LISTINT[v1];

		int position = 0;
		for(int j=0; j<vd[g_i]; j++)
		{
			fscanf(stream, "%d %d\n", &v1, &v2);
			graphVertexLabel[g_i][v1] = v2;
	
			graphPos[g_i][j] = v1;
			graphVertexId[g_i][v1] = j;

			map_int_iter  = graphStatLabel[g_i].find(v2);
			if(map_int_iter == graphStatLabel[g_i].end())
			{
				graphStatLabel[g_i].insert(pair<int,int>(v2, position));
				position++;
			}
		

		}//read vertices end

		
		for(int j=0; j<ed[g_i]; j++)//we assume that edges are undirected,the precondition is that the input data have completed the removal of the duplicates
		{
			fscanf(stream, "%d %d %d\n", & ch1, &v1, &v2, &v3);
			nei_list[v1].push_back(v2);
			nei_list[v2].push_back(v1);
			
			neiLabel_list[v1].push_back(v3);
			neiLabel_list[v2].push_back(v3);

			map_ib_iter = graphContainEdgeLabel[g_i].find(v3);
			if(map_ib_iter == graphContainEdgeLabel[g_i].end())
			{
				graphContainEdgeLabel[g_i].insert(pair<int,bool>(v3,0));
			}

			// sort the neighbor of each vertex according to its edge labels
			int sort_i;
			for( sort_i = neiLabel_list[v1].size()-1; sort_i>0; sort_i--)
			{
				if(v3 < neiLabel_list[v1][ sort_i-1 ])
				{
					neiLabel_list[v1][sort_i] = neiLabel_list[v1][ sort_i-1 ];
					nei_list[v1][sort_i] = nei_list[v1][ sort_i-1 ];
				}
				else
				{
					neiLabel_list[v1][sort_i] = v3;
					nei_list[v1][sort_i] = v2;
					break;
				}
			}
			
			if(sort_i == 0)
			{
				neiLabel_list[v1][0] = v3;
				nei_list[v1][0] = v2;
			}

			for( sort_i = neiLabel_list[v2].size()-1; sort_i>0; sort_i--)
			{
				if(v3 < neiLabel_list[v2][ sort_i-1 ])
				{
					neiLabel_list[v2][sort_i] = neiLabel_list[v2][ sort_i-1 ];
					nei_list[v2][sort_i] = nei_list[v2][ sort_i-1 ];
				}
				else
				{
					neiLabel_list[v2][sort_i] = v3;
					nei_list[v2][sort_i] = v1;
					break;
				}
			}
			
			if(sort_i == 0)
			{
				neiLabel_list[v2][0] = v3;
				nei_list[v2][0] = v1;
			}
			//sort end
			
		}// read edges end

		//vector —— array
		int nei_size;
		for(int v_i=0; v_i<vd[g_i]; v_i++)
		{
			nei_size = nei_list[v_i].size();
			graphVertexNeiSize[g_i][v_i] = nei_size;

			graphVertexNeighbor[g_i][v_i] = new int[ nei_size ];
			graphEdgeLabel[g_i][v_i] = new int[ nei_size ];

			for(int nei_i=0; nei_i<nei_size; nei_i++)
			{
				graphVertexNeighbor[g_i][v_i][nei_i] = nei_list[v_i][nei_i];
				graphEdgeLabel[g_i][v_i][nei_i] = neiLabel_list[v_i][nei_i];
			}

			nei_list[v_i].clear();
			neiLabel_list[v_i].clear();

		}
		
		delete[] nei_list;
		delete[] neiLabel_list;
	
		fscanf(stream, "\n");
	}//read graphs end

	fclose(stream);

	graphStatistic();
	//getchar();
	//getchar();
	//for global filter
	for(int g_i=0; g_i<GN; g_i++)
	{
		int vd_g = vd[g_i];
		for(int v_i=vd_g-1; v_i>0; v_i--)
		{
			int max = graphVertexLabel[g_i][v_i];
			int max_i = v_i;

			for(int v_j=0; v_j<v_i; v_j++)
			{
				int label_j = graphVertexLabel[g_i][v_j];
				if( max < label_j)
				{
					max = label_j;
					max_i = v_j;
				}
				else if(max == label_j)
				{
					int nei_size_max = graphVertexNeiSize[g_i][max_i];
					int nei_size_j = graphVertexNeiSize[g_i][v_j];
					int nei_i, nei_j;
					for( nei_i=0, nei_j=0; nei_i<nei_size_max && nei_j<nei_size_j; )
					{
						if(graphEdgeLabel[g_i][max_i][nei_i] < graphEdgeLabel[g_i][v_j][nei_j])
						{
							max_i = v_j;
							break;
						}
						else if(graphEdgeLabel[g_i][max_i][nei_i] == graphEdgeLabel[g_i][v_j][nei_j])
						{
							nei_i++;
							nei_j++;
						}
						else
						{
							break;
						}
					}

					if(nei_i == nei_size_max && nei_size_max<nei_size_j )
					{
						max_i = v_j;
					}
				}
			}

			graphVertexLabel[g_i][max_i] =  graphVertexLabel[g_i][v_i];
			graphVertexLabel[g_i][v_i] = max;

			INT_P temp = graphVertexNeighbor[g_i][max_i];
			graphVertexNeighbor[g_i][max_i] = graphVertexNeighbor[g_i][v_i];
			graphVertexNeighbor[g_i][v_i] = temp;
			
			temp = graphEdgeLabel[g_i][max_i];
			graphEdgeLabel[g_i][max_i] = graphEdgeLabel[g_i][v_i];
			graphEdgeLabel[g_i][v_i] = temp;

			int temp_size = graphVertexNeiSize[g_i][max_i];
			graphVertexNeiSize[g_i][max_i] = graphVertexNeiSize[g_i][v_i];
			graphVertexNeiSize[g_i][v_i] = temp_size;

			int temp_id = graphPos[g_i][max_i];
			graphPos[g_i][max_i] = graphPos[g_i][v_i];
			graphPos[g_i][v_i] = temp_id;

			graphVertexId[g_i][temp_id] = v_i;
			temp_id = graphPos[g_i][max_i];
			graphVertexId[g_i][temp_id] = max_i;

		}

		/*

		for(int v_i=0; v_i<vd_g; v_i++)
		{
			printf("%d (", graphVertexLabel[g_i][v_i]);

			for(int nei_i=0; nei_i<graphVertexNeiSize[g_i][v_i]; nei_i++)
			{
				printf("%d ", graphEdgeLabel[g_i][v_i][nei_i]);
			}

			printf(")\n");
		}

		printf("\n");
		
		*/

		delete[] graphPos[g_i];
	}//global filter end
	
	delete [] graphPos;
	
    
	
	//memset(Dclique_size, 0, sizeof(int) * vd);常用操作

	//maximalClique_list.clear();//常用操作

	//delete [] lable_bool;//常用操作

	//memset(Dvisited, -1, sizeof(int) * vd);//常用操作

}// end DGraphReader


void GraphReader::QGraphReader(char fileName_Q [])//QGraphReader start
{
	
	FILE *stream = fopen( fileName_Q, "r" ); 

	char ch1;//每行的第一个字符

	fscanf( stream, "%c	%d	%d\n", &ch1,&vq,&eq);
	
	QVertexLabel = new int[vq];
	QVertexNeiSize = new int[vq];
	QVertexNeighbor = new INT_P[vq];
	QEdgeLabel = new INT_P[vq];
	QVertexId = new int[vq];
	QPos = new int[vq];

	QVertexUsed = new bool[vq];
	QEdgeUsed = new bool *[vq];


	int v1,v2,v3;

	//读取顶点的label
	for(int v_i=0; v_i <vq; v_i++)//此处是顶点的label
	{
		fscanf( stream, "%c %d %d\n", &ch1, &v1, &v2);
              
		QVertexLabel[v1]=v2;//此时v2是Label  //i----v1
		QPos[v_i] = v1;
		QVertexId[v1] = v_i;

		QEdgeUsed[v_i] = new bool[vq];
		//QEdgeVisited[v_i] = new bool [vq];
	}

	LISTINT * nei_list = new LISTINT[vq];
	LISTINT * neiLabel_list = new LISTINT[vq];

	//QEdgeIdLabel = new int[eq];

	for(int e_i=0; e_i<eq; e_i++)//we assume that edges are undirected,the precondition is that the input data have completed the removal of the duplicates
	{
		fscanf(stream, "%c %d %d %d\n", & ch1, &v1, &v2, &v3);
		nei_list[v1].push_back(v2);
		nei_list[v2].push_back(v1);
			
		neiLabel_list[v1].push_back(v3);
		neiLabel_list[v2].push_back(v3);

		/*
		int key=0;
		if(v1<v2)
		{
			key = v1*200+v2;
		}
		else
		{
			key = v2*200+v1;
		}

		//QEdgeId.insert(pair<int,int>(key,e_i));

		//QEdgeIdLabel[e_i] = v3;

		*/

		// sort the neighbor of each vertex according to its edge labels
		int sort_i;
		for( sort_i= neiLabel_list[v1].size()-1; sort_i>0; sort_i--)
		{
			if(v3 < neiLabel_list[v1][ sort_i-1 ])
			{
				neiLabel_list[v1][sort_i] = neiLabel_list[v1][ sort_i-1 ];
				nei_list[v1][sort_i] = nei_list[v1][ sort_i-1 ];
			}
			else
			{
				neiLabel_list[v1][sort_i] = v3;
				nei_list[v1][sort_i] = v2;
				break;
			}
		}
		if(sort_i == 0)
		{
			neiLabel_list[v1][0] = v3;
			nei_list[v1][0] = v2;
		}
			
		for( sort_i = neiLabel_list[v2].size()-1; sort_i>0; sort_i--)
		{
			if(v3 < neiLabel_list[v2][ sort_i-1 ])
			{
				neiLabel_list[v2][sort_i] = neiLabel_list[v2][ sort_i-1 ];
				nei_list[v2][sort_i] = nei_list[v2][ sort_i-1 ];
			}
			else
			{
				neiLabel_list[v2][sort_i] = v3;
				nei_list[v2][sort_i] = v1;
				break;
			}
		}

		if(sort_i == 0)
		{
			neiLabel_list[v2][0] = v3;
			nei_list[v2][0] = v1;
		}
		//sort end
			
	}// read edges end

	//vector —— array
	int nei_size;
	for(int v_i=0; v_i<vq; v_i++)
	{
		nei_size = nei_list[v_i].size();
		QVertexNeiSize[v_i] = nei_size;

		QVertexNeighbor[v_i] = new int[ nei_size ];
		QEdgeLabel[v_i] = new int[ nei_size ];

		for(int nei_i=0; nei_i<nei_size; nei_i++)
		{
			QVertexNeighbor[v_i][nei_i] = nei_list[v_i][nei_i];
			QEdgeLabel[v_i][nei_i] = neiLabel_list[v_i][nei_i];
		}

		nei_list[v_i].clear();
		neiLabel_list[v_i].clear();

	}
	
	delete[] nei_list;
	delete[] neiLabel_list;

	fclose(stream);

	//graphStatistic();//statistic

	//for global filter
	for(int v_i=vq-1; v_i>0; v_i--)
	{
		int max = QVertexLabel[v_i];
		int max_i = v_i;

		for(int v_j=0; v_j<v_i; v_j++)
		{
			int label_j = QVertexLabel[v_j];
			if( max < label_j)
			{
				max = label_j;
				max_i = v_j;
			}
			else if(max == label_j)
			{
				int nei_size_max = QVertexNeiSize[max_i];
				int nei_size_j = QVertexNeiSize[v_j];
				int nei_i, nei_j;
				for( nei_i=0, nei_j=0; nei_i<nei_size_max && nei_j<nei_size_j; )
				{
					if(QEdgeLabel[max_i][nei_i] < QEdgeLabel[v_j][nei_j])
					{
						max_i = v_j;
						break;
					}
					else if(QEdgeLabel[max_i][nei_i] == QEdgeLabel[v_j][nei_j])
					{
						nei_i++;
						nei_j++;
					}
					else
					{
						break;
					}
				}

				if(nei_i == nei_size_max && nei_size_max<nei_size_j )
				{
					max_i = v_j;
				}
			}
		}

		QVertexLabel[max_i] =  QVertexLabel[v_i];
		QVertexLabel[v_i] = max;

		INT_P temp = QVertexNeighbor[max_i];
		QVertexNeighbor[max_i] = QVertexNeighbor[v_i];
		QVertexNeighbor[v_i] = temp;
			
		temp = QEdgeLabel[max_i];
		QEdgeLabel[max_i] = QEdgeLabel[v_i];
		QEdgeLabel[v_i] = temp;

		int temp_size = QVertexNeiSize[max_i];
		QVertexNeiSize[max_i] = QVertexNeiSize[v_i];
		QVertexNeiSize[v_i] = temp_size;

		int temp_id = QPos[max_i];
		QPos[max_i] = QPos[v_i];
		QPos[v_i] = temp_id;

		QVertexId[temp_id] = v_i;
		temp_id = QPos[max_i];
		QVertexId[temp_id] = max_i;

	}//global filter end

	printf("output query\n");
	for(int v_i=0; v_i<vq; v_i++)
	{
		printf("%d (", QVertexLabel[v_i]);

		for(int nei_i=0; nei_i<QVertexNeiSize[v_i]; nei_i++)
		{
			printf("%d ", QEdgeLabel[v_i][nei_i]);
		}

		printf(")\n");
	}

	printf("\n");

	
	delete [] QPos;


	
	//memset(Qvisited, -1, sizeof(int) * vq);

	//memset(Qyet, 0, sizeof(bool) * vq);
 
}//QGraphReader end


void GraphReader::graphStatistic()//this operation is done before golbal filter, so there is no need to use postition
{
	graphStatNeiVertex = new MAP_IB *[GN];
	graphStatNeiEdge = new MAP_IB *[GN];

	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter map_ib_iter;
	for(int g_i=0; g_i<GN; g_i++)//for1
	{
		int vd_i = vd[g_i];
		int map_size = graphStatLabel[g_i].size();
		graphStatNeiVertex[g_i] = new MAP_IB[map_size];
		graphStatNeiEdge[g_i] = new MAP_IB[map_size];

		int cur_pos;
		for(int v_i=0; v_i<vd_i; v_i++)//for2
		{
			int label_i = graphVertexLabel[g_i][v_i];

			map_int_iter = graphStatLabel[g_i].find(label_i);

			cur_pos = map_int_iter->second;
			
			int nei_size = graphVertexNeiSize[g_i][v_i];
			for(int nei_i=0; nei_i<nei_size; nei_i++)//for3
			{
				int nei_id = graphVertexNeighbor[g_i][v_i][nei_i];

				int nei_label = graphVertexLabel[g_i][nei_id];

				map_ib_iter = graphStatNeiVertex[g_i][cur_pos].find(nei_label);
				if(map_ib_iter == graphStatNeiVertex[g_i][cur_pos].end())
				{
					graphStatNeiVertex[g_i][cur_pos].insert(pair<int,bool>(nei_label,true));
				}


				nei_label = graphEdgeLabel[g_i][v_i][nei_i];

				map_ib_iter = graphStatNeiEdge[g_i][cur_pos].find(nei_label);
				if(map_ib_iter == graphStatNeiEdge[g_i][cur_pos].end())
				{
					graphStatNeiEdge[g_i][cur_pos].insert(pair<int,bool>(nei_label,true));
				}
			
			}//for3 end
			
		}//for2 end

		//output
		/*
		for(map_int_iter = graphStatLabel[g_i].begin(); map_int_iter!= graphStatLabel[g_i].end(); map_int_iter++)
		{	
			int label = map_int_iter->first;
			int pos = map_int_iter->second;

			printf("label: %d  NeiEdge(", label);
			for(map_ib_iter= graphStatNeiEdge[g_i][pos].begin(); map_ib_iter!= graphStatNeiEdge[g_i][pos].end(); map_ib_iter++)
			{
				printf(" %d ",map_ib_iter->first);
			}
			printf(")  NeiVertex(");

			for(map_ib_iter=graphStatNeiVertex[g_i][pos].begin(); map_ib_iter!= graphStatNeiVertex[g_i][pos].end(); map_ib_iter++)
			{
				printf(" %d ",map_ib_iter->first);
			
			}
			printf(")\n");

		}
		
		printf("\n");

		*/	

	}//for1 end

}


void GraphReader:: outputDGraph()
{

	printf("outputDGraph...\n");
	for(int graph_i=0; graph_i<GN; graph_i++)
	{
		printf("graph%d:\n",graph_i);
		for(int v_i=0; v_i<vd[ graph_i ]; v_i++)
		{
			int v_i_pos = graphVertexId[graph_i][v_i];

			printf("vertexId=%d vertexLabel=%d pos=%d\n",v_i, graphVertexLabel[graph_i][v_i_pos], v_i_pos);
		}

		for(int v_i=0; v_i<vd[graph_i]; v_i++)
		{
			int v_i_pos = graphVertexId[graph_i][v_i];
			for(int nei_i=0; nei_i<graphVertexNeiSize[graph_i][v_i_pos]; nei_i++)
			{
				printf("e %d %d %d\n", v_i, graphVertexNeighbor[graph_i][v_i_pos][nei_i], graphEdgeLabel[graph_i][v_i_pos][nei_i]);
			}
			
		}
			
	}

}


void GraphReader:: outputQueryGraph()
{

	printf("outputQueryGraph...\n");
	
	for(int v_i=0; v_i<vq; v_i++)
	{
		int v_i_pos = QVertexId[v_i];

		printf("vertxId=%d vertexLabel=%d pos=%d\n",v_i, QVertexLabel[v_i_pos], v_i_pos);
	}

	for(int v_i=0; v_i<vq; v_i++)
	{
		int v_i_pos = QVertexId[v_i];
		for(int nei_i=0; nei_i<QVertexNeiSize[v_i_pos]; nei_i++)
		{
			printf("e %d %d %d\n", v_i, QVertexNeighbor[v_i_pos][nei_i], QEdgeLabel[v_i_pos][nei_i]);
		}
			
	}

}


int GraphReader:: GlobalFilter(int graphId)
{
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	int v_d = vd[graphId];
	INT_P DVertexLabel = graphVertexLabel[graphId];
	INT_P DVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * DEdgeLabel = graphEdgeLabel[graphId];

	int label_d, label_q;

	int v_d_i=0,  v_q_i=0;

	if(v_d >= vq )//if-1
	{
		int last_label = -1;
		int passed_q = 0;
		int passed_d = 0;

		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )//for-1
		{
			 label_d = DVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 if(label_d == last_label)
				 {
					passed_d++;
					v_d_i++;
					continue;
				 }

				 count1++;
				 v_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				if(label_q == last_label)
				 {
					passed_q++;
					v_q_i++;
					continue;
				 }

				v_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {
				if(label_d != last_label)// if-1.1.1
				{
					if(passed_q <= passed_d)
					{
						count1 = count1 + passed_d - passed_q;
						count2 = count2 + passed_q;
					}
					else
					{
						count2 = count2 + passed_d;
					}
					
					passed_q = 0;
					passed_d = 0;

					last_label = label_d;
				}// if-1.1.1 end

				int nei_size_d = DVertexNeiSize[v_d_i];
				int nei_size_q = QVertexNeiSize[v_q_i];
				int nei_i_d=0, nei_i_q=0;
				for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; nei_i_d++, nei_i_q++)//for-1.1
				{
					int e_label_d = DEdgeLabel[v_d_i][nei_i_d];
					int e_label_q = QEdgeLabel[v_q_i][nei_i_q];
					if(e_label_d < e_label_q )
					{
						passed_d++;
						//count2++;
						v_d_i++;
						break;
					}
					else if(e_label_d > e_label_q )
					{
						passed_q++;
						v_q_i++;
						break;
					}
				}//for-1.1 end

				if(nei_i_d == nei_size_d && nei_i_q<nei_size_q )// if-1.1.2
				{
					passed_d++;
					//count2++;
					v_d_i++;
				}// if-1.1.2 middle
				else if(nei_i_d<nei_size_d && nei_i_q == nei_size_q)
				{
					passed_q++;
					v_q_i++;
				}// if-1.1.2 middle
				else if(nei_i_d==nei_size_d && nei_i_q == nei_size_q)
				{
					v_d_i++;
					v_q_i++;
				}//if-1.1.2 end

			 }// if-1.1 end
		}// for-1 end

		for( ; v_d_i<v_d; v_d_i++ )
		{
			label_d = DVertexLabel[v_d_i];
			if( label_d != last_label )
			{	
				break;
			}
			passed_d++;		
		}

		for( ; v_q_i<vq; v_q_i++)
		{
			label_q = QVertexLabel[v_q_i];
			if(  label_q != last_label )
			{
				break;
			}
			passed_q++;
		}

		if(passed_q <= passed_d)
		{
			count1 = count1 + passed_d - passed_q;
			count2 = count2 + passed_q;
		}
		else
		{
			count2 = count2 + passed_d;
		}

		count1 = count1 + v_d - v_d_i;

	}// if-1 middle
	else
	{
		int last_label = -1;
		int passed_q = 0;
		int passed_d = 0;

		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )
		{
			 label_d = DVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)
			 {	 
				 if(label_d == last_label)
				 {
					passed_d++;
					v_d_i++;
					continue;
				 }
				 v_d_i++;
				 continue;
			 }
			 else if(label_d > label_q)
			 {
				 if(label_q == last_label)
				 {
					passed_q++;
					v_q_i++;
					continue;
				 }
				 count1++;
				 v_q_i++;
				 continue;
			 }
			 else
			 {
				if(label_d != last_label)
				{
					if(passed_q >= passed_d)
					{
						count1 = count1 + passed_q - passed_d;
						count2 = count2 + passed_d;
					}
					else
					{
						count2 = count2 + passed_q;
					}
					
					passed_q = 0;
					passed_d = 0;

					last_label = label_d;
				}

				int nei_size_d = DVertexNeiSize[v_d_i];
				int nei_size_q = QVertexNeiSize[v_q_i];
				int nei_i_d=0, nei_i_q=0;
				for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; nei_i_d++, nei_i_q++)
				{
					int e_label_d = DEdgeLabel[v_d_i][nei_i_d];
					int e_label_q = QEdgeLabel[v_q_i][nei_i_q];
					if(e_label_d < e_label_q )
					{
						v_d_i++;
						passed_d++;
						break;
					}
					else if(e_label_d > e_label_q )
					{
						//count2++;
						passed_q++;
						v_q_i++;
						break;
					}
				}//for end
				if(nei_i_d == nei_size_d && nei_i_q<nei_size_q )
				{
					v_d_i++;
					passed_d++;
				}
				else if(nei_i_d<nei_size_d && nei_i_q == nei_size_q)
				{
					//count2++;
					passed_q++;
					v_q_i++;
				}
				else if(nei_i_d==nei_size_d && nei_i_q == nei_size_q)
				{
					v_d_i++;
					v_q_i++;
				}

			 }// else end
		}// for end

		for( ; v_d_i<v_d; v_d_i++ )
		{
			label_d = DVertexLabel[v_d_i];
			if( label_d != last_label )
			{	
				break;
			}
			passed_d++;		
		}

		for( ; v_q_i<vq; v_q_i++)
		{
			label_q = QVertexLabel[v_q_i];
			if(  label_q != last_label )
			{
				break;
			}
			passed_q++;
		}
		
		if(passed_q >= passed_d)
		{
			count1 = count1 + passed_q - passed_d;
			count2 = count2 + passed_d;
		}
		else
		{
			count2 = count2 + passed_q;
		}
					
		count1 = count1 + vq - v_q_i;
	}
	
	if(count2%2 == 0)
	{
		ged = count1+count2/2;
	}
	else
	{
		ged = count1+count2/2+1;
	}
	
	return ged;
}


bool GraphReader::partition(int graphId)
{
	int ged = 0;//graph edit distance

	int v_d = vd[graphId];

	INT_P DVertexId = graphVertexId[graphId];
	INT_P DVertexLabel = graphVertexLabel[graphId];
	INT_P DVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * DEdgeLabel = graphEdgeLabel[graphId];

	
	MAP_IB * GStatNeiVertex = graphStatNeiVertex[graphId];
	MAP_IB * GStatNeiEdge = graphStatNeiEdge[graphId];

	QVertexVisited = new bool[vq];
	QEdgeVisited = new bool *[vq];

	memset(QVertexUsed, 0, sizeof(bool) * vq);//initial

	memset(QVertexVisited, 0, sizeof(bool) * vq);

	for(int v_i=0; v_i<vq; v_i++)
	{
		memset(QEdgeUsed[v_i], 0, sizeof(bool)*vq);//initial

		QEdgeVisited[v_i] = new bool[vq];
		memset(QEdgeVisited[v_i], 0, sizeof(bool)*vq);//initial
	}


	int label_q;
	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter map_ib_iter;

	int count_used =0;
	//if(v_d < vq)//partition query ---- if-1
	{
		for(int v_i_q=0; v_i_q<vq; v_i_q++)//for-1
		{
			int pos_i_q = QVertexId[v_i_q];

			if(!QVertexUsed[v_i_q])//have not been used ---- if-1.1
			{
				label_q = QVertexLabel[pos_i_q];
				map_int_iter = graphStatLabel[graphId].find(label_q);
				if(map_int_iter == graphStatLabel[graphId].end())//if-1.1.1
				{
					ged++;
					QVertexUsed[v_i_q] = true;
					count_used++;
					continue;
				}
				else// if-1.1.1 middle
				{
					int v_label_pos_d = map_int_iter->second;

					int nei_size = QVertexNeiSize[pos_i_q];
					int nei_i = 0;
					for( nei_i=0; nei_i<nei_size; nei_i++)//for-1.1
					{
						int nei_id = QVertexNeighbor[pos_i_q][nei_i];

						if(!QEdgeUsed[v_i_q][nei_id])//if-1.1.1.1
						{
							int edge_label = QEdgeLabel[pos_i_q][nei_i];

							map_ib_iter = graphContainEdgeLabel[graphId].find(edge_label);
							if(map_ib_iter == graphContainEdgeLabel[graphId].end())
							{
								ged++;
								QEdgeUsed[v_i_q][nei_id] = true;
								QEdgeUsed[nei_id][v_i_q] = true;
								count_used++;
								continue;
							}

							map_ib_iter = GStatNeiEdge[v_label_pos_d].find(edge_label);
							if(map_ib_iter == GStatNeiEdge[v_label_pos_d].end())
							{
								QEdgeUsed[v_i_q][nei_id] = true;
								QEdgeUsed[nei_id][v_i_q] = true;

								QVertexUsed[v_i_q] = true;
								ged++;
								count_used = count_used + 2;
								break;
							}
							
						}//if-1.1.1.1 end

					}//for-1.1 end

					if(nei_i < nei_size)
					{
						continue;
					}

					for( nei_i=0; nei_i<nei_size; nei_i++)//for-1.2
					{
						int nei_id = QVertexNeighbor[pos_i_q][nei_i];

						int nei_pos = QVertexId[nei_id];

						if(!QVertexUsed[nei_id])
						{
							int nei_label = QVertexLabel[nei_pos];

							map_int_iter = graphStatLabel[graphId].find(nei_label);

							if(map_int_iter == graphStatLabel[graphId].end())
							{
								QVertexUsed[nei_id] = true;
								ged++;
								count_used++;
								continue;
							}

							map_ib_iter = GStatNeiVertex[v_label_pos_d].find(nei_label);

							if(map_ib_iter == GStatNeiVertex[v_label_pos_d].end())
							{
								QVertexUsed[nei_id] = true;
								QVertexUsed[v_i_q] = true;
								count_used = count_used + 3;
								ged++;
								break;
							}
						}

					}//for-1.2 end

					if(nei_i < nei_size)
					{
						continue;
					}

				}//if-1.1.1 end

			}//if-1.1 end 

		}//for-1 end

		//before here: meaning that we find a mismatch starting with the current vertex. 
		//following: deal with the case can not find mismatch.
		

		if(ged > tau)
		{
			return true;
		}

		if(  (vq+eq - count_used)  - (tau+1-ged) * 3  < 0 )
		{
			return false;
		}

		GVertexVisited = new bool[v_d];
		GEdgeVisited = new bool*[v_d];

		memset(GVertexVisited, 0, sizeof(bool)*v_d);
		for(int v_i_d=0; v_i_d<v_d; v_i_d++)
		{
			GEdgeVisited[v_i_d] = new bool[v_d];
			memset(GEdgeVisited[v_i_d],0,sizeof(bool)*v_d);
		}

		match_pos = new int[vq];

		for(int v_i_q=0; v_i_q<vq; v_i_q++)//for-2
		{
			if(QVertexUsed[v_i_q])//have been used
			{
				continue;
			}

			int yu = (vq+eq-count_used)%(tau + 1 - ged);
			int max_block_size = (vq+eq-count_used)/(tau + 1 - ged);
			if(yu != 0)
			{
				max_block_size++;
			}
			
			match_q = new int[max_block_size];
			match_d = new int[max_block_size];

			match_q_edgeLabel = new int[max_block_size];

			match_q_pre = new int[max_block_size];
			match_d_pre = new int[max_block_size];

			memset(match_q, -1, max_block_size*sizeof(int));
			memset(match_d, -1, max_block_size*sizeof(int));
			memset(match_q_edgeLabel, -1, max_block_size*sizeof(int));
			
			memset(match_q_pre, -1, max_block_size*sizeof(int));
			memset(match_d_pre, -1, max_block_size*sizeof(int));


			int pos_i_q = QVertexId[v_i_q];
			int label_q = QVertexLabel[pos_i_q];

			match_q[0] = v_i_q;

			match_pos[v_i_q] = 0;

			int nei_size_q = QVertexNeiSize[pos_i_q];

			int cur_f = 1;
			cur_f_global = 1;
			match_last = 1;

			for(int nei_i_q=0; nei_i_q<nei_size_q; nei_i_q++)//for-2.1
			{
				int nei_id_q = QVertexNeighbor[pos_i_q][nei_i_q];
				if(QEdgeUsed[v_i_q][nei_id_q])
				{
					continue;
				}

				if(cur_f < max_block_size)
				{
					match_q[cur_f] = nei_id_q*200+v_i_q;
					QEdgeVisited[v_i_q][nei_id_q] = true;
					QEdgeVisited[nei_id_q][v_i_q] = true;

					match_q_edgeLabel[cur_f] = QEdgeLabel[pos_i_q][nei_i_q];

					match_q_pre[cur_f] = v_i_q;

					cur_f++;
					cur_f_global++;
						 
				}
			}//for-2.1 end

			//int cur_f_local = cur_f;

			int v_i_d =0;
			for( ; v_i_d < v_d; v_i_d++)//for-2.2
			{
				int pos_i_d = DVertexId[v_i_d];

				int label_d = DVertexLabel[pos_i_d];

				if(label_d != label_q)
				{
					continue;
				}
				
				int nei_i_d =0;
				int e_i_q =1;
				
				int nei_size_d = DVertexNeiSize[pos_i_d];
				for(; nei_i_d<nei_size_d && e_i_q <cur_f;  )//for-2.2.1
				{
					int e_q = match_q_edgeLabel[e_i_q];
					int e_d = DEdgeLabel[pos_i_d][nei_i_d];

					if(e_q < e_d)
					{
						break;
					}
					else if(e_q == e_d)
					{
						nei_i_d++;
						e_i_q++;
					}
					else
					{
						nei_i_d++;
					}
				}//for-2.2.1 end

				if(e_i_q != cur_f)// the current v_i_d  is not suitable
				{
					continue;
				}

				GVertexVisited[v_i_d] = true;
				QVertexVisited[v_i_q] = true;
		
				match_d[0] = v_i_d;
				
				for(int pre_i=1; pre_i<cur_f; pre_i++)
				{
					match_d_pre[pre_i] = v_i_d;
				}
				
				cur_need  = 1;
				bool ismatch = match(graphId,max_block_size, cur_f);// 

				GVertexVisited[v_i_d] = false;
				QVertexVisited[v_i_q] = false;

				if(ismatch)
				{		
					count_used = count_used + cur_f_global;

					for(int cur_i=0; cur_i<cur_f_global; cur_i++)
					{
						int cur_real = match_q[cur_i];
						if(cur_real < vq)
						{
							QVertexUsed[ cur_real ] = true;
						}
						else
						{
							int va = cur_real / 200;
							int vb = cur_real % 200;
							QEdgeUsed[va][vb] = true;
							QEdgeUsed[vb][va] = true;

							QEdgeVisited[va][vb] = false;
							QEdgeVisited[vb][va] = false;
						}
					}

					break;
				}	

			}//for-2.2 end



			if(v_i_d == v_d)//mismatch
			{
				ged++;

				count_used = count_used + match_last +1;

				for(int cur_i=0; cur_i<= match_last; cur_i++)
				{
					int cur_real = match_q[cur_i];
					if(cur_real < vq)
					{
						QVertexUsed[ cur_real ] = true;
					}
					else
					{
						int va = cur_real / 200;
						int vb = cur_real % 200;
						QEdgeUsed[va][vb] = true;
						QEdgeUsed[vb][va] = true;

						QEdgeVisited[va][vb] = false;
						QEdgeVisited[vb][va] = false;
					}
				}

			}

			
			delete [] match_q;
			delete [] match_d;
			delete [] match_q_edgeLabel;

			delete [] match_q_pre;
			delete [] match_d_pre;

			if(ged > tau)
			{
				break;
			}

			if(  (vq+eq - count_used)  - (tau+1-ged) * 3  < 0 )
			{
				return false;
			}


		}//for-2 end

		delete [] match_pos;

		delete[] GVertexVisited;
		delete[] QVertexVisited;

		for(int v_i_d=0; v_i_d<v_d; v_i_d++)
		{
			delete[] GEdgeVisited[v_i_d];
		}

		for(int v_i_q=0; v_i_q<vq; v_i_q++)
		{
			delete[] QEdgeVisited[v_i_q];
		}

	}//if-1 end

	if(ged > tau)
	{
		return true;// have pruned
	}
	else
	{
		return false;//not pruned
	}
}


bool GraphReader:: match(int graphId, int max_block_size, int cur_f)
{
	
	if(cur_need > match_last)
	{
		match_last = cur_need;
	}

	if(cur_f > cur_f_global)
	{
		cur_f_global = cur_f;
	}
	

	if(cur_need >= cur_f)
	{
		return true;
	}


	
	int v_i_q = match_q_pre[cur_need];
	int v_i_d = match_d_pre[cur_need];

	int pos_i_q = QVertexId[v_i_q];
	int pos_i_d = graphVertexId[graphId][v_i_d];


	int cur_real = match_q[cur_need];

	if(cur_real >= vq)// current is an edge  ---- if-1
	{
		int va = cur_real / 200;
		int vb = cur_real % 200;

		if(!QVertexVisited[va])// if-1.1
		{
			if(!QVertexUsed[va] && cur_f < max_block_size)
			{
				match_q[cur_f] = va; 
				match_pos[va] = cur_f;
				match_q_edgeLabel[cur_f] = match_q_edgeLabel[cur_need];
				
				match_q_pre[cur_f] = v_i_q;
				match_d_pre[cur_f] = v_i_d;

				cur_f++;
			}
			

			QVertexVisited[va] = true;
			cur_need++;
			bool ismatch = match(graphId, max_block_size,cur_f);
			cur_need--;//可以不把cur_need作为全局变量，而将match函数的返回值改为int，即返回cur_need的值
			QVertexVisited[va] = false;

			return ismatch;

		}//if-1.1 middle
		else if(!QVertexVisited[vb])
		{
			if(!QVertexUsed[vb] && cur_f < max_block_size)
			{
				match_q[cur_f] = vb; 
				match_pos[vb] = cur_f;
				match_q_edgeLabel[cur_f] = match_q_edgeLabel[cur_need];

				match_q_pre[cur_f] = v_i_q;
				match_d_pre[cur_f] = v_i_d;

				cur_f++;
			}

			QVertexVisited[vb] = true;
			cur_need++;
			bool ismatch = match(graphId, max_block_size, cur_f);
			cur_need--;//可以不把cur_need作为全局变量，而将match函数的返回值改为int，即返回cur_need的值
			QVertexVisited[vb] = false;

			return ismatch;

		}//if-1.1 middle
		else
		{
			int edge_label = match_q_edgeLabel[cur_need];

			int pos_a = match_pos[va];
			int pos_b = match_pos[vb];

			int va_d = match_d[pos_a];
			int vb_d = match_d[pos_b];

			int va_d_pos = graphVertexId[graphId][va_d];

			int neiSize = graphVertexNeiSize[graphId][va_d_pos];
			for(int nei_i=0; nei_i<neiSize; nei_i++)
			{
				int nei_label = graphEdgeLabel[graphId][va_d_pos][nei_i];
				if(nei_label < edge_label)
				{
					continue;
				}
				else if(nei_label > edge_label)
				{
					return false;
				}
				else
				{
					int nei_id = graphVertexNeighbor[graphId][va_d_pos][nei_i];
					if(nei_id == vb_d)
					{

						cur_need++;
						bool ismatch = match(graphId, max_block_size, cur_f);
						cur_need--;

						return ismatch;
					}
				}
			}

			return false;

		}//if-1.1 end

	}//if-1 middle
	else
	{
		int cur_pos_q = QVertexId[cur_real];
		int cur_label_q = QVertexLabel[cur_pos_q];

		int nei_i_d = 0;
		int nei_size_d = graphVertexNeiSize[graphId][pos_i_d];

		
		int e_i_q = cur_f;
		
		int cur_nei_size_q = QVertexNeiSize[cur_pos_q];

		LISTINT visited_true;
		for(int cur_nei_i_q =0; cur_nei_i_q<cur_nei_size_q; cur_nei_i_q++)// for-1.1
		{
			int cur_nei_id_q = QVertexNeighbor[cur_pos_q][cur_nei_i_q];
			if(QEdgeUsed[cur_real][cur_nei_id_q] ||QEdgeVisited[cur_real][cur_nei_id_q] )
			{
				continue;
			}
			if(cur_f < max_block_size)
			{
				match_q[cur_f] = cur_real*200+cur_nei_id_q;
				QEdgeVisited[cur_real][cur_nei_id_q] = true;
				QEdgeVisited[cur_nei_id_q][cur_real] = true;

				visited_true.push_back(cur_real);
				visited_true.push_back(cur_nei_id_q);

				match_q_edgeLabel[cur_f] = QEdgeLabel[cur_pos_q][cur_nei_i_q];

				match_q_pre[cur_f] = cur_real;
				//match_d_pre[cur_f] = nei_id_d;

				cur_f++;
			}
			else
			{
				break;
			}

		}//for-1.1 end


		for( ; nei_i_d<nei_size_d; nei_i_d++)//for-1
		{
			int nei_id_d = graphVertexNeighbor[graphId][pos_i_d][nei_i_d];
			int nei_id_d_pos = graphVertexId[graphId][nei_id_d];
			int nei_label_d = graphVertexLabel[graphId][nei_id_d_pos];
			if(nei_label_d != cur_label_q)// if-1.2
			{
				continue;
			}// if-1.2 end
			
			
			if(GVertexVisited[nei_id_d])
			{
				continue;
			}
			
			int edge_label_d = graphEdgeLabel[graphId][pos_i_d][nei_i_d];
					
			int edge_label_q = match_q_edgeLabel[cur_need];

			if(edge_label_d != edge_label_q)// 
			{
				continue;
			}// 
						
			GEdgeVisited[nei_id_d][v_i_d] = true;
			GEdgeVisited[v_i_d][nei_id_d] = true;

			
			
			
			int num = 1;
			int e_i_q_i = e_i_q;

			int cur_nei_size_d = graphVertexNeiSize[graphId][nei_id_d_pos];
			int cur_nei_id_i =0;
			for( ; cur_nei_id_i<cur_nei_size_d && e_i_q_i < cur_f;  )// for-1.2
			{
				int nei_i_id_d_nei = graphVertexNeighbor[graphId][nei_id_d_pos][cur_nei_id_i];

				if(GEdgeVisited[nei_id_d][nei_i_id_d_nei])
				{
					cur_nei_id_i++;
					continue;
				}

				int e_q = match_q_edgeLabel[e_i_q_i];
				int e_d = graphEdgeLabel[graphId][nei_id_d_pos][cur_nei_id_i];

				if(e_q < e_d)
				{
					break;
				}
				else if(e_q == e_d)
				{
					match_d_pre[e_i_q_i] = nei_id_d;

					cur_nei_id_i++;
					e_i_q_i++;

					num++;
				}
				else
				{
					cur_nei_id_i++;
				}
			}//for-1.2 end

			if(cur_need + num > match_last)
			{
				match_last = cur_need + num;
			}

			if(e_i_q_i != cur_f)// the current nei_i_id_d  is not suitable
			{
				GEdgeVisited[nei_id_d][v_i_d] = false;
				GEdgeVisited[v_i_d][nei_id_d] = false;
				continue;
			}

			GVertexVisited[nei_id_d] = true;

			GEdgeVisited[v_i_d][nei_id_d] = true;
			GEdgeVisited[nei_id_d][v_i_d] = true;
	
			match_d[cur_need] = nei_id_d;

			cur_need ++;
			bool ismatch = match(graphId, max_block_size, cur_f);// 
			cur_need --;

			GVertexVisited[nei_id_d] = false;
			GEdgeVisited[v_i_d][nei_id_d] = false;
			GEdgeVisited[nei_id_d][v_i_d] = false;


			if(ismatch)
			{	
				return true;
			}
			

		}//for-1 end

		if(nei_i_d >= nei_size_d)
		{
			for(int t_i=0; t_i<visited_true.size(); )
			{
				int va = visited_true[t_i];
				t_i++;
				int vb = visited_true[t_i];
				t_i++;
				QEdgeVisited[va][vb] = false;
				QEdgeVisited[vb][va] = false;
			}
			return false;
		}

	}//if-1 end

}

int GraphReader::graphEditDistance(int graphId)
{
	int ged = 0;//graph edit distance

	int v_d = vd[graphId];

	int ged_gx = 0;
	int ged_hx1 = 0;
	int ged_hx2 = 0;

	INT_P GVertexId = graphVertexId[graphId];
	INT_P GVertexLabel = graphVertexLabel[graphId];
	INT_P GVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * GVertexNeighbor = graphVertexNeighbor[graphId];
	INT_P * GEdgeLabel = graphEdgeLabel[graphId];
	
	int length = 0;// the length of match pairs
	if(v_d <= vq)// when v_d==vq, we assume that v_d is larger and v_d is inner --- if-1
	{
		length = vq;

		INT_P arr_d = new int[length];//stores the vertex pos arry
		INT_P arr_d_pos = new int[length] ;//stores the the position of vertex in arr_q

		GVertexVisited = new bool[length];
		QVertexVisited = new bool[length];

		memset(GVertexVisited, 0, length*sizeof(bool));
		memset(QVertexVisited, 0, length*sizeof(bool));

		breadthFirstFilling( v_d, arr_d, arr_d_pos, GVertexId, GVertexNeiSize, GVertexNeighbor, GVertexVisited);
		
		memset(GVertexVisited, 0, length*sizeof(bool));

		for(int len_i=v_d; len_i<length; len_i++)
		{
			arr_d[ len_i ] = -1;
		}

		int label_d_0 = GVertexLabel[ 0 ];

		int neiSize_d = GVertexNeiSize[ 0 ];
		INT_P edge_d_0 = new int[neiSize_d];

		GVertexVisited[ 0 ] = true;//pos_d_0

		for(int nei_i_d=0; nei_i_d<neiSize_d; nei_i_d++)
		{
			edge_d_0[ nei_i_d ] = GEdgeLabel[0][nei_i_d];
		}

		int arr_id = 0;

		LISTINT arr_ls;// stores the arr id
		LISTINT currentEstimateValue;//the list that consists of estimated ged value

		INT_P_vc arrP_vc;//stores the arr pointer
		INT_P_vc arrP_pos_vc;
		LISTINT currentEditDist;//the current edit distance having been computed
		LISTINT currentLen;//the current length having been computed

		for(int pos_i_q=0; pos_i_q<vq; pos_i_q++)// for-1
		{
			INT_P arr_q = new int[1];
			arr_q[ 0 ] = pos_i_q;

			INT_P arr_q_pos = new int[length];
			arr_q_pos[ pos_i_q ] = 0;

			int label_q = QVertexLabel[ pos_i_q ];

			if(label_d_0 == label_q)
			{
				ged = 0;
			}
			else
			{
				ged = 1;
			}
			currentLen.push_back(1);
			currentEditDist.push_back(ged);
			
			ged_hx1 = 0;//the first kind of estimation
		
			int nei_i_d = 0, nei_i_q = 0; 
			int neiSize_q  = QVertexNeiSize[ pos_i_q ];
			for( ; nei_i_d<neiSize_d && nei_i_q<neiSize_q; )// for-1.1
			{
				int e_label_d = edge_d_0[nei_i_d];
				int e_label_q = QEdgeLabel[ pos_i_q ][nei_i_q];

				if(e_label_d < e_label_q)
				{
					nei_i_d++;
				}
				else if(e_label_d == e_label_q)
				{
					ged_hx1++;
					nei_i_d++;
					nei_i_q++;
				}
				else
				{
					nei_i_q++;
				}

			}// for-1.1 end

			if(neiSize_d >= neiSize_q)
			{
				ged_hx1 = neiSize_d - ged_hx1;
			}
			else
			{
				ged_hx1 = neiSize_q - ged_hx1;
			}

			//estimate the second kind cost
			QVertexVisited[ pos_i_q ] = true;
			
			ged_hx2 = LocalFilter(graphId);

			QVertexVisited[ pos_i_q ] = false;

			//insert the new match pair into a suitable position
			ged = ged + ged_hx1 + ged_hx2;
			
			arrP_vc.push_back(arr_q);
			arrP_pos_vc.push_back(arr_q_pos);

			//currentEstimateValue.push_back(ged);
			//arr_ls.push_back(arr_id);			

			if(arr_ls.size() == 0)
			{
				currentEstimateValue.push_back(ged);
				arr_ls.push_back(arr_id);
				arr_id++;
				continue;
			}

			int begin = 0;
			int end = arr_ls.size()-1;
			int middle  = ( begin + end )/2;


			while(begin <= end)
			{
				if(currentEstimateValue[middle] > ged)
				{
					end = middle - 1;				
				}
				else if(currentEstimateValue[ middle ] < ged)
				{
					begin = middle + 1;
				}
				else
				{
					break;
				}
				middle = ( begin + end )/2 ;
			}
			
			if(currentEstimateValue[middle] > ged)
			{
				currentEstimateValue.insert( currentEstimateValue.begin() + middle, ged );
				arr_ls.insert( arr_ls.begin() + middle, arr_id );
			}
			else
			{
				currentEstimateValue.insert( currentEstimateValue.begin() + middle +1, ged );
				arr_ls.insert( arr_ls.begin() + middle +1, arr_id );
			}
			
			arr_id++;
			
		}// for-1 end

		delete [] edge_d_0;
		int ged_global = 10000;

		ged_global = greedyMaximumMatch(graphId);

		if(currentEstimateValue[0] >= ged_global)
		{
			delete [] arr_d;
			delete [] arr_d_pos;

			delete [] GVertexVisited;
			delete [] QVertexVisited;

			for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
			{						
				arr_id = arr_ls[ arr_i ];												
				delete[] arrP_vc[arr_id];						
				delete[] arrP_pos_vc[arr_id];						
			}
			arr_ls.clear();
			arrP_vc.clear();
			arrP_pos_vc.clear();
			currentEstimateValue.clear();
			currentEditDist.clear();
			currentLen.clear();

			if(ged_global > tau)
			{
				return -1;
			}
			else
			{
				return ged_global;
			}
		}

		if(currentEstimateValue[0] > tau )
		{
			delete [] arr_d;
			delete [] arr_d_pos;

			delete [] GVertexVisited;
			delete [] QVertexVisited;

			for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
			{						
				arr_id = arr_ls[ arr_i ];												
				delete[] arrP_vc[arr_id];						
				delete[] arrP_pos_vc[arr_id];						
			}
			arr_ls.clear();
			arrP_vc.clear();
			arrP_pos_vc.clear();
			currentEstimateValue.clear();
			currentEditDist.clear();
			currentLen.clear();
			return -1;
		}
		while(arr_ls.size())//select the first pair in estimateId to extend the match --- while-1
		{
			//printf("arr_ls.size()=%d currentValue[0]=%d currentValue[%d]=%d \n",arr_ls.size(), currentEstimateValue[0], arr_ls.size()-1, currentEstimateValue[arr_ls.size()-1]);

			int currentId = arr_ls[0];
			
			arr_ls.erase( arr_ls.begin() ); 
			currentEstimateValue.erase( currentEstimateValue.begin() );

			int cur_len = currentLen[currentId];

			INT_P arr_q = arrP_vc[currentId];
			INT_P arr_q_pos = arrP_pos_vc[currentId];

			memset(GVertexVisited, 0 , length*sizeof(bool));
			memset(QVertexVisited, 0,  length*sizeof(bool));

			for( int len_i=0;len_i<v_d && len_i < cur_len; len_i++)
			{
				int pos_d = arr_d[ len_i ];
				GVertexVisited[ pos_d ] = true;				
				int pos_q = arr_q[ len_i ];
				QVertexVisited[ pos_q ] = true;
			}

			if(v_d < cur_len)
			{
				for(int len_i=v_d; len_i<cur_len; len_i++)
				{
					int pos_q = arr_q[ len_i ];
					QVertexVisited[ pos_q ] = true;
				}
			}

			int pos_d = arr_d[ cur_len ];

			if(pos_d == -1)// if-1.1
			{
				int num = 0;
				for(int pos_i_q=0; pos_i_q<vq; pos_i_q++)// for-2
				{	
					if( QVertexVisited[ pos_i_q ] )
					{
						continue;
					}
					num++;

					int neiSize_q = QVertexNeiSize[pos_i_q];
					
					num = num + neiSize_q;
										
				}// for-2 end

				ged = currentEditDist[currentId] + num;

				if(ged <= currentEstimateValue[0])
				{
					delete [] arr_q;
					delete [] arr_q_pos;

					delete [] arr_d;
					delete [] arr_d_pos;

					delete [] GVertexVisited;
					delete [] QVertexVisited;

					for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
					{						
						arr_id = arr_ls[ arr_i ];												
						delete[] arrP_vc[arr_id];						
						delete[] arrP_pos_vc[arr_id];						
					}
					arr_ls.clear();
					arrP_vc.clear();
					arrP_pos_vc.clear();
					currentEstimateValue.clear();
					currentEditDist.clear();
					currentLen.clear();

					if(ged > tau)
					{
						return -1;
					}
					else
					{
						return ged;
					}
					
				}

				if(ged >= ged_global )
				{
					delete [] arr_q;
					delete [] arr_q_pos;
					continue;
				}

				ged_global = ged;

				for(int len_i=0; len_i<currentEstimateValue.size(); len_i++)
				{
					if(ged <= currentEstimateValue[len_i] )
					{
						int arr_i = arr_ls[ len_i ];
						delete[] arrP_vc[arr_i];
						delete[] arrP_pos_vc[arr_i];

						currentEstimateValue.erase( currentEstimateValue.begin() + len_i );
						arr_ls.erase( arr_ls.begin() + len_i );
						len_i--;
					}
				}

			}// if-1.1 middle
			else
			{
				
				GVertexVisited[ pos_d ] = true;
				int label_d = GVertexLabel[ pos_d ];

				LISTINT matchLS_d;
				int neiSize_d = GVertexNeiSize[ pos_d ];

				LISTINT eLS;
				for(int nei_i_d=0; nei_i_d<neiSize_d; nei_i_d++)
				{
					int nei_id_d = GVertexNeighbor[ pos_d ][ nei_i_d ];
					int nei_id_d_pos = GVertexId[ nei_id_d ];

					if( !GVertexVisited[ nei_id_d_pos ])
					{
						int e_label = GEdgeLabel[ pos_d ][ nei_i_d ];
						eLS.push_back( e_label );
					}
					else
					{
						int pos_m = arr_d_pos[ nei_id_d_pos ];
						int e_label = GEdgeLabel[ pos_d ][ nei_i_d ];

						matchLS_d.push_back( 200*e_label + pos_m );
					}
	

				}

				for(int pos_i_q=0; pos_i_q<vq; pos_i_q++)// for-3
				{
					ged_gx =0;
					ged_hx1 = 0;
					ged_hx2 = 0;
					if( QVertexVisited[ pos_i_q ] )
					{
						continue;
					}

					int label_q = QVertexLabel[ pos_i_q ];

					if( label_d != label_q)
					{
						ged_gx++;
					}
					
					int neiSize_q = QVertexNeiSize[pos_i_q];

					ged_gx = ged_gx + ged_gx_compute(matchLS_d, arr_q_pos, neiSize_q, QVertexNeighbor[ pos_i_q ], QVertexId, QEdgeLabel[ pos_i_q ], QVertexVisited);

					ged = currentEditDist[currentId] + ged_gx;

					if(ged_global <= ged  )//this extending can not be the best
					{
						continue;
					}

					//estimate the first kind cost
					QVertexVisited[ pos_i_q ] = true;

					LISTINT eLS_d;
					LISTINT eLS_q;

					for(int len_i=0; len_i<cur_len; len_i++)//for-3.1
					{
						int pd_i = arr_d[ len_i ];
						int pq_i = arr_q[ len_i ];

						int nei_size_pd = GVertexNeiSize[ pd_i ];
						int nei_size_pq = QVertexNeiSize[ pq_i ];

						eLS_d.clear();
						eLS_q.clear();
						int same = 0;
	
						for(int nei_d=0; nei_d<nei_size_pd; nei_d++)//for-3.1.1
						{
							int nei_d_id = GVertexNeighbor[ pd_i ][ nei_d ];

							int nei_d_id_pos = GVertexId[ nei_d_id ];
							
							if(GVertexVisited[ nei_d_id_pos ])
							{								
								continue;
							}

							int e_d = GEdgeLabel[ pd_i ][ nei_d ];
			
							eLS_d.push_back(e_d);
						
						}//for-3.1.1 end

						for(int nei_q=0; nei_q<nei_size_pq; nei_q++)//for-3.1.2
						{							
							int nei_q_id = QVertexNeighbor[ pq_i ][ nei_q];
							
							int nei_q_id_pos = QVertexId[ nei_q_id ];
							
							if( QVertexVisited[ nei_q_id_pos ] )
							{															
								continue;
							}

							int e_q = QEdgeLabel[ pq_i ][ nei_q ];

							eLS_q.push_back(e_q);
							
						
						}//for-3.1.2 end

						int eLS_d_size = eLS_d.size();
						int eLS_q_size = eLS_q.size();

						for(int nei_d=0, nei_q=0; nei_d<eLS_d_size && nei_q<eLS_q_size; )
						{
							int el_d = eLS_d[ nei_d ];
							int el_q = eLS_q[ nei_q ];
							if(el_d < el_q)
							{
								nei_d++;
								continue;
							}
							else if(el_d > el_q)
							{
								nei_q++;
								continue;
							}
							else
							{
								nei_d++;
								nei_q++;
								same++;
							}
						}

						if(eLS_d_size <= eLS_q_size)
						{
							ged_hx1 = ged_hx1 +  eLS_q_size - same ;
						}
						else
						{
							ged_hx1 = ged_hx1 +  eLS_d_size - same ;
						}
					
					
					}// for-3.1 end
					
					int same = 0;

					eLS_d.clear();
					eLS_q.clear();

					for(int nei_i_q=0; nei_i_q<neiSize_q; nei_i_q++)//for-3.2
					{
						int nei_id_q = QVertexNeighbor[ pos_i_q ][ nei_i_q ];
						int nei_id_q_pos = QVertexId[ nei_id_q ];

						if(QVertexVisited[ nei_id_q_pos ])
						{
							continue;
						}
						int e_q = QEdgeLabel[ pos_i_q ][ nei_i_q ];
						eLS_q.push_back(e_q);
						
					}//for-3.2 end

					for(int e_i_d=0, e_i_q=0; e_i_d<eLS.size() && e_i_q<eLS_q.size(); )//for-3.3
					{					
						int e_d = eLS[ e_i_d ];
						int e_q = eLS_q[ e_i_q ];

						if( e_d < e_q)
						{
							e_i_d++;
						}
						else if( e_d > e_q )
						{
							e_i_q++;
						}
						else
						{
							same++;
							e_i_d++;
							e_i_q++;
						}
						
					}//for-3.3 end

					if( eLS_q.size() <= eLS.size())
					{
						ged_hx1 = ged_hx1 +  eLS.size() - same ;
					}
					else
					{
						ged_hx1 = ged_hx1 +  eLS_q.size() - same ;
					}

					eLS_q.clear();
				
					ged = ged + ged_hx1;

					if(ged_global <= ged  )//this extending can not be the best
					{
						eLS_d.clear();
						eLS_q.clear();
						continue;
					}

					//estimate the second kind cost
					
					ged_hx2 = LocalFilter(graphId);

					QVertexVisited[ pos_i_q ] = false;

					ged = ged + ged_hx2;

					if(ged_global <= ged  )//this extending can not be the best
					{
						eLS_d.clear();
						eLS_q.clear();

						continue;
					}

					if(cur_len + 1 == length)
					{
						eLS_d.clear();
						eLS_q.clear();
				
						if(ged <= currentEstimateValue[0])
						{
							delete [] arr_q;
							delete [] arr_q_pos;

							matchLS_d.clear();
							eLS.clear();
									
							delete [] GVertexVisited;
							delete [] QVertexVisited;

							delete [] arr_d;
							delete [] arr_d_pos;
			
							for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
							{
								arr_id = arr_ls[ arr_i ];
								delete[] arrP_vc[arr_id];
								delete[] arrP_pos_vc[arr_id];
							}

							arr_ls.clear();
							arrP_vc.clear();
							arrP_pos_vc.clear();
							currentEstimateValue.clear();
							currentEditDist.clear();
							currentLen.clear();

							if(ged > tau)
							{
								return -1;
							}
							else
							{
								return ged;
							}
							
						}

						if(ged >= ged_global )
						{
							continue;
						}

						ged_global = ged;

						for(int len_i=0; len_i<currentEstimateValue.size(); len_i++)
						{
							if(ged <= currentEstimateValue[len_i] )
							{
								int arr_i = arr_ls[ len_i ];
								delete[] arrP_vc[arr_i];
								delete[] arrP_pos_vc[arr_i];

								currentEstimateValue.erase( currentEstimateValue.begin() + len_i );
								arr_ls.erase( arr_ls.begin() + len_i );
								len_i--;
							}
						}

						continue;
					}

					INT_P arr_q_new = new int [cur_len+1];
					for(int len_i=0; len_i<cur_len; len_i++)
					{
						arr_q_new[ len_i ] = arr_q[ len_i ];
					}
					arr_q_new[ cur_len ] = pos_i_q;

					INT_P arr_q_pos_new = new int[length];
					for(int len_i=0; len_i<length; len_i++)
					{
						arr_q_pos_new[ len_i ] = arr_q_pos[ len_i ];
					}
					arr_q_pos_new[ pos_i_q ] = cur_len;

					arrP_vc.push_back(arr_q_new);
					arrP_pos_vc.push_back(arr_q_pos_new);
					currentLen.push_back( cur_len+1 );
					currentEditDist.push_back( ged_gx + currentEditDist[currentId] );

					//currentEstimateValue.push_back(ged);
					//arr_ls.push_back(arr_id);			

					if(arr_ls.size() == 0)// if-1.1.1
					{
						currentEstimateValue.push_back(ged);
						arr_ls.push_back(arr_id);
					}//if-1.1.1 middle
					else
					{
						int begin = 0;
						int end = arr_ls.size()-1;
						int middle  = ( begin + end )/2;

						while(begin <= end)
						{
							if(currentEstimateValue[middle] > ged)
							{
								end = middle - 1;				
							}
							else if(currentEstimateValue[ middle ] < ged)
							{
								begin = middle + 1;
							}
							else
							{
								break;
							}
							middle = ( begin + end )/2 ;
						}

						if(currentEstimateValue[middle] > ged)
						{
							currentEstimateValue.insert( currentEstimateValue.begin() + middle, ged );
							arr_ls.insert( arr_ls.begin() + middle, arr_id );
						}
						else
						{
							currentEstimateValue.insert( currentEstimateValue.begin() + middle +1, ged );
							arr_ls.insert( arr_ls.begin() + middle +1, arr_id );
						}
	
					
					}//if-1.1.1 end


					arr_id++;
										
				}// for-3 end	

				matchLS_d.clear();
				eLS.clear();
				
			}// if-1.1 end

			delete [] arr_q;
			delete [] arr_q_pos;


		}// while-1 end

		delete [] arr_d;
		delete [] arr_d_pos;

		for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
		{
			arr_id = arr_ls[ arr_i ];
			delete[] arrP_vc[arr_id];
			delete[] arrP_pos_vc[arr_id];
		}

		delete [] GVertexVisited;
		delete [] QVertexVisited;


		arr_ls.clear();
		arrP_vc.clear();
		arrP_pos_vc.clear();
		currentEstimateValue.clear();
		currentEditDist.clear();
		currentLen.clear();
	}// if-1 middle
	else
	{
		length = v_d;

		INT_P arr_q = new int[length];//stores the vertex pos arry
		INT_P arr_q_pos = new int[length] ;//stores the the position of vertex in arr_q

		GVertexVisited = new bool[length];
		QVertexVisited = new bool[length];

		memset(GVertexVisited, 0, length*sizeof(bool));
		memset(QVertexVisited, 0, length*sizeof(bool));

		breadthFirstFilling( vq, arr_q, arr_q_pos, QVertexId, QVertexNeiSize, QVertexNeighbor, QVertexVisited);
		
		memset(QVertexVisited, 0, length*sizeof(bool));

		for(int len_i=vq; len_i<length; len_i++)
		{
			arr_q[ len_i ] = -1;
		}

		int label_q_0 = QVertexLabel[ 0 ];

		int neiSize_q = QVertexNeiSize[ 0 ];
		INT_P edge_q_0 = new int[neiSize_q];

		QVertexVisited[ 0 ] = true;//pos_d_0

		for(int nei_i_q=0; nei_i_q<neiSize_q; nei_i_q++)
		{
			edge_q_0[ nei_i_q ] = QEdgeLabel[0][nei_i_q];
		}

		int arr_id = 0;

		LISTINT arr_ls;// stores the arr id
		LISTINT currentEstimateValue;//the list that consists of estimated ged value

		INT_P_vc arrP_vc;//stores the arr pointer
		INT_P_vc arrP_pos_vc;
		LISTINT currentEditDist;//the current edit distance having been computed
		LISTINT currentLen;//the current length having been computed

		for(int pos_i_d=0; pos_i_d<v_d; pos_i_d++)// for-1
		{
			INT_P arr_d = new int[1];
			arr_d[ 0 ] = pos_i_d;

			INT_P arr_d_pos = new int[length];
			arr_d_pos[ pos_i_d ] = 0;

			int label_d = GVertexLabel[ pos_i_d ];

			if(label_q_0 == label_d)
			{
				ged = 0;
			}
			else
			{
				ged = 1;
			}
			currentLen.push_back(1);
			currentEditDist.push_back(ged);
			
			ged_hx1 = 0;//the first kind of estimation
		
			int nei_i_d = 0, nei_i_q = 0; 
			int neiSize_d  = GVertexNeiSize[ pos_i_d ];
			for( ; nei_i_q<neiSize_q && nei_i_d<neiSize_d; )// for-1.1
			{
				int e_label_q = edge_q_0[nei_i_q];
				int e_label_d = GEdgeLabel[ pos_i_d ][nei_i_d];

				if(e_label_d < e_label_q)
				{
					nei_i_d++;
				}
				else if(e_label_d == e_label_q)
				{
					ged_hx1++;
					nei_i_d++;
					nei_i_q++;
				}
				else
				{
					nei_i_q++;
				}

			}// for-1.1 end

			if(neiSize_q >= neiSize_d)
			{
				ged_hx1 = neiSize_q - ged_hx1;
			}
			else
			{
				ged_hx1 = neiSize_d - ged_hx1;
			}

			//estimate the second kind cost
			GVertexVisited[ pos_i_d ] = true;
			
			ged_hx2 = LocalFilter(graphId);

			GVertexVisited[ pos_i_d ] = false;

			//insert the new match pair into a suitable position
			ged = ged + ged_hx1 + ged_hx2;
			
			arrP_vc.push_back(arr_d);
			arrP_pos_vc.push_back(arr_d_pos);

			//currentEstimateValue.push_back(ged);
			//arr_ls.push_back(arr_id);			

			if(arr_ls.size() == 0)
			{
				currentEstimateValue.push_back(ged);
				arr_ls.push_back(arr_id);
				arr_id++;
				continue;
			}

			int begin = 0;
			int end = arr_ls.size()-1;
			int middle  = ( begin + end )/2;


			while(begin <= end)
			{
				if(currentEstimateValue[middle] > ged)
				{
					end = middle - 1;				
				}
				else if(currentEstimateValue[ middle ] < ged)
				{
					begin = middle + 1;
				}
				else
				{
					break;
				}
				middle = ( begin + end )/2 ;
			}
			
			if(currentEstimateValue[middle] > ged)
			{
				currentEstimateValue.insert( currentEstimateValue.begin() + middle, ged );
				arr_ls.insert( arr_ls.begin() + middle, arr_id );
			}
			else
			{
				currentEstimateValue.insert( currentEstimateValue.begin() + middle +1, ged );
				arr_ls.insert( arr_ls.begin() + middle +1, arr_id );
			}
			
			arr_id++;
			
		}// for-1 end

		delete [] edge_q_0;

		int ged_global = 10000;

		ged_global = greedyMaximumMatch(graphId);

		if(currentEstimateValue[0] >= ged_global)
		{
			delete [] arr_q;
			delete [] arr_q_pos;

			delete [] GVertexVisited;
			delete [] QVertexVisited;

			for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
			{						
				arr_id = arr_ls[ arr_i ];												
				delete[] arrP_vc[arr_id];						
				delete[] arrP_pos_vc[arr_id];						
			}
			arr_ls.clear();
			arrP_vc.clear();
			arrP_pos_vc.clear();
			currentEstimateValue.clear();
			currentEditDist.clear();
			currentLen.clear();

			if(ged_global > tau)
			{
				return -1;
			}
			else
			{
				return ged_global;
			}
		}

		if(currentEstimateValue[0] > tau )
		{
			delete [] arr_q;
			delete [] arr_q_pos;

			delete [] GVertexVisited;
			delete [] QVertexVisited;

			for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
			{						
				arr_id = arr_ls[ arr_i ];												
				delete[] arrP_vc[arr_id];						
				delete[] arrP_pos_vc[arr_id];						
			}
			arr_ls.clear();
			arrP_vc.clear();
			arrP_pos_vc.clear();
			currentEstimateValue.clear();
			currentEditDist.clear();
			currentLen.clear();
			return -1;
		}

		while(arr_ls.size())//select the first pair in estimateId to extend the match --- while-1
		{
			int currentId = arr_ls[0];
			
			arr_ls.erase( arr_ls.begin() ); 
			currentEstimateValue.erase( currentEstimateValue.begin() );

			int cur_len = currentLen[currentId];

			INT_P arr_d = arrP_vc[currentId];
			INT_P arr_d_pos = arrP_pos_vc[currentId];

			memset(GVertexVisited, 0 , length*sizeof(bool));
			memset(QVertexVisited, 0,  length*sizeof(bool));

			for( int len_i=0;len_i<vq && len_i < cur_len; len_i++)
			{
				int pos_d = arr_d[ len_i ];
				GVertexVisited[ pos_d ] = true;				
				int pos_q = arr_q[ len_i ];
				QVertexVisited[ pos_q ] = true;
			}

			if(vq< cur_len)
			{
				for(int len_i=vq; len_i<cur_len; len_i++)
				{
					int pos_d = arr_d[ len_i ];
					GVertexVisited[ pos_d ] = true;
				}
			}

			int pos_q = arr_q[ cur_len ];

			if(pos_q == -1)// if-1.1
			{
				int num = 0;
				for(int pos_i_d=0; pos_i_d<v_d; pos_i_d++)// for-2
				{	
					if( GVertexVisited[ pos_i_d ] )
					{
						continue;
					}
					num++;

					int neiSize_d = GVertexNeiSize[pos_i_d];
					
					num = num + neiSize_d;
										
				}// for-2 end

				ged = currentEditDist[currentId] + num;

				if(ged <= currentEstimateValue[0])
				{
					delete [] arr_q;
					delete [] arr_q_pos;

					delete [] GVertexVisited;
					delete [] QVertexVisited;

					for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
					{						
						arr_id = arr_ls[ arr_i ];												
						delete[] arrP_vc[arr_id];						
						delete[] arrP_pos_vc[arr_id];						
					}
					arr_ls.clear();
					arrP_vc.clear();
					arrP_pos_vc.clear();
					currentEstimateValue.clear();
					currentEditDist.clear();
					currentLen.clear();

					if(ged > tau)
					{
						return -1;
					}
					else
					{
						return ged;
					}
					
				}

				if(ged >= ged_global )
				{
					delete [] arr_d;
					delete [] arr_d_pos;
					continue;
				}

				ged_global = ged;

				for(int len_i=0; len_i<currentEstimateValue.size(); len_i++)
				{
					if(ged <= currentEstimateValue[len_i] )
					{
						int arr_i = arr_ls[ len_i ];
						delete[] arrP_vc[arr_i];
						delete[] arrP_pos_vc[arr_i];

						currentEstimateValue.erase( currentEstimateValue.begin() + len_i );
						arr_ls.erase( arr_ls.begin() + len_i );
						len_i--;
					}
				}

			}// if-1.1 middle
			else
			{
				
				QVertexVisited[ pos_q ] = true;
				int label_q = QVertexLabel[ pos_q ];

				LISTINT matchLS_q;
				int neiSize_q = QVertexNeiSize[ pos_q ];

				LISTINT eLS;
				for(int nei_i_q=0; nei_i_q<neiSize_q; nei_i_q++)
				{
					int nei_id_q = QVertexNeighbor[ pos_q ][ nei_i_q ];
					int nei_id_q_pos = QVertexId[ nei_id_q ];

					if( !QVertexVisited[ nei_id_q_pos ])
					{
						int e_label = QEdgeLabel[ pos_q ][ nei_i_q ];
						eLS.push_back( e_label );
					}
					else
					{
						int pos_m = arr_q_pos[ nei_id_q_pos ];
						int e_label = QEdgeLabel[ pos_q ][ nei_i_q ];

						matchLS_q.push_back( 200*e_label + pos_m );
					}
	

				}

				for(int pos_i_d=0; pos_i_d<v_d; pos_i_d++)// for-3
				{
					ged_gx =0;
					ged_hx1 = 0;
					ged_hx2 = 0;
					if( GVertexVisited[ pos_i_d ] )
					{
						continue;
					}

					int label_d = GVertexLabel[ pos_i_d ];

					if( label_d != label_q)
					{
						ged_gx++;
					}
					
					int neiSize_d = GVertexNeiSize[pos_i_d];

					ged_gx = ged_gx + ged_gx_compute(matchLS_q, arr_d_pos, neiSize_d, GVertexNeighbor[ pos_i_d ], GVertexId, GEdgeLabel[ pos_i_d ], GVertexVisited);

					ged = currentEditDist[currentId] + ged_gx;

					if(ged_global <= ged  )//this extending can not be the best
					{
						continue;
					}

					//estimate the first kind cost
					GVertexVisited[ pos_i_d ] = true;

					LISTINT eLS_d;
					LISTINT eLS_q;

					for(int len_i=0; len_i<cur_len; len_i++)//for-3.1
					{
						int pd_i = arr_d[ len_i ];
						int pq_i = arr_q[ len_i ];

						int nei_size_pd = GVertexNeiSize[ pd_i ];
						int nei_size_pq = QVertexNeiSize[ pq_i ];

						eLS_d.clear();
						eLS_q.clear();
						int same = 0;
	
						for(int nei_d=0; nei_d<nei_size_pd; nei_d++)//for-3.1.1
						{
							int nei_d_id = GVertexNeighbor[ pd_i ][ nei_d ];

							int nei_d_id_pos = GVertexId[ nei_d_id ];
							
							if(GVertexVisited[ nei_d_id_pos ])
							{								
								continue;
							}

							int e_d = GEdgeLabel[ pd_i ][ nei_d ];
			
							eLS_d.push_back(e_d);
						
						}//for-3.1.1 end

						for(int nei_q=0; nei_q<nei_size_pq; nei_q++)//for-3.1.2
						{							
							int nei_q_id = QVertexNeighbor[ pq_i ][ nei_q];
							
							int nei_q_id_pos = QVertexId[ nei_q_id ];
							
							if( QVertexVisited[ nei_q_id_pos ] )
							{															
								continue;
							}

							int e_q = QEdgeLabel[ pq_i ][ nei_q ];

							eLS_q.push_back(e_q);
							
						
						}//for-3.1.2 end

						int eLS_d_size = eLS_d.size();
						int eLS_q_size = eLS_q.size();

						for(int nei_d=0, nei_q=0; nei_d<eLS_d_size && nei_q<eLS_q_size; )
						{
							int el_d = eLS_d[ nei_d ];
							int el_q = eLS_q[ nei_q ];
							if(el_d < el_q)
							{
								nei_d++;
								continue;
							}
							else if(el_d > el_q)
							{
								nei_q++;
								continue;
							}
							else
							{
								nei_d++;
								nei_q++;
								same++;
							}
						}

						if(eLS_d_size <= eLS_q_size)
						{
							ged_hx1 = ged_hx1 +  eLS_q_size - same ;
						}
						else
						{
							ged_hx1 = ged_hx1 +  eLS_d_size - same ;
						}
					
					
					}// for-3.1 end
					
					int same = 0;

					eLS_d.clear();
					eLS_q.clear();

					for(int nei_i_d=0; nei_i_d<neiSize_d; nei_i_d++)//for-3.2
					{
						int nei_id_d = GVertexNeighbor[ pos_i_d ][ nei_i_d ];
						int nei_id_d_pos = GVertexId[ nei_id_d ];

						if(GVertexVisited[ nei_id_d_pos ])
						{
							continue;
						}
						int e_q = GEdgeLabel[ pos_i_d ][ nei_i_d ];
						eLS_d.push_back(e_q);
						
					}//for-3.2 end

					for(int e_i_q=0, e_i_d=0; e_i_q<eLS.size() && e_i_d<eLS_d.size(); )//for-3.3
					{					
						int e_q = eLS[ e_i_q ];
						int e_d = eLS_d[ e_i_d ];

						if( e_d < e_q)
						{
							e_i_d++;
						}
						else if( e_d > e_q )
						{
							e_i_q++;
						}
						else
						{
							same++;
							e_i_d++;
							e_i_q++;
						}
						
					}//for-3.3 end

					if( eLS_d.size() <= eLS.size())
					{
						ged_hx1 = ged_hx1 +  eLS.size() - same ;
					}
					else
					{
						ged_hx1 = ged_hx1 +  eLS_d.size() - same ;
					}

					eLS_d.clear();
				
					ged = ged + ged_hx1;

					if(ged_global <= ged  )//this extending can not be the best
					{
						eLS_d.clear();
						eLS_q.clear();

						continue;
					}

					//estimate the second kind cost
					
					ged_hx2 = LocalFilter(graphId);

					GVertexVisited[ pos_i_d ] = false;

					ged = ged + ged_hx2;

					if(ged_global <= ged  )//this extending can not be the best
					{
						eLS_d.clear();
						eLS_q.clear();

						continue;
					}

					if(cur_len + 1 == length)
					{
						eLS_d.clear();
						eLS_q.clear();
				
						if(ged <= currentEstimateValue[0])
						{
							delete [] arr_d;
							delete [] arr_d_pos;

							matchLS_q.clear();
							eLS.clear();
									
							delete [] GVertexVisited;
							delete [] QVertexVisited;

							delete [] arr_q;
							delete [] arr_q_pos;
			
							for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
							{
								arr_id = arr_ls[ arr_i ];
								delete[] arrP_vc[arr_id];
								delete[] arrP_pos_vc[arr_id];
							}

							arr_ls.clear();
							arrP_vc.clear();
							arrP_pos_vc.clear();
							currentEstimateValue.clear();
							currentEditDist.clear();
							currentLen.clear();

							if(ged > tau)
							{
								return -1;
							}
							else
							{
								return ged;
							}
							
						}

						if(ged >= ged_global )
						{
							continue;
						}

						ged_global = ged;

						for(int len_i=0; len_i<currentEstimateValue.size(); len_i++)
						{
							if(ged <= currentEstimateValue[len_i] )
							{
								int arr_i = arr_ls[ len_i ];
								delete[] arrP_vc[arr_i];
								delete[] arrP_pos_vc[arr_i];

								currentEstimateValue.erase( currentEstimateValue.begin() + len_i );
								arr_ls.erase( arr_ls.begin() + len_i );
								len_i--;
							}
						}

						continue;
					}

					INT_P arr_d_new = new int [cur_len+1];
					for(int len_i=0; len_i<cur_len; len_i++)
					{
						arr_d_new[ len_i ] = arr_d[ len_i ];
					}
					arr_d_new[ cur_len ] = pos_i_d;

					INT_P arr_d_pos_new = new int[length];
					for(int len_i=0; len_i<length; len_i++)
					{
						arr_d_pos_new[ len_i ] = arr_d_pos[ len_i ];
					}
					arr_d_pos_new[ pos_i_d ] = cur_len;

					arrP_vc.push_back(arr_d_new);
					arrP_pos_vc.push_back(arr_d_pos_new);
					currentLen.push_back( cur_len+1 );
					currentEditDist.push_back( ged_gx + currentEditDist[currentId] );

					//currentEstimateValue.push_back(ged);
					//arr_ls.push_back(arr_id);			

					if(arr_ls.size() == 0)// if-1.1.1
					{
						currentEstimateValue.push_back(ged);
						arr_ls.push_back(arr_id);
					}//if-1.1.1 middle
					else
					{
						int begin = 0;
						int end = arr_ls.size()-1;
						int middle  = ( begin + end )/2;

						while(begin <= end)
						{
							if(currentEstimateValue[middle] > ged)
							{
								end = middle - 1;				
							}
							else if(currentEstimateValue[ middle ] < ged)
							{
								begin = middle + 1;
							}
							else
							{
								break;
							}
							middle = ( begin + end )/2 ;
						}

						if(currentEstimateValue[middle] > ged)
						{
							currentEstimateValue.insert( currentEstimateValue.begin() + middle, ged );
							arr_ls.insert( arr_ls.begin() + middle, arr_id );
						}
						else
						{
							currentEstimateValue.insert( currentEstimateValue.begin() + middle +1, ged );
							arr_ls.insert( arr_ls.begin() + middle +1, arr_id );
						}
					
					}//if-1.1.1 end

					arr_id++;
										
				}// for-3 end	

				matchLS_q.clear();
				eLS.clear();
				
			}// if-1.1 end

			delete [] arr_d;
			delete [] arr_d_pos;


		}// while-1 end

		delete [] arr_q;
		delete [] arr_q_pos;

		for(int arr_i=0; arr_i<arr_ls.size(); arr_i++)
		{
			arr_id = arr_ls[ arr_i ];
			delete[] arrP_vc[arr_id];
			delete[] arrP_pos_vc[arr_id];
		}

		delete [] GVertexVisited;
		delete [] QVertexVisited;


		arr_ls.clear();
		arrP_vc.clear();
		arrP_pos_vc.clear();
		currentEstimateValue.clear();
		currentEditDist.clear();
		currentLen.clear();
	}// if-1 end


	return ged;

}

void GraphReader::breadthFirstFilling(int vn, int *arr, int *arr_pos, int *VertexId, int *VertexNeiSize, int **VertexNeighbor, bool *VertexVisited)
{

	arr[0] = 0;
	arr_pos[ 0 ] = 0;
	VertexVisited[0] = true;

	int cur_last = 1;// the last position to be filled

	for(int pos_i=0; pos_i<vn; pos_i++)//for-1
	{
		if(cur_last == vn)
		{
			break;
		}

		//int v_id = arr[arr_i];
		//int pos_i = VertexId[v_id];
		int v_nei_size = VertexNeiSize[pos_i];

		for(int nei_i=0; nei_i<v_nei_size; nei_i++)
		{
			int nei_id = VertexNeighbor[pos_i][nei_i];
			int nei_id_pos = VertexId[nei_id];

			if( VertexVisited[nei_id_pos] )
			{
				continue;
			}
			arr[ cur_last ] = nei_id_pos;
			arr_pos[ nei_id_pos ] = cur_last;
			VertexVisited[ nei_id_pos ] = true;

			cur_last++;		
		}
	
	}//for-1 end

	/*
	for(int pos_i=0; pos_i<vn; pos_i++)//for-2
	{
		printf("arr[%d]=%d arr_pos[%d]=%d\n", pos_i, arr[pos_i], pos_i, arr_pos[pos_i]);
	}// for2-end

	*/
	
}

int GraphReader::LocalFilter(int graphId)// the second kind estimation
{
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	int v_d = vd[graphId];
	INT_P GVertexLabel = graphVertexLabel[graphId];
	INT_P GVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * GEdgeLabel = graphEdgeLabel[graphId];
	INT_P * GVertexNeighbor = graphVertexNeighbor[graphId];
	INT_P GVertexId = graphVertexId[graphId];

	int label_d, label_q;

	//int v_d_i=0,  v_q_i=0;

	int pos_d_i = 0, pos_q_i = 0;
	if(v_d >= vq )//if-1
	{
		bool * used = new bool[vq];
		memset(used, 0, vq*sizeof(bool));

		for( pos_d_i=0,  pos_q_i=0; pos_d_i<v_d && pos_q_i<vq; )//for-1
		{
			 if( GVertexVisited[pos_d_i] )
			 {
				pos_d_i++;
				continue;
			 }

			 if(QVertexVisited[pos_q_i])
			 {
				pos_q_i++;
				continue;
			 }

			 label_d = GVertexLabel[pos_d_i];
			 label_q = QVertexLabel[pos_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 count1++;
				 pos_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				pos_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {
				int total_d = 0;
				int total_q = 0;
				int same = 0;
				
				//int pos_d_i_local = pos_d_i;
				int pos_q_i_local = pos_q_i;

				while(label_d == label_q)//while-1
				{
					if( GVertexVisited[ pos_d_i ] )
					 {
						pos_d_i++;
						label_d = GVertexLabel[ pos_d_i ];
						continue;
					}

					total_d++;
					total_q = 0;
					while(label_d == label_q)//while-1.1
					{
						if( QVertexVisited[ pos_q_i_local ] )
						{
							pos_q_i_local++;
							label_q = QVertexLabel[ pos_q_i_local ];
							continue;
						}
						total_q++;

						if( used[ pos_q_i_local ])
						{
							pos_q_i_local++;

							label_q = QVertexLabel[ pos_q_i_local ];
							continue;
						}

						int nei_size_d = GVertexNeiSize[pos_d_i];
						int nei_size_q = QVertexNeiSize[pos_q_i_local];

						bool equal = true;
						int nei_i_d=0, nei_i_q=0;
						for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; )//for-1.1
						{
							int nei_id_d = GVertexNeighbor[ pos_d_i ][nei_i_d];
							int nei_id_d_pos = GVertexId[ nei_id_d ];

							if(GVertexVisited[ pos_d_i ] || GVertexVisited[nei_id_d_pos])
							{
								nei_i_d++;
								continue;
							}

							int nei_id_q = QVertexNeighbor[ pos_q_i_local ][nei_i_q];
							int nei_id_q_pos = QVertexId[ nei_id_q ];

							if(QVertexVisited[ pos_q_i_local ] || QVertexVisited[nei_id_q_pos])
							{
								nei_i_q++;
								continue;
							}
					
							int e_label_d = GEdgeLabel[ pos_d_i ][nei_i_d];
							int e_label_q = QEdgeLabel[ pos_q_i_local ][nei_i_q];
							if(e_label_d != e_label_q )
							{	
								equal = false;
								break;
							}
							else 
							{
								nei_i_d++; 
								nei_i_q++;
							}

						}//for-1.1 end

						for( ; nei_i_d<nei_size_d; nei_i_d++)//for-1.1
						{
							int nei_id_d = GVertexNeighbor[ pos_d_i ][nei_i_d];
							int nei_id_d_pos = GVertexId[ nei_id_d ];

							if(GVertexVisited[ pos_d_i ] || GVertexVisited[nei_id_d_pos])
							{
								continue;
							}
							else
							{
								break;
							}
						}

						for(  ; nei_i_q<nei_size_q; nei_i_q++)//for-1.1
						{						
							int nei_id_q = QVertexNeighbor[ pos_q_i_local ][nei_i_q];
							int nei_id_q_pos = QVertexId[ nei_id_q ];

							if(QVertexVisited[ pos_q_i_local ] || QVertexVisited[nei_id_q_pos])
							{								
								continue;
							}
							else
							{
								break;
							}
					

						}//for-1.1 end


						if( equal && nei_i_d==nei_size_d && nei_i_q == nei_size_q)
						{
							used[ pos_q_i_local ] = true;
							same++;
							break;
						}

						pos_q_i_local++;
						if(pos_q_i_local == vq)
						{
							break;
						}
						label_q = QVertexLabel[ pos_q_i_local ];

					}//while-1.1 end

					pos_d_i++;
					pos_q_i_local = pos_q_i;

					if(pos_d_i == v_d)
					{
						break;
					}

					label_d = GVertexLabel[ pos_d_i ];
					label_q = QVertexLabel[ pos_q_i_local ];

				}//while-1 end

				pos_q_i = pos_q_i + total_q;
				
				if(total_d >= total_q)
				{
					count1 = count1 + total_d - total_q;
					count2 = count2 + total_q - same;
				}
				else
				{
					count2 = count2 + total_d - same;
				}

			}// if-1.1 end

		}// for-1 end

		int num = 0;
		for( ; pos_d_i<v_d; pos_d_i++ )//for-1
		{
			 if( GVertexVisited[pos_d_i] )
			 {				
				continue;
			 }
			 num++;
		}
		
		count1 = count1 + num;//count1 = count1 + v_d - pos_d_i;

		delete [] used;

	}// if-1 middle
	else
	{
		bool * used = new bool[v_d];
		memset(used, 0, v_d*sizeof(bool));

		for( pos_d_i=0,  pos_q_i=0; pos_d_i<v_d && pos_q_i<vq; )//for-1
		{
			 if( GVertexVisited[pos_d_i] )
			 {
				pos_d_i++;
				continue;
			 }

			 if(QVertexVisited[pos_q_i])
			 {
				pos_q_i++;
				continue;
			 }

			 label_d = GVertexLabel[pos_d_i];
			 label_q = QVertexLabel[pos_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 pos_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				count1++;
				pos_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {
				int total_d = 0;
				int total_q = 0;
				int same = 0;
				
				//int pos_d_i_local = pos_d_i;
				int pos_d_i_local = pos_d_i;

				while(label_d == label_q)//while-1
				{
					if( QVertexVisited[ pos_q_i ] )
					 {
						pos_q_i++;
						label_q = QVertexLabel[ pos_q_i ];
						continue;
					}

					total_q++;
					total_d = 0;
					while(label_d == label_q)//while-1.1
					{
						if( GVertexVisited[ pos_d_i_local ] )
						{
							pos_d_i_local++;
							label_d = GVertexLabel[ pos_d_i_local ];
							continue;
						}
						total_d++;

						if( used[ pos_d_i_local ])
						{
							pos_d_i_local++;

							label_d = GVertexLabel[ pos_d_i_local ];
							continue;
						}

						int nei_size_d = GVertexNeiSize[pos_d_i_local];
						int nei_size_q = QVertexNeiSize[pos_q_i];

						bool equal = true;
						int nei_i_d=0, nei_i_q=0;
						for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; )//for-1.1
						{
							int nei_id_d = GVertexNeighbor[ pos_d_i_local ][nei_i_d];
							int nei_id_d_pos = GVertexId[ nei_id_d ];

							if(GVertexVisited[ pos_d_i_local ] || GVertexVisited[nei_id_d_pos])
							{
								nei_i_d++;
								continue;
							}

							int nei_id_q = QVertexNeighbor[ pos_q_i ][nei_i_q];
							int nei_id_q_pos = QVertexId[ nei_id_q ];

							if(QVertexVisited[ pos_q_i ] || QVertexVisited[nei_id_q_pos])
							{
								nei_i_q++;
								continue;
							}
					
							int e_label_d = GEdgeLabel[ pos_d_i_local ][nei_i_d];
							int e_label_q = QEdgeLabel[ pos_q_i ][nei_i_q];
							if(e_label_d != e_label_q )
							{	
								equal = false;
								break;
							}
							else 
							{
								nei_i_d++; 
								nei_i_q++;
							}

						}//for-1.1 end

						for( ;nei_i_d<nei_size_d; nei_i_d++)
						{
							int nei_id_d = GVertexNeighbor[ pos_d_i_local ][nei_i_d];
							int nei_id_d_pos = GVertexId[ nei_id_d ];

							if(GVertexVisited[ pos_d_i_local ] || GVertexVisited[nei_id_d_pos])
							{
								continue;
							}
							else
							{
								break;
							}
						}
						
						for( ;nei_i_q<nei_size_q; nei_i_q++)
						{
							int nei_id_q = QVertexNeighbor[ pos_q_i ][nei_i_q];
							int nei_id_q_pos = QVertexId[ nei_id_q ];

							if(QVertexVisited[ pos_q_i ] || QVertexVisited[nei_id_q_pos])
							{
								continue;
							}
							else
							{
								break;
							}
						}

						if( equal && nei_i_d==nei_size_d && nei_i_q == nei_size_q)
						{
							used[ pos_d_i_local ] = true;
							same++;
							break;
						}

						pos_d_i_local++;
						if(pos_d_i_local == v_d)
						{
							break;
						}

						label_d = GVertexLabel[ pos_d_i_local ];

					}//while-1.1 end

					pos_q_i++;
					pos_d_i_local = pos_d_i;

					if(pos_q_i == vq)
					{
						break;
					}

					label_d = GVertexLabel[ pos_d_i_local ];
					label_q = QVertexLabel[ pos_q_i ];

				}//while-1 end

				pos_d_i = pos_d_i + total_d;
				
				if(total_d <= total_q)
				{
					count1 = count1 + total_q - total_d;
					count2 = count2 + total_d - same;
				}
				else
				{
					count2 = count2 + total_q - same;
				}

			}// if-1.1 end

		}// for-1 end

		int num = 0;
		for( ; pos_q_i<vq; pos_q_i++)//for-1
		{			
			 if(QVertexVisited[pos_q_i])
			 {				
				continue;
			 }
			 num++;
		}

		count1 = count1 + num;//count1 = count1 + vq - pos_q_i

		delete [] used;
		
	}// if-1 end
	
	if(count2%2 == 0)
	{
		ged = count1+count2/2;
	}
	else
	{
		ged = count1+count2/2+1;
	}
	

	return ged;
}

int GraphReader::ged_gx_compute(LISTINT &matchLS_d, int *arr_pos, int neiSize, int *Neighbor, int *VertexId, int *EdgeLabel, bool *VertexVisited)
{
	int ged = 0;
	
	int length = matchLS_d.size();
	int total = 0;// the total number in the latter one
	int bothSame = 0;
	int singleSame = 0;
	for(int nei_i=0; nei_i<neiSize; nei_i++)//for-1
	{
		int nei_id = Neighbor[nei_i];
		int nei_id_pos = VertexId[ nei_id ];

		if( !VertexVisited[nei_id_pos] )
		{
			continue;
		}
		total++;
		int edge_label = EdgeLabel[ nei_i ];
		int m_pos = arr_pos[ nei_id_pos ];

		for(int len_i=0; len_i<length; len_i++)
		{
			int e_l = matchLS_d[len_i]/200;
			int m = matchLS_d[len_i]%200;

			if( m == m_pos )
			{
				if( e_l == edge_label)
				{
					bothSame++;
				}
				else
				{
					singleSame++;
				}

				break;
			}
		}
		
	}//for-1 end

	ged = length + total - 2*bothSame - singleSame;
	
	return ged;
}

int GraphReader::greedyMaximumMatch(int graphId)
{
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	int v_d = vd[graphId];
	INT_P DVertexLabel = graphVertexLabel[graphId];
	INT_P DVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * DEdgeLabel = graphEdgeLabel[graphId];
	INT_P * DVertexNeighbor = graphVertexNeighbor[graphId];
	INT_P DVertexId = graphVertexId[graphId];

	int label_d, label_q;

	int v_d_i=0,  v_q_i=0;

	INT_P matchQ = new int[vq];//[pos]
	INT_P matchG = new int[v_d]; //[pos]

	bool * QVisited = new bool[vq];
	bool * GVisited = new bool[v_d];

	memset(QVisited, 0, vq*sizeof(bool));
	memset(GVisited, 0, v_d*sizeof(bool));

	if(v_d >= vq )// Q is in outer layer --- if-1
	{
		INT_P matchG_pos = new int[ v_d ];

		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )//for-1
		{
			 label_d = DVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 v_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				v_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {		
				int nei_size_d = DVertexNeiSize[v_d_i];
				int nei_size_q = QVertexNeiSize[v_q_i];
				int nei_i_d=0, nei_i_q=0;
				for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; nei_i_d++, nei_i_q++)//for-1.1
				{
					int e_label_d = DEdgeLabel[v_d_i][nei_i_d];
					int e_label_q = QEdgeLabel[v_q_i][nei_i_q];
					if(e_label_d < e_label_q )
					{											
						v_d_i++;
						break;
					}
					else if(e_label_d > e_label_q )
					{						
						v_q_i++;
						break;
					}
				}//for-1.1 end

				if(nei_i_d == nei_size_d && nei_i_q<nei_size_q )// if-1.1.2
				{				
					v_d_i++;
				}// if-1.1.2 middle
				else if(nei_i_d<nei_size_d && nei_i_q == nei_size_q)
				{
					v_q_i++;
				}// if-1.1.2 middle
				else if(nei_i_d==nei_size_d && nei_i_q == nei_size_q)
				{
					QVisited[ v_q_i ] = true;
					GVisited[ v_d_i ] = true;

					matchG[ v_q_i ] = v_d_i;
					matchG_pos[ v_d_i ] = v_q_i;

					v_d_i++;
					v_q_i++;
				}//if-1.1.2 end

			 }// if-1.1 end
		}// for-1 end

		for( v_q_i=0; v_q_i<vq; v_q_i++)//for-2
		{
			if(QVisited[ v_q_i ])
			{
				continue;
			}

			label_q = QVertexLabel[v_q_i];

			for( v_d_i=0; v_d_i<v_d; v_d_i++)//for-2.1
			{
				if(GVisited[ v_d_i ])
				{
					continue;
				}

				label_d = DVertexLabel[ v_d_i ];
				if(label_d == label_q)
				{
					QVisited[ v_q_i ] = true;
					GVisited[ v_d_i ] = true;

					matchG[ v_q_i ] = v_d_i;
					matchG_pos[ v_d_i ] = v_q_i;
					break;
				}
				else if(label_d > label_q)
				{
					break;
				}
			
			}//for-2.1 end

		}//for-2 end

		for( v_q_i=0; v_q_i<vq; v_q_i++)//for-3
		{
			if(QVisited[ v_q_i ])
			{
				continue;
			}

			for( v_d_i=0; v_d_i<v_d; v_d_i++)//for-3.1
			{
				if(GVisited[ v_d_i ])
				{
					continue;
				}

				QVisited[ v_q_i ] = true;
				GVisited[ v_d_i ] = true;
				matchG[ v_q_i ] = v_d_i;
				matchG_pos[ v_d_i ] = v_q_i;
				break;
			
			}//for-3.1 end

		}//for-3 end

		memset(QVisited, 0, vq*sizeof(bool));
		memset(GVisited, 0, v_d*sizeof(bool));

		//compute the edit distance
		label_q = QVertexLabel[ 0 ];

		int v_d_0 = matchG[ 0 ];
		label_d = DVertexLabel[ v_d_0 ];

		QVisited[ 0 ] = true;
		GVisited[ v_d_0 ] = true;

		if(label_q == label_d)
		{
			ged = 0;
		}
		else
		{
			ged = 1;
		}

		LISTINT eLS;
		for(v_q_i = 1; v_q_i<vq; v_q_i++)//for-4
		{
			label_q = QVertexLabel[ v_q_i ];

			int v_d_i = matchG[ v_q_i ];
			label_d = DVertexLabel[ v_d_i ];

			if(label_q != label_d)
			{
				ged++;
			}

			int neiSize_q = QVertexNeiSize[ v_q_i ];
			for(int nei_i_q=0; nei_i_q<neiSize_q; nei_i_q++)
			{
				int nei_id_q = QVertexNeighbor[ v_q_i ][ nei_i_q ];
					int nei_id_q_pos = QVertexId[ nei_id_q ];

					if( QVisited[ nei_id_q_pos ])
					{
						int pos_m = nei_id_q_pos;//int pos_m = matchQ[ nei_id_q_pos ];
						int e_label = QEdgeLabel[ v_q_i ][ nei_i_q ];

						eLS.push_back( 200*e_label + pos_m );
					}
			}

			int neiSize_d = DVertexNeiSize[ v_d_i ];
			ged =  ged + ged_gx_compute(eLS, matchG_pos, neiSize_d, DVertexNeighbor[ v_d_i ], DVertexId, DEdgeLabel[ v_d_i ], GVisited);
	
			eLS.clear();
			QVisited[ v_q_i ] = true;
			GVisited[ v_d_i ] = true;

		}//for-4 end

		for(int v_d_i=0; v_d_i<v_d; v_d_i++)
		{
			if(GVisited[ v_d_i ])
			{
				continue;
			}
			ged++;
			ged = ged + DVertexNeiSize[ v_d_i ];
		
		}

		delete [] matchG_pos;

		delete [] matchQ;
		delete [] matchG;
		delete [] QVisited;
		delete [] GVisited;

		return ged;//

	}// if-1 middle
	else
	{		//Q is larger, and Q is in inner layer
		INT_P matchQ_pos = new int[ vq ];

		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )//for-1
		{
			 label_d = DVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 v_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				v_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {		
				int nei_size_d = DVertexNeiSize[v_d_i];
				int nei_size_q = QVertexNeiSize[v_q_i];
				int nei_i_d=0, nei_i_q=0;
				for( nei_i_d=0, nei_i_q=0; nei_i_d<nei_size_d && nei_i_q<nei_size_q; nei_i_d++, nei_i_q++)//for-1.1
				{
					int e_label_d = DEdgeLabel[v_d_i][nei_i_d];
					int e_label_q = QEdgeLabel[v_q_i][nei_i_q];
					if(e_label_d < e_label_q )
					{											
						v_d_i++;
						break;
					}
					else if(e_label_d > e_label_q )
					{						
						v_q_i++;
						break;
					}
				}//for-1.1 end

				if(nei_i_d == nei_size_d && nei_i_q<nei_size_q )// if-1.1.2
				{				
					v_d_i++;
				}// if-1.1.2 middle
				else if(nei_i_d<nei_size_d && nei_i_q == nei_size_q)
				{
					v_q_i++;
				}// if-1.1.2 middle
				else if(nei_i_d==nei_size_d && nei_i_q == nei_size_q)
				{
					QVisited[ v_q_i ] = true;
					GVisited[ v_d_i ] = true;

					matchQ[ v_d_i ] = v_q_i;
					matchQ_pos[ v_q_i ] = v_d_i;

					v_d_i++;
					v_q_i++;
				}//if-1.1.2 end

			 }// if-1.1 end
		}// for-1 end

		for( v_d_i=0; v_d_i<v_d; v_d_i++)//for-2
		{
			if(GVisited[ v_d_i ])
			{
				continue;
			}

			label_d = DVertexLabel[v_d_i];

			for( v_q_i=0; v_q_i<vq; v_q_i++)//for-2.1
			{
				if(QVisited[ v_q_i ])
				{
					continue;
				}

				label_q = QVertexLabel[ v_q_i ];
				if(label_d == label_q)
				{
					QVisited[ v_q_i ] = true;
					GVisited[ v_d_i ] = true;

					matchQ[ v_d_i ] = v_q_i;
					matchQ_pos[ v_q_i ] = v_d_i;
					break;
				}
				else if(label_q > label_d)
				{
					break;
				}
			
			}//for-2.1 end

		}//for-2 end

		for( v_d_i=0; v_d_i<v_d; v_d_i++)//for-3
		{
			if(GVisited[ v_d_i ])
			{
				continue;
			}

			for( v_q_i=0; v_q_i<vq; v_q_i++)//for-3.1
			{
				if(QVisited[ v_q_i ])
				{
					continue;
				}

				QVisited[ v_q_i ] = true;
				GVisited[ v_d_i ] = true;

				matchQ[ v_d_i ] = v_q_i;
				matchQ_pos[ v_q_i ] = v_d_i;
				break;
			
			}//for-3.1 end

		}//for-3 end

		/*
		for(int v_i=0; v_i<vq; v_i++)
		{
			printf(" %d", matchQ[v_i]);
		}
		*/

		memset(QVisited, 0, vq*sizeof(bool));
		memset(GVisited, 0, v_d*sizeof(bool));

		//compute the edit distance
		label_d = DVertexLabel[ 0 ];

		int v_q_0 = matchQ[ 0 ];
		label_q = QVertexLabel[ v_q_0 ];

		GVisited[ 0 ] = true;
		QVisited[ v_q_0 ] = true;

		if(label_q == label_d)
		{
			ged = 0;
		}
		else
		{
			ged = 1;
		}

		LISTINT eLS;
		for(v_d_i = 1; v_d_i<v_d; v_d_i++)//for-4
		{
			label_d = DVertexLabel[ v_d_i ];

			int v_q_i = matchQ[ v_d_i ];
			label_q = QVertexLabel[ v_q_i ];

			if(label_q != label_d)
			{
				ged++;
			}

			int neiSize_d = DVertexNeiSize[ v_d_i ];
			for(int nei_i_d=0; nei_i_d<neiSize_d; nei_i_d++)
			{
				int nei_id_d = DVertexNeighbor[ v_d_i ][ nei_i_d ];
				int nei_id_d_pos = DVertexId[ nei_id_d ];

				if( GVisited[ nei_id_d_pos ])
				{
					int pos_m = nei_id_d_pos;//int pos_m = matchQ[ nei_id_q_pos ];
					int e_label = DEdgeLabel[ v_d_i ][ nei_i_d ];

					eLS.push_back( 200*e_label + pos_m );
				}
			}

			int neiSize_q = QVertexNeiSize[ v_q_i ];
			ged =  ged + ged_gx_compute(eLS, matchQ_pos, neiSize_q, QVertexNeighbor[ v_q_i ], QVertexId, QEdgeLabel[ v_q_i ], QVisited);
	
			eLS.clear();
			QVisited[ v_q_i ] = true;
			GVisited[ v_d_i ] = true;

		}//for-4 end

		for(int v_q_i=0; v_q_i<vq; v_q_i++)
		{
			if(QVisited[ v_q_i ])
			{
				continue;
			}
			ged++;
			ged = ged + QVertexNeiSize[ v_q_i ];
		
		}

		delete [] matchQ_pos;

		delete [] matchQ;
		delete [] matchG;
		delete [] QVisited;
		delete [] GVisited;

		return ged;//
		
	}

}




int GraphReader::mcs_wangdong(int graphId)
{
	int mcs = 0;//graph edit distance

	int v_d = vd[graphId];

	INT_P GVertexId = graphVertexId[graphId];
	INT_P GVertexLabel = graphVertexLabel[graphId];
	INT_P GVertexNeiSize  = graphVertexNeiSize[graphId];
	INT_P * GVertexNeighbor = graphVertexNeighbor[graphId];
	INT_P * GEdgeLabel = graphEdgeLabel[graphId];

	int * degree_d = new int[v_d];
	int * degree_q = new int[vq];

	memset(degree_d,0,v_d * sizeof(int));
	memset(degree_q,0,vq * sizeof(int));

	for(int v_i_pos_d=0; v_i_pos_d<v_d; v_i_pos_d++)
	{
		int deg = GVertexNeiSize[ v_i_pos_d ];
		degree_d[ deg ]++;
	}

	for(int v_i_pos_q=0; v_i_pos_q<vq; v_i_pos_q++)
	{
		int deg = QVertexNeiSize[ v_i_pos_q ];
		degree_q[ deg ]++;
	}

	int total_d = 0;
	int total_q = 0;

	int max = 0;

	if(v_d >= vq)
	{
		for(int start=vq; start<v_d; start++)
		{
			total_d = total_d + degree_d[start];
		}
		
		int * difference = new int[vq];

		for(int d_i=vq-1; d_i>=0; d_i--)
		{
			total_d = total_d + degree_d[ d_i ];
			total_q = total_q + degree_q[ d_i ];

			difference[ d_i ] = total_q - total_d;
		}

		for(int d_i=0; d_i<vq; d_i++)
		{
			if(difference[ d_i ] > max )
			{
				max = difference[ d_i ];
			}
		}

		delete[] difference;
	}
	else
	{
		for(int start=v_d; start<vq; start++)
		{
			total_q = total_q + degree_q[start];
		}
		
		int * difference = new int[v_d];

		for(int d_i=v_d-1; d_i>=0; d_i--)
		{
			total_d = total_d + degree_d[ d_i ];
			total_q = total_q + degree_q[ d_i ];

			difference[ d_i ] = total_d - total_q;
		}
		
	
		for(int d_i=0; d_i<v_d; d_i++)
		{
			if(difference[ d_i ] > max )
			{
				max = difference[ d_i ];
			}
		}

		delete[] difference;
	}
	
	delete[] degree_d;
	delete[] degree_q;

	if(max%2 ==0)
	{
		max = max/2;
	}
	else
	{
		max = max/2 + 1;
	}

	//estimate the institution operation
	
	int label_d, label_q;

	int v_d_i=0,  v_q_i=0;

	int count1 = 0;

	if(v_d >= vq)
	{
		
		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )//for-1
		{
			 label_d = GVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 v_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				count1++;
				v_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {
				v_d_i++;
				v_q_i++;

			 }// if-1.1 end
		}// for-1 end

		if( v_q_i< vq-1 )
		{
			count1 = count1 + vq - 1 - v_q_i;
		}

	}
	else
	{
		for( v_d_i=0,  v_q_i=0; v_d_i<v_d && v_q_i<vq; )//for-1
		{
			 label_d = GVertexLabel[v_d_i];
			 label_q = QVertexLabel[v_q_i];

			 if(label_d < label_q)// if-1.1
			 {
				 count1++;
				 v_d_i++;
				 continue;
			 }// if-1.1 middle
			 else if(label_d > label_q)
			 {
				v_q_i++;
				continue;
			 }// if-1.1 middle
			 else
			 {
				v_d_i++;
				v_q_i++;

			 }// if-1.1 end
		}// for-1 end

		if( v_d_i< v_d - 1 )
		{
			count1 = count1 + v_d - 1 - v_d_i;
		}
	
	}

	

	if(v_d > vq)
	{
		return (v_d + ed[ graphId ] - vq - eq + 2*max + count1);
	}
	else
	{
		return (vq + eq - v_d - ed[ graphId ]  + 2*max + count1);
	}

}

void GraphReader::DGraphReaderIndex(char fileName_G [])
{
	FILE *stream = fopen( fileName_G, "r" ); 
  
	fscanf( stream, "%d\n",&GN);

	vd = new int[GN];
	ed = new int[GN];

	graphVertexId = new INT_P[GN];
	graphVertexLabel = new INT_P[GN];
	graphVertexNeiSize = new INT_P[GN];
	graphVertexNeighbor = new INT_P *[GN];
	graphEdgeLabel = new INT_P *[GN];

	//graphPos = new INT_P[GN];

	graphStatLabel = new MAP_INT[GN];
	graphContainEdgeLabel = new MAP_IB[GN];

	char ch1;
	int v1,v2,v3;


	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter map_ib_iter;
	for(int g_i=0; g_i<GN; g_i++)
	{
		fscanf(stream, "%c %d %d %d\n", &ch1, &v3,&v1, &v2);
		//printf("%c %d %d %d %d\n",ch1,v3,v1,v2, g_i);
		//if(g_i==5)getchar();
		graphVertexLabel[g_i] = new int[v1];
		graphVertexNeiSize[g_i]= new int[v1];
		
		graphVertexNeighbor[g_i] = new INT_P[v1];
		graphEdgeLabel[g_i] = new INT_P[v1];

		graphVertexId[g_i] = new int[v1];
		//graphPos[g_i] = new int[v1];

		vd[g_i] = v1;
		ed[g_i] = v2;
		
//int all = v1;
		LISTINT * nei_list = new LISTINT[v1];
		LISTINT * neiLabel_list = new LISTINT[v1];
		
		
		int position = 0;
		for(int j=0; j<vd[g_i]; j++)
		{
			fscanf(stream, "%d %d\n", &v1, &v2);
			graphVertexLabel[g_i][v1] = v2;
			//printf("%d %d\n",v1,v2);
			//graphPos[g_i][j] = v1;
			graphVertexId[g_i][v1] = j;

			map_int_iter  = graphStatLabel[g_i].find(v2);
			if(map_int_iter == graphStatLabel[g_i].end())
			{
				graphStatLabel[g_i].insert(pair<int,int>(v2, position));
				position++;
			}
		

		}//read vertices end


		for(int j=0; j<ed[g_i]; j++)//we assume that edges are undirected,the precondition is that the input data have completed the removal of the duplicates
		{
			fscanf(stream, "%d %d %d\n", &v1, &v2, &v3);
			//printf("size=%d edge%d %d %d %d\n", g_i, j, v1, v2,v3);
			nei_list[v1].push_back(v2);
			nei_list[v2].push_back(v1);
			//printf("edge%d %d %d %d\n", j, v1, v2,v3);
			neiLabel_list[v1].push_back(v3);
			neiLabel_list[v2].push_back(v3);
			
			//printf("edge%d %d %d %d\n", j, v1, v2,v3);
			map_ib_iter = graphContainEdgeLabel[g_i].find(v3);
			if(map_ib_iter == graphContainEdgeLabel[g_i].end())
			{
				graphContainEdgeLabel[g_i].insert(pair<int,bool>(v3,0));
			}

			// sort the neighbor of each vertex according to its edge labels
			int sort_i;
			//printf("edge%d %d %d %d\n", j, v1, v2,v3);
			for( sort_i = neiLabel_list[v1].size()-1; sort_i>0; sort_i--)
			{
				if(v3 < neiLabel_list[v1][ sort_i-1 ])
				{
					neiLabel_list[v1][sort_i] = neiLabel_list[v1][ sort_i-1 ];
					nei_list[v1][sort_i] = nei_list[v1][ sort_i-1 ];
				}
				else
				{
					neiLabel_list[v1][sort_i] = v3;
					nei_list[v1][sort_i] = v2;
					break;
				}
			}
			
			if(sort_i == 0)
			{
				neiLabel_list[v1][0] = v3;
				nei_list[v1][0] = v2;
			}

			for( sort_i = neiLabel_list[v2].size()-1; sort_i>0; sort_i--)
			{
				if(v3 < neiLabel_list[v2][ sort_i-1 ])
				{
					neiLabel_list[v2][sort_i] = neiLabel_list[v2][ sort_i-1 ];
					nei_list[v2][sort_i] = nei_list[v2][ sort_i-1 ];
				}
				else
				{
					neiLabel_list[v2][sort_i] = v3;
					nei_list[v2][sort_i] = v1;
					break;
				}
			}
			
			if(sort_i == 0)
			{
				neiLabel_list[v2][0] = v3;
				nei_list[v2][0] = v1;
			}
			//sort end
			
			
		}// read edges end

		//vector —— array
		int nei_size;
		for(int v_i=0; v_i<vd[g_i]; v_i++)
		{
			nei_size = nei_list[v_i].size();
			graphVertexNeiSize[g_i][v_i] = nei_size;

			graphVertexNeighbor[g_i][v_i] = new int[ nei_size ];
			graphEdgeLabel[g_i][v_i] = new int[ nei_size ];

			for(int nei_i=0; nei_i<nei_size; nei_i++)
			{
				graphVertexNeighbor[g_i][v_i][nei_i] = nei_list[v_i][nei_i];
				graphEdgeLabel[g_i][v_i][nei_i] = neiLabel_list[v_i][nei_i];
			}

			nei_list[v_i].clear();
			neiLabel_list[v_i].clear();

		}
		
		delete[] nei_list;
		delete[] neiLabel_list;
	
		fscanf(stream, "\n");
	}//read graphs end

	fclose(stream);

	graphStatistic();
	//getchar();
	//getchar();
}



void GraphReader::BuildInex(char fileName_G [])
{
	//DGraphReaderIndex(fileName_G);

	map_branch = new MAP_Str[200];// assume the vertex label number is 200
	MAP_Str_Iter map_branch_iter;

	graphBranchRoot = new INT_P[GN];
	graphBranchCode = new INT_P[GN];
	graphBranchCount = new INT_P[GN];
	graphBranchSize = new int[GN];


	//devise the branch code for each data graph
	for(int g_i=0; g_i<GN; g_i++)//for start---1
	{
		int vd_g = vd[g_i];

		LISTINT branchRoot;
		LISTINT branchCode;
		LISTINT branchCount;

		for(int pos_i=0; pos_i<vd_g; pos_i++)//for start---1.1
		{
			int label = graphVertexLabel[g_i][pos_i];

			int nei_size = graphVertexNeiSize[g_i][pos_i];

			string str_key;

			

			for(int nei_i=0; nei_i<nei_size; nei_i++)//for start---1.1.1
			{
				int e_label = graphEdgeLabel[g_i][pos_i][nei_i];
				
				//char s_temp[6];
				//itoa(e_label, s_temp, 10);
				//str_key = str_key+" "+ s_temp;
				stringstream ss;
				ss<<e_label;
				string str_t = ss.str();
				str_key = str_key+" "+str_t;


			}//for end---1.1.1

			int branch_id;
			map_branch_iter = map_branch[label].find(str_key);
			if(map_branch_iter == map_branch[label].end())
			{
				branch_id = map_branch[label].size();//start from 0
				map_branch[label].insert(pair<string,int>(str_key, branch_id));
			}
			else
			{
				branch_id = map_branch_iter->second;
			}

			int left = 0;
			int right = branchRoot.size()-1;

			int mid = -1;
			
			bool existed = false;

			while(left <= right)
			{
				mid = (left+right)/2;
				if(label == branchRoot[mid]) 
				{
					if(branch_id == branchCode[mid])
					{
						existed = true;
						break;
					}
					else if(branch_id > branchCode[mid])
					{
						left = mid + 1;
					}
					else
					{
						right = mid - 1;
					}
					
				}
				else if(label > branchRoot[mid]) 
					left = mid + 1;
				else right = mid - 1;
			}

			if(existed)
			{
				ListIntIter iter = branchCount.begin()+mid;//mid
				(*iter)++;
			}
			else// if(left > mid)
			{
				branchRoot.insert(branchRoot.begin()+left, label); //left
				branchCode.insert(branchCode.begin()+left, branch_id);
				branchCount.insert(branchCount.begin()+left, 1);
			}
			

		
		}//for end---1.1

		int branch_size = branchRoot.size();
		graphBranchSize[g_i] = branch_size;
		graphBranchRoot[g_i] = new int[branch_size];
		graphBranchCode[g_i] = new int[branch_size];
		graphBranchCount[g_i] = new int[branch_size];

		for(int branch_i=0; branch_i<branch_size; branch_i++)
		{
			graphBranchRoot[g_i][branch_i] = branchRoot[branch_i];
			graphBranchCode[g_i][branch_i] = branchCode[branch_i];
			graphBranchCount[g_i][branch_i] = branchCount[branch_i];
		}

		branchRoot.clear();
		branchCode.clear();
		branchCount.clear();

	}//for end---1
    
	//output the branches of each graph
	/*
	for(int g_i=884; g_i<885; g_i++)//for start---2
	{
		int branch_size = graphBranchSize[g_i];
		printf("graph %d:\n",g_i);
		for(int branch_i=0; branch_i<branch_size; branch_i++)
		{
			printf("(%d,%d,%d)  ",graphBranchRoot[g_i][branch_i], graphBranchCode[g_i][branch_i], graphBranchCount[g_i][branch_i]);
		}
		printf("\n");
	}//for end---2
	*/

	//memset(Dclique_size, 0, sizeof(int) * vd);常用操作

	//maximalClique_list.clear();//常用操作

	//delete [] lable_bool;//常用操作

	//memset(Dvisited, -1, sizeof(int) * vd);//常用操作

}

void GraphReader::BuildTree()
{
	
	//int ged = branchDistance(vd[6],vd[52], graphBranchRoot[6], graphBranchCode[6], graphBranchCount[6], graphBranchSize[6], graphBranchRoot[52],
		//graphBranchCode[52], graphBranchCount[52], graphBranchSize[52]);

	//int score[ maxChildren+1 ][ maxChildren+1];

	//divide top 11 graphs into two clusters
	int max = 0;
	int max_i = 0;
	int max_j = 1;

	for(int g_i=0; g_i<maxChildren; g_i++)
	{
		for(int g_j=g_i+1; g_j<(maxChildren+1); g_j++)
		{
			score[ g_i ][ g_j ] = branchDistance(vd[g_i], graphBranchRoot[g_i], graphBranchCode[g_i], graphBranchCount[g_i], graphBranchSize[g_i], vd[g_j], graphBranchRoot[g_j],
		                               graphBranchCode[g_j], graphBranchCount[g_j], graphBranchSize[g_j]);
			
			score[g_j][g_i] = score[g_i][g_j];
			if(max < score[ g_i ][ g_j ])
			{
				max = score[g_i][g_j];
				max_i = g_i;
				max_j = g_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT[1];
	LISTINT * node2 = new LISTINT[1];
	
	nodeParent.push_back(-1);//root
	nodeParent.push_back(-1);

	node1[0].push_back(max_i);
	node2[0].push_back(max_j);

	LISTINT * nodeRoot1 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode1 = new LISTINT[1];//[branchPos]=branch code
	LISTINT *nodeCount1 = new LISTINT[1];//[branchPos]=branch count

	int size1 = 0;
	int size2 = 0;

	for(int b_i=0; b_i<graphBranchSize[max_i]; b_i++)
	{
		nodeRoot1[0].push_back(graphBranchRoot[max_i][b_i]);
		nodeCode1[0].push_back(graphBranchCode[max_i][b_i]);
		nodeCount1[0].push_back(graphBranchCount[max_i][b_i]);
		size1 = size1 + graphBranchCount[max_i][b_i];
	}

	LISTINT * nodeRoot2 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode2 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount2 = new LISTINT[1];//[branchPos]=branch count

	for(int b_i=0; b_i<graphBranchSize[max_j]; b_i++)
	{
		nodeRoot2[0].push_back(graphBranchRoot[max_j][b_i]);
		nodeCode2[0].push_back(graphBranchCode[max_j][b_i]);
		nodeCount2[0].push_back(graphBranchCount[max_j][b_i]);
		size2 = size2 + graphBranchCount[max_j][b_i];
	}


	for(int g_i=0; g_i<maxChildren+1; g_i++)
	{
		if(g_i != max_i && g_i != max_j)
		{
			if(score[max_i][g_i] <= score[max_j][g_i])
			{
				node1[0].push_back(g_i);
				
				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot1[0].size()-1, *nodeRoot1, *nodeCode1, *nodeCount1,graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size1 = size1 + count_temp;
				}

			}
			else
			{
				node2[0].push_back(g_i);

				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot2[0].size()-1, *nodeRoot2, *nodeCode2, *nodeCount2, graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size2 = size2 + count_temp;
				}
			}
		}
	
	}

	//nodeSize.push_back(0);
	nodeSize.push_back(size1);
	nodeSize.push_back(size2);

	/*
	LISTINT nodeRoot0;
	LISTINT nodeCode0;
	LISTINT nodeCount0;

	nodeBranchRoot.push_back( & nodeRoot0 );
	nodeBranchRoot.push_back(& nodeCode0);
	nodeBranchCount.push_back(& nodeCount0);
	*/
	
	nodeBranchRoot.push_back( nodeRoot1);
	nodeBranchCode.push_back( nodeCode1);
	nodeBranchCount.push_back( nodeCount1);

	nodeBranchRoot.push_back( nodeRoot2);
	nodeBranchCode.push_back( nodeCode2);
	nodeBranchCount.push_back( nodeCount2);

	rootNode.push_back(GN+0);
	rootNode.push_back(GN+1);

	//nodeChildren.push_back( & rootNode );
	nodeChildren.push_back( node1 );
	nodeChildren.push_back( node2 );


	depth = 0;
	depth2 = 0;
	for(int g_i=maxChildren+1; g_i<GN; g_i++)
	{
		//printf("g_i=%d\n", g_i);
		insert(g_i);//0 represents rootNode
	}

	//printf("depth=%d depth2=%d\n", depth, depth2);
	
}

void GraphReader::insert(int g_i)
{

	int min_node = -1;

	LISTINT * children = & rootNode;
	
	while(true)//while start---1
	{
		bool is_leaf = false;
		
		int distance = 10000000;

		//printf("g_i=%d here1\n", g_i);
		for(int c_i=0; c_i< children->size(); c_i++)
		{
			int nodeId = children->at( c_i );
			if(nodeId < GN)
			{
				is_leaf = true;
				break;
			}

			nodeId = nodeId % GN;

			int temp_distance = branchDistanceINT_List(vd[g_i], graphBranchRoot[g_i], graphBranchCode[g_i], graphBranchCount[g_i], graphBranchSize[g_i],
				nodeSize[nodeId], *nodeBranchRoot[nodeId], *nodeBranchCode[nodeId], *nodeBranchCount[nodeId], nodeBranchRoot[nodeId]->size()); ;

			if(distance > temp_distance)
			{
				distance = temp_distance;
				min_node = nodeId;
			}
		}

		//printf("g_i=%d here2\n", g_i);

		if(is_leaf)
		{

			//printf("g_i=%d here3\n", g_i);
			children->push_back(g_i);
			if(children->size() > maxChildren)//full
			{
				//printf("g_i=%d here4\n", g_i);
				depth++;
				//split(min_node);
				
				//searchNode(&rootNode, -1,6);//flag
				//printf(".........................\n");
				splitMul(min_node);
				//searchNode(&rootNode, -1,6);//flag
				//printf(".........................\n");
			}
			break;
		}
		//printf("g_i=%d here5\n", g_i);
		//printf("distance=%d\n", distance);

		children = nodeChildren[ min_node ];
		for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
		{

			int count_temp = binaryInsert(0, nodeBranchRoot[ min_node]->size()-1, *nodeBranchRoot[ min_node], *nodeBranchCode[ min_node], *nodeBranchCount[ min_node], graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

			nodeSize[ min_node ] = nodeSize[ min_node ] + count_temp;
		}

		//printf("g_i=%d here6\n", g_i);

	}// while end---1
}

int GraphReader::directedDistanceINT_List(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, LISTINT &branchRoot_b, LISTINT &branchCode_b, LISTINT &branchCount_b, int branchSize_b)
{									   
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	//INT_P commonCount = new int[ branchSize_a ];
	//memset(commonCount, 0, branchSize_a * sizeof(int));

	int label_a, label_b;
	int a_i=0, b_i=0;


	int last_label = -1;
	int passed_a = 0;
	int passed_b = 0;

	for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
	{
		label_a = branchRoot_a[ a_i ];
		label_b = branchRoot_b[ b_i ];
		if(label_a < label_b )
		{
			if(label_a == last_label)
			{
				passed_a = passed_a + branchCount_a[a_i];
				a_i++;
				continue;
			}
			count1 = count1 + branchCount_a[ a_i ];
			a_i++;
			continue;
		}
		else if(label_a > label_b )
		{
			if(label_b == last_label)
			{
				passed_b = passed_b + branchCount_b[b_i];
				b_i++;
				continue;
			}
			b_i++;
			continue;
		}
		else//equal label
		{
	
			if(label_a != last_label)
			{
				if(passed_a >= passed_b )
				{
					count1 = count1 + passed_a - passed_b;
					count2 = count2 + passed_b;
				}
				else
				{
					count2 = count2 + passed_a;
				}

				passed_a = 0;
				passed_b = 0;
				last_label = label_a;
				
			}

			if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
			{
				passed_b = passed_b + branchCount_b[ b_i ];
				b_i++;
			}
			else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
			{
				passed_a = passed_a + branchCount_a[ a_i ];
				a_i++;
			}
			else
			{
				if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
				{
					passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
				}
				else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
				{
					passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
				}

				a_i++;
				b_i++;
			}
				
		}
	}//for end---1
	
	for( ; a_i<branchSize_a; a_i++)
	{
		if(branchRoot_a[ a_i ] != last_label)
		{
			break;
		}
		passed_a = passed_a + branchCount_a[ a_i ];
	}

	for( ; b_i<branchSize_b; b_i++)
	{
		if(branchRoot_b[ b_i ] != last_label)
		{
			break;
		}
		passed_b = passed_b + branchCount_b[ b_i ];
	}

	if(passed_a >= passed_b)
	{
		count1 = count1 + passed_a - passed_b;
		count2 = count2 + passed_b;
	}
	else
	{
		count2 = count2 + passed_a;
	}

	for( ; a_i<branchSize_a; a_i++)
	{
		count1 = count1 + branchCount_a[ a_i ];
	}
	

	if(count2%2 == 0)
	{
		ged = count1 + count2/2;
	}
	else
	{
		ged = count1 + count2/2+1;
	}
	
	return ged;

}


void GraphReader::split(int node)
{
	//int score[ maxChildren+1 ][ maxChildren+1];

	//divide top 11 graphs into two clusters
	int max = 0;
	int max_i = 0;
	int max_j = 1;

	int pos_i = 0;
	int pos_j = 0;

	for(int c_i=0; c_i < (nodeChildren[node]->size()-1); c_i++)
	{
		int g_i = nodeChildren[node]->at( c_i );

		for(int c_j=c_i+1; c_j < nodeChildren[node]->size(); c_j++)
		{
			int g_j = nodeChildren[node]->at( c_j );
			score[ c_i ][ c_j ] = branchDistance(vd[g_i], graphBranchRoot[g_i], graphBranchCode[g_i], graphBranchCount[g_i], graphBranchSize[g_i], vd[g_j], graphBranchRoot[g_j],
		                               graphBranchCode[g_j], graphBranchCount[g_j], graphBranchSize[g_j]);
			
			score[c_j][c_i] = score[c_i][c_j];
			if(max < score[ c_i ][ c_j ])
			{
				max = score[c_i][c_j];
				max_i = g_i;
				max_j = g_j;

				pos_i = c_i;
				pos_j = c_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT [1];
	LISTINT * node2 = new LISTINT [1];;
	
	nodeParent.push_back(node);//root
	nodeParent.push_back(node);

	node1->push_back(max_i);
	node2->push_back(max_j);

	LISTINT * nodeRoot1 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode1 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount1 = new LISTINT[1];//[branchPos]=branch count

	int size1 = 0;
	int size2 = 0;

	for(int b_i=0; b_i<graphBranchSize[max_i]; b_i++)
	{
		nodeRoot1->push_back(graphBranchRoot[max_i][b_i]);
		nodeCode1->push_back(graphBranchCode[max_i][b_i]);
		nodeCount1->push_back(graphBranchCount[max_i][b_i]);
		size1 = size1 + graphBranchCount[max_i][b_i];
	}

	LISTINT * nodeRoot2 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode2 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount2 = new LISTINT[1];//[branchPos]=branch count

	for(int b_i=0; b_i<graphBranchSize[max_j]; b_i++)
	{
		nodeRoot2[0].push_back(graphBranchRoot[max_j][b_i]);
		nodeCode2[0].push_back(graphBranchCode[max_j][b_i]);
		nodeCount2[0].push_back(graphBranchCount[max_j][b_i]);
		size2 = size2 + graphBranchCount[max_j][b_i];
	}


	for(int c_i=0; c_i<nodeChildren[node]->size(); c_i++)
	{
		int g_i = nodeChildren[node]->at( c_i );
		if(g_i != max_i && g_i != max_j)
		{
			if(score[pos_i][c_i] <= score[pos_j][c_i])
			{
				node1->push_back(g_i);
				
				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot1->size()-1, *nodeRoot1, *nodeCode1, *nodeCount1,graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size1 = size1 + count_temp;
				}

			}
			else
			{
				node2[0].push_back(g_i);

				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot2[0].size()-1, nodeRoot2[0], nodeCode2[0], nodeCount2[0],graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size2 = size2 + count_temp;
				}
			}
		}
	
	}

	//nodeSize.push_back(0);
	nodeSize.push_back(size1);
	nodeSize.push_back(size2);

	/*
	LISTINT nodeRoot0;
	LISTINT nodeCode0;
	LISTINT nodeCount0;

	nodeBranchRoot.push_back( & nodeRoot0 );
	nodeBranchRoot.push_back(& nodeCode0);
	nodeBranchCount.push_back(& nodeCount0);
	*/
	

	nodeChildren[node]->clear();
	nodeChildren[node]->push_back( GN + nodeChildren.size() );
	nodeChildren[node]->push_back( GN + nodeChildren.size() + 1 );

	nodeChildren.push_back( node1 );
	nodeChildren.push_back( node2 );


	nodeBranchRoot.push_back( nodeRoot1);
	nodeBranchCode.push_back( nodeCode1);
	nodeBranchCount.push_back( nodeCount1);

	nodeBranchRoot.push_back( nodeRoot2);
	nodeBranchCode.push_back( nodeCode2);
	nodeBranchCount.push_back( nodeCount2);

	//nodeChildren.push_back( & rootNode );

}

void GraphReader::splitMul(int node)
{
	//int score[ maxChildren+1 ][ maxChildren+1];

	//divide top 11 graphs into two clusters
	int max = 0;
	int max_i = 0;
	int max_j = 1;

	int pos_i = 0;
	int pos_j = 0;

	for(int c_i=0; c_i < (nodeChildren[node]->size()-1); c_i++)
	{
		int g_i = nodeChildren[node]->at( c_i );

		for(int c_j=c_i+1; c_j < nodeChildren[node]->size(); c_j++)
		{
			int g_j = nodeChildren[node]->at( c_j );
			score[ c_i ][ c_j ] = branchDistance(vd[g_i], graphBranchRoot[g_i], graphBranchCode[g_i], graphBranchCount[g_i], graphBranchSize[g_i], vd[g_j], graphBranchRoot[g_j],
		                               graphBranchCode[g_j], graphBranchCount[g_j], graphBranchSize[g_j]);
			
			score[c_j][c_i] = score[c_i][c_j];
			if(max < score[ c_i ][ c_j ])
			{
				max = score[c_i][c_j];
				max_i = g_i;
				max_j = g_j;

				pos_i = c_i;
				pos_j = c_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT [1];
	LISTINT * node2 = new LISTINT [1];;
	
	int parent = nodeParent[node];

	nodeParent.push_back(parent);//for node2

	node1->push_back(max_i);
	node2->push_back(max_j);

	LISTINT * nodeRoot1 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode1 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount1 = new LISTINT[1];//[branchPos]=branch count

	int size1 = 0;
	int size2 = 0;

	for(int b_i=0; b_i<graphBranchSize[max_i]; b_i++)
	{
		nodeRoot1->push_back(graphBranchRoot[max_i][b_i]);
		nodeCode1->push_back(graphBranchCode[max_i][b_i]);
		nodeCount1->push_back(graphBranchCount[max_i][b_i]);
		size1 = size1 + graphBranchCount[max_i][b_i];
	}

	LISTINT * nodeRoot2 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode2 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount2 = new LISTINT[1];//[branchPos]=branch count

	for(int b_i=0; b_i<graphBranchSize[max_j]; b_i++)
	{
		nodeRoot2[0].push_back(graphBranchRoot[max_j][b_i]);
		nodeCode2[0].push_back(graphBranchCode[max_j][b_i]);
		nodeCount2[0].push_back(graphBranchCount[max_j][b_i]);
		size2 = size2 + graphBranchCount[max_j][b_i];
	}


	for(int c_i=0; c_i<nodeChildren[node]->size(); c_i++)
	{
		int g_i = nodeChildren[node]->at( c_i );

		if(g_i != max_i && g_i != max_j)
		{
			if(score[pos_i][c_i] <= score[pos_j][c_i])
			{
				node1->push_back(g_i);
				
				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot1->size()-1, *nodeRoot1, *nodeCode1, *nodeCount1,graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size1 = size1 + count_temp;
				}

				/*
				if(g_i == 884)
				{
					printf("parent = %d\n",node);
				}
				*/

			}
			else
			{
				/*
				if(g_i == 6)
				{
					printf("parent = %d\n",nodeSize.size());
				}
				*/

				node2[0].push_back(g_i);

				for(int b_i=0; b_i<graphBranchSize[g_i]; b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot2[0].size()-1, nodeRoot2[0], nodeCode2[0], nodeCount2[0],graphBranchRoot[g_i][b_i], graphBranchCode[g_i][b_i], graphBranchCount[g_i][b_i]);

					size2 = size2 + count_temp;
				}
			}
		}
	
	}

	//nodeSize.push_back(0);
	nodeSize[node] = size1;

	delete [] nodeChildren[node];
	nodeChildren[node] = node1 ;

	nodeSize.push_back(size2);

	/*
	LISTINT nodeRoot0;
	LISTINT nodeCode0;
	LISTINT nodeCount0;

	nodeBranchRoot.push_back( & nodeRoot0 );
	nodeBranchRoot.push_back(& nodeCode0);
	nodeBranchCount.push_back(& nodeCount0);
	*/
	if(parent == -1)
	{
		rootNode.push_back( GN + nodeChildren.size() );
	}
	else
	{
		nodeChildren[parent]->push_back( GN + nodeChildren.size() );
	}
	

	nodeChildren.push_back( node2 );


	delete [] nodeBranchRoot[node];
	delete [] nodeBranchCode[node];
	delete [] nodeBranchCount[node];

	nodeBranchRoot[node] = nodeRoot1;
	nodeBranchCode[node] = nodeCode1;
	nodeBranchCount[node] = nodeCount1;

	nodeBranchRoot.push_back( nodeRoot2);
	nodeBranchCode.push_back( nodeCode2);
	nodeBranchCount.push_back( nodeCount2);


	splitMul_sub(parent);


	//nodeChildren.push_back( & rootNode );

}

void GraphReader::splitMul_sub(int node)
{
	//int score[ maxChildren+1 ][ maxChildren+1];

	//divide top 11 graphs into two clusters

	LISTINT * children;
	if(node == -1)
	{
		children = & rootNode;
	}
	else
	{
		children = nodeChildren[node];
	}

	if(children->size() <= maxChildren)
	{
		return;
	}

	int max = 0;
	int max_i = 0;
	int max_j = 1;

	int pos_i = 0;
	int pos_j = 0;

	for(int c_i=0; c_i < (children->size()-1); c_i++)
	{
		int n_i = children->at( c_i ) % GN;

		for(int c_j=c_i+1; c_j < children->size(); c_j++)
		{
			int n_j = children->at( c_j ) % GN;
			score[ c_i ][ c_j ] = branchDistanceList(nodeSize[n_i], *nodeBranchRoot[n_i], *nodeBranchCode[n_i], *nodeBranchCount[n_i], nodeBranchRoot[n_i]->size(), 
													 nodeSize[n_j], *nodeBranchRoot[n_j], *nodeBranchCode[n_j], *nodeBranchCount[n_j], nodeBranchRoot[n_j]->size());
			
			score[c_j][c_i] = score[c_i][c_j];
			if(max < score[ c_i ][ c_j ])
			{
				max = score[c_i][c_j];
				max_i = n_i;
				max_j = n_j;

				pos_i = c_i;
				pos_j = c_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT [1];
	LISTINT * node2 = new LISTINT [1];;
	
	node1->push_back(max_i+GN);
	node2->push_back(max_j+GN);

	LISTINT * nodeRoot1 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode1 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount1 = new LISTINT[1];//[branchPos]=branch count

	int size1 = 0;
	int size2 = 0;

	for(int b_i=0; b_i<nodeBranchRoot[max_i]->size(); b_i++)
	{
		nodeRoot1->push_back(nodeBranchRoot[max_i]->at(b_i) );
		nodeCode1->push_back(nodeBranchCode[max_i]->at(b_i) );
		nodeCount1->push_back(nodeBranchCount[max_i]->at(b_i) );
		size1 = size1 + nodeBranchCount[max_i]->at(b_i);
	}

	LISTINT * nodeRoot2 = new LISTINT[1];//branchPos]=Rootlabel
	LISTINT * nodeCode2 = new LISTINT[1];//[branchPos]=branch code
	LISTINT * nodeCount2 = new LISTINT[1];//[branchPos]=branch count

	for(int b_i=0; b_i<nodeBranchRoot[max_j]->size(); b_i++)
	{
		nodeRoot2[0].push_back(nodeBranchRoot[max_j]->at(b_i) );
		nodeCode2[0].push_back(nodeBranchCode[max_j]->at(b_i) );
		nodeCount2[0].push_back(nodeBranchCount[max_j]->at(b_i) );
		size2 = size2 + nodeBranchCount[max_j]->at(b_i);
	}


	for(int c_i=0; c_i<children->size(); c_i++)
	{
		int n_i = children->at( c_i ) % GN;
		if(n_i != max_i && n_i != max_j)
		{
			if(score[pos_i][c_i] <= score[pos_j][c_i])
			{
				node1->push_back(n_i+GN);
				
				for(int b_i=0; b_i<nodeBranchRoot[ n_i ]->size(); b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot1->size()-1, *nodeRoot1, *nodeCode1, *nodeCount1,nodeBranchRoot[n_i]->at(b_i), nodeBranchCode[n_i]->at(b_i), nodeBranchCount[n_i]->at(b_i) );

					size1 = size1 + count_temp;
				}

			}
			else
			{
				node2[0].push_back(n_i+GN);

				for(int b_i=0; b_i<nodeBranchRoot[n_i]->size(); b_i++)
				{

					int count_temp = binaryInsert(0, nodeRoot2[0].size()-1, nodeRoot2[0], nodeCode2[0], nodeCount2[0],nodeBranchRoot[n_i]->at(b_i), nodeBranchCode[n_i]->at(b_i), nodeBranchCount[n_i]->at(b_i) );

					size2 = size2 + count_temp;
				}
			}
		}
	
	}


	//nodeSize.push_back(0);
	if(node != -1)
	{
		int parent = nodeParent[node];

		nodeParent.push_back(parent);//for node2

		nodeSize[node] = size1;

		delete [] nodeChildren[node];
		nodeChildren[node] = node1;

		nodeSize.push_back(size2);

		if(parent != -1)
		{
			nodeChildren[parent]->push_back( GN + nodeChildren.size() );//for node2
		}
		else
		{
			rootNode.push_back( GN + nodeChildren.size() );//for node2
		}
		
		nodeChildren.push_back( node2 );

		
		delete [] nodeBranchRoot[node];
		delete [] nodeBranchCode[node];
		delete [] nodeBranchCount[node];

		nodeBranchRoot[node] = nodeRoot1;
		nodeBranchCode[node] = nodeCode1;
		nodeBranchCount[node] = nodeCount1;

		nodeBranchRoot.push_back( nodeRoot2);
		nodeBranchCode.push_back( nodeCode2);
		nodeBranchCount.push_back( nodeCount2);

		splitMul_sub(parent);

	}
	else
	{
		depth2++;
		rootNode.clear();

		rootNode.push_back( GN + nodeChildren.size() );//for node1
		rootNode.push_back( GN + nodeChildren.size() +1);//for node2

		nodeParent.push_back(-1);//for node1
		nodeParent.push_back(-1);//for node2
	
		nodeSize.push_back(size1);
		nodeSize.push_back(size2);

		nodeChildren.push_back( node1 );
		nodeChildren.push_back( node2 );

		nodeBranchRoot.push_back( nodeRoot1);
		nodeBranchCode.push_back( nodeCode1);
		nodeBranchCount.push_back( nodeCount1);

		nodeBranchRoot.push_back( nodeRoot2);
		nodeBranchCode.push_back( nodeCode2);
		nodeBranchCount.push_back( nodeCount2);

	}


	//nodeChildren.push_back( & rootNode );

}


int GraphReader::binaryInsert(int left, int right, LISTINT &branchRoot, LISTINT &branchCode, LISTINT &branchCount, int label, int branch_id, int count)
{
	int mid = -1;
			
	bool existed = false;

	while(left <= right)
	{
		mid = (left+right)/2;
		if(label == branchRoot[mid]) 
		{
			if(branch_id == branchCode[mid])
			{
				existed = true;
				break;
			}
			else if(branch_id > branchCode[mid])
			{
				left = mid + 1;
			}
			else
			{
				right = mid - 1;
			}
					
		}
		else if(label > branchRoot[mid]) 
			left = mid + 1;
		else right = mid - 1;
	}

	if(existed)
	{
		ListIntIter iter = branchCount.begin()+mid;//mid
		//(*iter) = (*iter) + count;
		if((*iter) < count )
		{
			*iter = count;
			return count;
		}

		return 0;
	}
	else// if(left > mid)
	{
		branchRoot.insert(branchRoot.begin()+left, label); //left
		branchCode.insert(branchCode.begin()+left, branch_id);
		branchCount.insert(branchCount.begin()+left, count);

		return count;
	}


}

int GraphReader::branchDistance(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, INT_P branchRoot_b, INT_P branchCode_b, INT_P branchCount_b, int branchSize_b)
{

	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	//INT_P commonCount = new int[ branchSize_a ];
	//memset(commonCount, 0, branchSize_a * sizeof(int));

	int label_a, label_b;
	int a_i=0, b_i=0;

	if(aSize >= bSize)// if start---1
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			label_b = branchRoot_b[ b_i ];
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				count1 = count1 + branchCount_a[ a_i ];
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a >= passed_b )
					{
						count1 = count1 + passed_a - passed_b;
						count2 = count2 + passed_b;
					}
					else
					{
						count2 = count2 + passed_a;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a >= passed_b)
		{
			count1 = count1 + passed_a - passed_b;
			count2 = count2 + passed_b;
		}
		else
		{
			count2 = count2 + passed_a;
		}

		for( ; a_i<branchSize_a; a_i++)
		{
			count1 = count1 + branchCount_a[ a_i ];
		}
		
	
	}//if middle---1
	else
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			label_b = branchRoot_b[ b_i ];
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				count1 = count1 + branchCount_b[ b_i ];
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a <= passed_b )
					{
						count1 = count1 + passed_b - passed_a;
						count2 = count2 + passed_a;
					}
					else
					{
						count2 = count2 + passed_b;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a <= passed_b)
		{
			count1 = count1 + passed_b - passed_a;
			count2 = count2 + passed_a;
		}
		else
		{
			count2 = count2 + passed_b;
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			count1 = count1 + branchCount_b[ b_i ];
		}
		
	}//if end---1

	if(count2%2 == 0)
	{
		ged = count1 + count2/2;
	}
	else
	{
		ged = count1 + count2/2+1;
	}
	
	return ged;

}

int GraphReader::branchDistanceList(int aSize, LISTINT &branchRoot_a, LISTINT &branchCode_a, LISTINT &branchCount_a, int branchSize_a, int bSize, LISTINT &branchRoot_b, LISTINT &branchCode_b, LISTINT &branchCount_b, int branchSize_b)
{
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	//INT_P commonCount = new int[ branchSize_a ];
	//memset(commonCount, 0, branchSize_a * sizeof(int));

	int label_a, label_b;
	int a_i=0, b_i=0;

	if(aSize >= bSize)// if start---1
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			label_b = branchRoot_b[ b_i ];
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				count1 = count1 + branchCount_a[ a_i ];
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a >= passed_b )
					{
						count1 = count1 + passed_a - passed_b;
						count2 = count2 + passed_b;
					}
					else
					{
						count2 = count2 + passed_a;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a >= passed_b)
		{
			count1 = count1 + passed_a - passed_b;
			count2 = count2 + passed_b;
		}
		else
		{
			count2 = count2 + passed_a;
		}

		for( ; a_i<branchSize_a; a_i++)
		{
			count1 = count1 + branchCount_a[ a_i ];
		}
		
	
	}//if middle---1
	else
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			label_b = branchRoot_b[ b_i ];
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				count1 = count1 + branchCount_b[ b_i ];
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a <= passed_b )
					{
						count1 = count1 + passed_b - passed_a;
						count2 = count2 + passed_a;
					}
					else
					{
						count2 = count2 + passed_b;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a <= passed_b)
		{
			count1 = count1 + passed_b - passed_a;
			count2 = count2 + passed_a;
		}
		else
		{
			count2 = count2 + passed_b;
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			count1 = count1 + branchCount_b[ b_i ];
		}
		
	}//if end---1

	if(count2%2 == 0)
	{
		ged = count1 + count2/2;
	}
	else
	{
		ged = count1 + count2/2+1;
	}
	
	return ged;

}

int GraphReader::branchDistanceINT_List(int aSize, INT_P branchRoot_a, INT_P branchCode_a, INT_P branchCount_a, int branchSize_a, int bSize, LISTINT &branchRoot_b, LISTINT &branchCode_b, LISTINT &branchCount_b, int branchSize_b)
{
	int ged = 0;//graph edit distance
	int count1 = 0;// the number of mismatches due to vertex label
	int count2 = 0;// the number of mismatches due to edge label

	//INT_P commonCount = new int[ branchSize_a ];
	//memset(commonCount, 0, branchSize_a * sizeof(int));

	int label_a, label_b;
	int a_i=0, b_i=0;

	if(aSize >= bSize)// if start---1
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			label_b = branchRoot_b[ b_i ];
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				count1 = count1 + branchCount_a[ a_i ];
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a >= passed_b )
					{
						count1 = count1 + passed_a - passed_b;
						count2 = count2 + passed_b;
					}
					else
					{
						count2 = count2 + passed_a;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a >= passed_b)
		{
			count1 = count1 + passed_a - passed_b;
			count2 = count2 + passed_b;
		}
		else
		{
			count2 = count2 + passed_a;
		}

		for( ; a_i<branchSize_a; a_i++)
		{
			count1 = count1 + branchCount_a[ a_i ];
		}
		
	
	}//if middle---1
	else
	{
		int last_label = -1;
		int passed_a = 0;
		int passed_b = 0;

		for( a_i=0, b_i=0; a_i<branchSize_a && b_i<branchSize_b; )//for_start---1
		{
			label_a = branchRoot_a[ a_i ];
			
			label_b = branchRoot_b[ b_i ];
		
			if(label_a < label_b )
			{
				if(label_a == last_label)
				{
					passed_a = passed_a + branchCount_a[a_i];
					a_i++;
					continue;
				}
				a_i++;
				continue;
			}
			else if(label_a > label_b )
			{
				if(label_b == last_label)
				{
					passed_b = passed_b + branchCount_b[b_i];
					b_i++;
					continue;
				}
				count1 = count1 + branchCount_b[ b_i ];
				b_i++;
				continue;
			}
			else//equal label
			{
	
				if(label_a != last_label)
				{
					if(passed_a <= passed_b )
					{
						count1 = count1 + passed_b - passed_a;
						count2 = count2 + passed_a;
					}
					else
					{
						count2 = count2 + passed_b;
					}

					passed_a = 0;
					passed_b = 0;
					last_label = label_a;
				
				}

				if(branchCode_a[ a_i ] > branchCode_b[ b_i ])
				{
					passed_b = passed_b + branchCount_b[ b_i ];
					b_i++;
				}
				else if(branchCode_a[ a_i ] < branchCode_b[ b_i ] )
				{
					passed_a = passed_a + branchCount_a[ a_i ];
					a_i++;
				}
				else
				{
					if(branchCount_a[ a_i ] > branchCount_b[ b_i ])
					{
						passed_a = passed_a + branchCount_a[ a_i ] - branchCount_b[ b_i ];
					}
					else if(branchCount_a[ a_i ] < branchCount_b[ b_i] )
					{
						passed_b =  passed_b + branchCount_b[ b_i ] - branchCount_a[ a_i ];
					}

					a_i++;
					b_i++;
				}
				
			}
		}//for end---1
	
		for( ; a_i<branchSize_a; a_i++)
		{
			if(branchRoot_a[ a_i ] != last_label)
			{
				break;
			}
			passed_a = passed_a + branchCount_a[ a_i ];
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			if(branchRoot_b[ b_i ] != last_label)
			{
				break;
			}
			passed_b = passed_b + branchCount_b[ b_i ];
		}

		if(passed_a <= passed_b)
		{
			count1 = count1 + passed_b - passed_a;
			count2 = count2 + passed_a;
		}
		else
		{
			count2 = count2 + passed_b;
		}

		for( ; b_i<branchSize_b; b_i++)
		{
			count1 = count1 + branchCount_b[ b_i ];
		}
		
	}//if end---1

	if(count2%2 == 0)
	{
		ged = count1 + count2/2;
	}
	else
	{
		ged = count1 + count2/2+1;
	}
	
	return ged;

}

void GraphReader::QGraphReaderIndex(char fileName_Q [])//QGraphReader start
{
	
	FILE *stream = fopen( fileName_Q, "r" ); 

	char ch1;//每行的第一个字符
	int  v_temp;
	fscanf( stream, "%c %d %d %d\n", &ch1,&v_temp, &vq,&eq);
	
	QVertexLabel = new int[vq];
	QVertexNeiSize = new int[vq];
	QVertexNeighbor = new INT_P[vq];
	QEdgeLabel = new INT_P[vq];
	QVertexId = new int[vq];
	QPos = new int[vq];

	QVertexUsed = new bool[vq];
	QEdgeUsed = new bool *[vq];


	int v1,v2,v3;

	//读取顶点的label
	for(int v_i=0; v_i <vq; v_i++)//此处是顶点的label
	{
		fscanf( stream, "%d %d\n",  &v1, &v2);
             // printf("%d %d\n", v1, v2);
		QVertexLabel[v1]=v2;//此时v2是Label  //i----v1
		QPos[v_i] = v1;
		QVertexId[v1] = v_i;

		QEdgeUsed[v_i] = new bool[vq];
		//QEdgeVisited[v_i] = new bool [vq];
	}

	LISTINT * nei_list = new LISTINT[vq];
	LISTINT * neiLabel_list = new LISTINT[vq];

	//QEdgeIdLabel = new int[eq];

	for(int e_i=0; e_i<eq; e_i++)//we assume that edges are undirected,the precondition is that the input data have completed the removal of the duplicates
	{
		fscanf(stream, "%d %d %d\n",  &v1, &v2, &v3);
		//printf("%d %d %d\n", v1, v2, v3);
		nei_list[v1].push_back(v2);
		nei_list[v2].push_back(v1);
			
		neiLabel_list[v1].push_back(v3);
		neiLabel_list[v2].push_back(v3);

		/*
		int key=0;
		if(v1<v2)
		{
			key = v1*200+v2;
		}
		else
		{
			key = v2*200+v1;
		}

		//QEdgeId.insert(pair<int,int>(key,e_i));

		//QEdgeIdLabel[e_i] = v3;

		*/

		// sort the neighbor of each vertex according to its edge labels
		int sort_i;
		for( sort_i= neiLabel_list[v1].size()-1; sort_i>0; sort_i--)
		{
			if(v3 < neiLabel_list[v1][ sort_i-1 ])
			{
				neiLabel_list[v1][sort_i] = neiLabel_list[v1][ sort_i-1 ];
				nei_list[v1][sort_i] = nei_list[v1][ sort_i-1 ];
			}
			else
			{
				neiLabel_list[v1][sort_i] = v3;
				nei_list[v1][sort_i] = v2;
				break;
			}
		}
		if(sort_i == 0)
		{
			neiLabel_list[v1][0] = v3;
			nei_list[v1][0] = v2;
		}
			
		for( sort_i = neiLabel_list[v2].size()-1; sort_i>0; sort_i--)
		{
			if(v3 < neiLabel_list[v2][ sort_i-1 ])
			{
				neiLabel_list[v2][sort_i] = neiLabel_list[v2][ sort_i-1 ];
				nei_list[v2][sort_i] = nei_list[v2][ sort_i-1 ];
			}
			else
			{
				neiLabel_list[v2][sort_i] = v3;
				nei_list[v2][sort_i] = v1;
				break;
			}
		}

		if(sort_i == 0)
		{
			neiLabel_list[v2][0] = v3;
			nei_list[v2][0] = v1;
		}
		//sort end
			
	}// read edges end

	//vector —— array
	int nei_size;
	for(int v_i=0; v_i<vq; v_i++)
	{
		nei_size = nei_list[v_i].size();
		QVertexNeiSize[v_i] = nei_size;

		QVertexNeighbor[v_i] = new int[ nei_size ];
		QEdgeLabel[v_i] = new int[ nei_size ];

		for(int nei_i=0; nei_i<nei_size; nei_i++)
		{
			QVertexNeighbor[v_i][nei_i] = nei_list[v_i][nei_i];
			QEdgeLabel[v_i][nei_i] = neiLabel_list[v_i][nei_i];
		}

		nei_list[v_i].clear();
		neiLabel_list[v_i].clear();

	}
	
	delete[] nei_list;
	delete[] neiLabel_list;

	fclose(stream);

	//graphStatistic();//statistic

	
	delete [] QPos;


	
	//memset(Qvisited, -1, sizeof(int) * vq);

	//memset(Qyet, 0, sizeof(bool) * vq);
 
}//QGraphReader end

void GraphReader::QIndex(char fileName_Q [])
{
		QGraphReaderIndex(fileName_Q);

		LISTINT branchRoot;
		LISTINT branchCode;
		LISTINT branchCount;

		MAP_Str_Iter map_branch_iter;
		for(int pos_i=0; pos_i<vq; pos_i++)//for start---1.1
		{
			int label = QVertexLabel[pos_i];

			int nei_size = QVertexNeiSize[pos_i];

			string str_key;
			

			for(int nei_i=0; nei_i<nei_size; nei_i++)//for start---1.1.1
			{
				int e_label = QEdgeLabel[pos_i][nei_i];
				
				//char s_temp[6];
				//itoa(e_label, s_temp, 10);
				//str_key = str_key+" "+ s_temp;
				stringstream ss;
				ss<<e_label;
				string str_t = ss.str();
				str_key = str_key+" "+str_t;

			}//for end---1.1.1

			int branch_id;
			map_branch_iter = map_branch[label].find(str_key);
			if(map_branch_iter == map_branch[label].end())
			{
				branch_id = map_branch[label].size();//start from 0
				map_branch[label].insert(pair<string,int>(str_key, branch_id));
			}
			else
			{
				branch_id = map_branch_iter->second;
			}

			int left = 0;
			int right = branchRoot.size()-1;

			int mid = -1;
			
			bool existed = false;

			while(left <= right)
			{
				mid = (left+right)/2;
				if(label == branchRoot[mid]) 
				{
					if(branch_id == branchCode[mid])
					{
						existed = true;
						break;
					}
					else if(branch_id > branchCode[mid])
					{
						left = mid + 1;
					}
					else
					{
						right = mid - 1;
					}
					
				}
				else if(label > branchRoot[mid]) 
					left = mid + 1;
				else right = mid - 1;
			}

			if(existed)
			{
				ListIntIter iter = branchCount.begin()+mid;//mid
				(*iter)++;
			}
			else// if(left > mid)
			{
				branchRoot.insert(branchRoot.begin()+left, label); //left
				branchCode.insert(branchCode.begin()+left, branch_id);
				branchCount.insert(branchCount.begin()+left, 1);
			}
			

		
		}//for end---1.1

		int branch_size = branchRoot.size();
		QBranchSize = branch_size;
		QBranchRoot = new int[branch_size];
		QBranchCode = new int[branch_size];
		QBranchCount = new int[branch_size];

		for(int branch_i=0; branch_i<branch_size; branch_i++)
		{
			QBranchRoot[branch_i] = branchRoot[branch_i];
			QBranchCode[branch_i] = branchCode[branch_i];
			QBranchCount[branch_i] = branchCount[branch_i];
		}

		branchRoot.clear();
		branchCode.clear();
		branchCount.clear();

		QBranchSize = branch_size;

		/*
		for(int i=0; i<branch_size; i++)
		{
			printf("(%d, %d, %d) ", QBranchRoot[i], QBranchCode[i], QBranchCount[i]);

		}
		*/
}

void GraphReader::searchNode(LISTINT * children, int nodeId, int g_i)
{
		int size = children->size();

		/*
		printf("\nnodeId=%d....\n", nodeId);
		printf("children:\n");
		for(int c_i=0; c_i<size; c_i++)
		{
			printf("%d ",children->at( c_i ) );
		}

		printf("\n");
		*/

		for(int c_i=0; c_i<size; c_i++)
		{
			int n_i = children->at( c_i );
			if(n_i < GN)
			{
				if(n_i == g_i)
				{
					printf("finding parent nodeId=%d\n", nodeId);
				}
			}
			else
			{
				n_i = n_i % GN;
				searchNode( nodeChildren[n_i], n_i, g_i );
			}
		}
}

void GraphReader::searchIndex( LISTINT * children, 	LISTINT & result )
{

		int size = children->size();
		for(int c_i=0; c_i<size; c_i++)
		{
			int n_i = children->at( c_i );
			if(n_i < GN)
			{
				int ged = branchDistance(vq, QBranchRoot, QBranchCode, QBranchCount, QBranchSize, vd[n_i], graphBranchRoot[n_i],
							graphBranchCode[n_i], graphBranchCount[n_i], graphBranchSize[n_i]);

				if(ged <= tau)
				{
					result.push_back(n_i);
				}

			}
			else
			{
				n_i = n_i % GN;
				int ged = directedDistanceINT_List(vq, QBranchRoot, QBranchCode, QBranchCount, QBranchSize, nodeSize[n_i], *nodeBranchRoot[n_i],
					      *nodeBranchCode[n_i], *nodeBranchCount[n_i], nodeBranchRoot[n_i]->size());
				if(ged > tau)
				{
					filter_branch++;
					continue;
				}

				searchIndex( nodeChildren[n_i], result );

			}
		
		}
	
}



void GraphReader::BuildTree_Partition()
{
	//divide top 11 graphs into two clusters
	double max = 0;
	int max_i = 0;
	int max_j = 1;

	for(int g_i=0; g_i<maxChildren; g_i++)
	{
		for(int g_j=g_i+1; g_j<(maxChildren+1); g_j++)
		{
			//printf("haha0.1\n");
			score_partiton[ g_i ][ g_j ] = jaccardPartition( & graphStatLabel[g_i], graphStatNeiVertex[g_i], graphStatNeiEdge[g_i], & graphContainEdgeLabel[g_i], 
															 & graphStatLabel[g_j], graphStatNeiVertex[g_j], graphStatNeiEdge[g_j], & graphContainEdgeLabel[g_j]);
			
			//printf("haha0\n");
			score_partiton[g_j][g_i] = score_partiton[g_i][g_j];
			if(max > score_partiton[ g_i ][ g_j ])
			{
				max = score_partiton[g_i][g_j];
				max_i = g_i;
				max_j = g_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT[1];
	LISTINT * node2 = new LISTINT[1];
	
	nodeParent_Partition.push_back(-1);//root
	nodeParent_Partition.push_back(-1);

	node1[0].push_back(max_i);
	node2[0].push_back(max_j);

	MAP_INT * statLabel_1 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_1 = new MAP_IB[1];
	//LIST_MAP_IB_P * statNeiVertex_1 = new LIST_MAP_IB_P[1];
	//LIST_MAP_IB_P * statNeiEdge_1 = new LIST_MAP_IB_P[1];
	//MAP_IB * containEdgeLabel = new MAP_IB[1];


	MAP_INT_Iter map_int_iter = graphStatLabel[max_i].begin();

	for( ; map_int_iter != graphStatLabel[max_i].end(); map_int_iter++)
	{
		statLabel_1[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
		
	}

	MAP_IB_Iter map_ib_iter = graphContainEdgeLabel[max_i].begin();
	for( ; map_ib_iter != graphContainEdgeLabel[max_i].end(); map_ib_iter++)
	{
		containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}

	
	MAP_INT * statLabel_2 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_2 = new MAP_IB[1];
	
	map_int_iter = graphStatLabel[max_j].begin();
	for( ; map_int_iter != graphStatLabel[max_j].end(); map_int_iter++)
	{
		statLabel_2[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
	}

	map_ib_iter = graphContainEdgeLabel[max_j].begin();
	for( ; map_ib_iter != graphContainEdgeLabel[max_j].end(); map_ib_iter++)
	{
		containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}

	

	for(int g_i=0; g_i<maxChildren+1; g_i++)
	{
		if(g_i != max_i && g_i != max_j)
		{
			if(score_partiton[max_i][g_i] >= score_partiton[max_j][g_i])
			{
				node1[0].push_back(g_i);

				map_int_iter = graphStatLabel[g_i].begin();
				for( ; map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
				{
					if(statLabel_1[0].find(map_int_iter->first) == statLabel_1[0].end())
					{
						statLabel_1[0].insert(pair<int,int>( map_int_iter->first, statLabel_1[0].size()) );
					}
					
				}

				map_ib_iter = graphContainEdgeLabel[g_i].begin();
				for( ; map_ib_iter != graphContainEdgeLabel[g_i].end(); map_ib_iter++)
				{
					if(containEdgeLabel_1[0].find( map_ib_iter->first ) == containEdgeLabel_1[0].end() )
					{
						containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

			}
			else
			{
				node2[0].push_back(g_i);

				map_int_iter = graphStatLabel[g_i].begin();
				for( ; map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
				{
					if(statLabel_2[0].find(map_int_iter->first) == statLabel_2[0].end())
					{
						statLabel_2[0].insert(pair<int,int>( map_int_iter->first, statLabel_2[0].size()) );
					}				
				}

				map_ib_iter = graphContainEdgeLabel[g_i].begin();
				for( ; map_ib_iter != graphContainEdgeLabel[g_i].end(); map_ib_iter++)
				{
					if(containEdgeLabel_2[0].find( map_ib_iter->first ) == containEdgeLabel_2[0].end() )
					{
						containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

				
			}
		}
	
	}

	MAP_IB * statNeiVertex_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiVertex_2 = new MAP_IB[ statLabel_2[0].size() ];

	MAP_IB * statNeiEdge_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiEdge_2 = new MAP_IB[ statLabel_2[0].size() ];


	for(int i=0; i<node1[0].size(); i++)
	{
		int g_i = node1[0].at(i);
		
		for( map_int_iter=graphStatLabel[g_i].begin(); map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_1[0][label];

			for(map_ib_iter=graphStatNeiVertex[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiVertex[g_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_1[pos_2].find(neiVL) == statNeiVertex_1[pos_2].end())
				{
					statNeiVertex_1[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=graphStatNeiEdge[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiEdge[g_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_1[pos_2].find(neiEL) == statNeiEdge_1[pos_2].end())
				{
					statNeiEdge_1[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}

		}
		
	}

	for(int i=0; i<node2[0].size(); i++)
	{
		int g_i = node2[0].at(i);
		
		for( map_int_iter=graphStatLabel[g_i].begin(); map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_2[0][label];
			
			for(map_ib_iter=graphStatNeiVertex[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiVertex[g_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_2[pos_2].find(neiVL) == statNeiVertex_2[pos_2].end())
				{
					statNeiVertex_2[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=graphStatNeiEdge[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiEdge[g_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_2[pos_2].find(neiEL) == statNeiEdge_2[pos_2].end())
				{
					statNeiEdge_2[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}
		}
		
	}

	nodeStatLabel.push_back(statLabel_1);
	nodeStatLabel.push_back(statLabel_2);

	nodeContainEdgeLabel.push_back(containEdgeLabel_1);
	nodeContainEdgeLabel.push_back(containEdgeLabel_2);

	nodeStatNeiVertex.push_back(statNeiVertex_1);
	nodeStatNeiVertex.push_back(statNeiVertex_2);

	nodeStatNeiEdge.push_back(statNeiEdge_1);
	nodeStatNeiEdge.push_back(statNeiEdge_2);
	

	rootNode_Partition.push_back(GN+0);
	rootNode_Partition.push_back(GN+1);

	//nodeChildren.push_back( & rootNode );
	nodeChildren_Partition.push_back( node1 );
	nodeChildren_Partition.push_back( node2 );


	depth = 0;
	depth2 = 0;
	for(int g_i=maxChildren+1; g_i<GN; g_i++)
	{
		//printf("g_i=%d\n", g_i);
		insert_Partition(g_i);//0 represents rootNode
	}

	printf("depth=%d depth2=%d\n", depth, depth2);
	

}

double GraphReader::jaccardPartition(MAP_INT * statLabel_a, MAP_IB *statNeiVertex_a, MAP_IB *statNeiEdge_a, MAP_IB * containEdgeLabel_a, MAP_INT *statLabel_b, MAP_IB *statNeiVertex_b, MAP_IB *statNeiEdge_b, MAP_IB * containEdgeLabel_b)
{
	double distance = 0;
	MAP_INT_Iter map_int_iter_a = statLabel_a[0].begin();
	MAP_INT_Iter map_int_iter_b;

	int common = 0;
	int total_a = 0;
	int total_b = 0;

	total_a = total_a + statLabel_a[0].size();

	for( ; map_int_iter_a!= statLabel_a[0].end(); map_int_iter_a++)
	{
		int label = map_int_iter_a->first;
		map_int_iter_b = statLabel_b[0].find(label);

		int cur_pos_a = map_int_iter_a->second;

		total_a = total_a + statNeiVertex_a[ cur_pos_a ].size();
		total_a = total_a + statNeiEdge_b[ cur_pos_a ].size();

		if( map_int_iter_b != statLabel_b[0].end())
		{
			common++;
			
			int cur_pos_b = map_int_iter_b->second;

			MAP_IB_Iter map_ib_iter_a = statNeiVertex_a[ cur_pos_a ].begin();
			MAP_IB_Iter map_ib_iter_b;
			for( ; map_ib_iter_a != statNeiVertex_a[ cur_pos_a ].end(); map_ib_iter_a++)
			{
				int neiVL = map_ib_iter_a->first;
				map_ib_iter_b = statNeiVertex_b[ cur_pos_b ].find(neiVL);
				if( map_ib_iter_b != statNeiVertex_b[ cur_pos_b ].end() )
				{
					common++;
				}
				
			}

			map_ib_iter_a = statNeiEdge_a[ cur_pos_a ].begin();
			for( ; map_ib_iter_a != statNeiEdge_a[ cur_pos_a ].end(); map_ib_iter_a++)
			{
				int neiEL = map_ib_iter_a->first;
				map_ib_iter_b = statNeiEdge_b[ cur_pos_b ].find(neiEL);
				if( map_ib_iter_b != statNeiEdge_b[ cur_pos_b ].end() )
				{
					common++;
				}
				
			}
		
		}
		
	}

	int size_b = statLabel_b[0].size();
	total_b = total_b + size_b;
		
	for(int i=0; i<size_b; i++)
	{
		total_b = total_b + statNeiVertex_b[i].size();
		total_b = total_b + statNeiEdge_b[i].size();
	}
	
	if(total_a >= total_b)
	{
		distance = 1.0 * common / total_a;
	}
	else
	{
		distance = 1.0 * common / total_b;
	}

	return distance;

}

void GraphReader::insert_Partition(int g_i)
{

	int min_node = -1;

	LISTINT * children = & rootNode_Partition;
	
	while(true)//while start---1
	{
		bool is_leaf = false;
		
		double distance = -1;

		//printf("g_i=%d here1\n", g_i);
		for(int c_i=0; c_i< children->size(); c_i++)
		{
			int nodeId = children->at( c_i );
			if(nodeId < GN)
			{
				is_leaf = true;
				break;
			}

			nodeId = nodeId % GN;

			//printf("haha1.1\n");
			double temp_sim = jaccardPartition( & graphStatLabel[g_i], graphStatNeiVertex[g_i], graphStatNeiEdge[g_i], & graphContainEdgeLabel[g_i], 
												  nodeStatLabel[nodeId], nodeStatNeiVertex[nodeId], nodeStatNeiEdge[nodeId], nodeContainEdgeLabel[nodeId]);
			//printf("haha1\n");
			if(distance < temp_sim)
			{
				distance = temp_sim;
				min_node = nodeId;
			}
		}

		//printf("g_i=%d here2\n", g_i);

		if(is_leaf)
		{

			//printf("g_i=%d here3\n", g_i);
			children->push_back(g_i);
			if(children->size() > maxChildren)//full
			{
				//printf("g_i=%d here4\n", g_i);
				depth++;
				//split(min_node);
				
				//searchNode_Partition(&rootNode_Partition, -1,6);//flag
				//printf(".........................\n");
				splitMul_Partition(min_node);
				//printf("g_i=%d here4 end\n", g_i);
				//searchNode_Partition(&rootNode_Partition, -1,6);//flag
				//printf(".........................\n");
			}
			break;
		}
		//printf("g_i=%d here5 min_node=%d\n", g_i, min_node);
		//printf("distance=%d\n", distance);

		children = nodeChildren_Partition[ min_node ];
		//printf("here 5.5\n");

		combine(g_i, min_node);

		//printf("g_i=%d here6\n", g_i);

	}// while end---1
}

void GraphReader::combine(int g_i, int nodeId)
{
	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter  map_ib_iter;

	//printf("g_i=%d here6.1\n", g_i);

	int size_nv = nodeStatLabel[nodeId][0].size();

	for(map_int_iter =graphStatLabel[g_i].begin(); map_int_iter!= graphStatLabel[g_i].end(); map_int_iter++ )
	{
		if(nodeStatLabel[nodeId]->find( map_int_iter->first ) == nodeStatLabel[nodeId]->end() )
		{
			nodeStatLabel[nodeId]->insert( pair<int,int>(map_int_iter->first, nodeStatLabel[nodeId]->size()) );
		}
	}

	for(map_ib_iter =graphContainEdgeLabel[g_i].begin(); map_ib_iter !=graphContainEdgeLabel[g_i].end(); map_ib_iter++ )
	{
		if(nodeContainEdgeLabel[nodeId]->find( map_ib_iter->first ) == nodeContainEdgeLabel[nodeId]->end() )
		{
			nodeContainEdgeLabel[nodeId]->insert( pair<int,bool>(map_ib_iter->first, false) );
		}
	
	}

	MAP_IB * statNeiVertex = new MAP_IB[ nodeStatLabel[nodeId]->size() ];
	MAP_IB * statNeiEdge = new MAP_IB[ nodeStatLabel[nodeId]->size() ];

	//printf("g_i=%d here6.2\n", g_i);

	for(int i=0; i<size_nv; i++)
	{
		//printf("i=%d size_nv=%d\n", i, size_nv);
		for(map_ib_iter=nodeStatNeiVertex[nodeId][i].begin(); map_ib_iter != nodeStatNeiVertex[nodeId][i].end(); map_ib_iter++)
		{
			statNeiVertex[i].insert( pair<int ,bool>(map_ib_iter->first,false)  );
		}
		//printf("ii=%d size_nv=%d\n", i, size_nv);
		for(map_ib_iter=nodeStatNeiEdge[nodeId][i].begin(); map_ib_iter != nodeStatNeiEdge[nodeId][i].end(); map_ib_iter++)
		{
			statNeiEdge[i].insert( pair<int ,bool>(map_ib_iter->first,false)  );
		}
		//printf("iii=%d size_nv=%d\n", i, size_nv);
		
	}
	//printf("g_i=%d here6.3\n", g_i);
		
	for( map_int_iter=graphStatLabel[g_i].begin(); map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
	{
		int label = map_int_iter->first;
		int pos_1 = map_int_iter->second;

		int pos_2 = nodeStatLabel[nodeId][0][label];
			
		for(map_ib_iter=graphStatNeiVertex[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiVertex[g_i][pos_1].end(); map_ib_iter++)
		{
			int neiVL = map_ib_iter->first;
			if(statNeiVertex[pos_2].find(neiVL) == statNeiVertex[pos_2].end())
			{
				statNeiVertex[pos_2].insert(pair<int,bool>(neiVL,false));
			}
		}
				
		for(map_ib_iter=graphStatNeiEdge[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiEdge[g_i][pos_1].end(); map_ib_iter++ )
		{
			int neiEL = map_ib_iter->first;
			if(statNeiEdge[pos_2].find(neiEL) == statNeiEdge[pos_2].end())
			{
				statNeiEdge[pos_2].insert(pair<int,bool>(neiEL,false));
			}

		}
		
	}
	//printf("g_i=%d here6.4\n", g_i);

	delete [] nodeStatNeiVertex[nodeId];
	nodeStatNeiVertex[nodeId] = statNeiVertex;
	
	delete [] nodeStatNeiEdge[nodeId];
	nodeStatNeiEdge[nodeId] = statNeiEdge;
}

void GraphReader::splitMul_Partition(int node)
{
	//int score[ maxChildren+1 ][ maxChildren+1];

	//divide top 11 graphs into two clusters
	int max = 1000;
	int max_i = 0;
	int max_j = 1;

	int pos_i = 0;
	int pos_j = 0;
	//printf("here4 partition 1 node=%d\n", node);
	for(int c_i=0; c_i < (nodeChildren_Partition[node]->size()-1); c_i++)
	{
		int g_i = nodeChildren_Partition[node]->at( c_i );

		for(int c_j=c_i+1; c_j < nodeChildren_Partition[node]->size(); c_j++)
		{
			int g_j = nodeChildren_Partition[node]->at( c_j );
			//printf("haha2.1\n");
			score_partiton[ c_i ][ c_j ] = jaccardPartition( & graphStatLabel[g_i], graphStatNeiVertex[g_i], graphStatNeiEdge[g_i], & graphContainEdgeLabel[g_i], 
															 & graphStatLabel[g_j], graphStatNeiVertex[g_j], graphStatNeiEdge[g_j], & graphContainEdgeLabel[g_j]);

			//printf("haha2\n");
			score_partiton[c_j][c_i] = score_partiton[c_i][c_j];
			//printf("score_partiton=%f ",score_partiton[c_i][c_j]);
			if(max > score_partiton[ c_i ][ c_j ])
			{
				max = score_partiton[c_i][c_j];
				max_i = g_i;
				max_j = g_j;

				pos_i = c_i;
				pos_j = c_j;
			}
		}
	}
	//printf("here4 partition 2 node=%d\n", node);
	LISTINT * node1 = new LISTINT [1];
	LISTINT * node2 = new LISTINT [1];;
	
	int parent = nodeParent_Partition[node];

	nodeParent_Partition.push_back(parent);//for node2

	node1[0].push_back(max_i);
	node2[0].push_back(max_j);

	MAP_INT * statLabel_1 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_1 = new MAP_IB[1];
	
	MAP_INT_Iter map_int_iter = graphStatLabel[max_i].begin();

	for( ; map_int_iter != graphStatLabel[max_i].end(); map_int_iter++)
	{
		statLabel_1[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
		
	}

	MAP_IB_Iter map_ib_iter = graphContainEdgeLabel[max_i].begin();
	for( ; map_ib_iter != graphContainEdgeLabel[max_i].end(); map_ib_iter++)
	{
		containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}

	//printf("here4 partition 3 node=%d\n", node);
	
	MAP_INT * statLabel_2 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_2 = new MAP_IB[1];
	
	map_int_iter = graphStatLabel[max_j].begin();
	for( ; map_int_iter != graphStatLabel[max_j].end(); map_int_iter++)
	{
		statLabel_2[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
	}

	map_ib_iter = graphContainEdgeLabel[max_j].begin();
	for( ; map_ib_iter != graphContainEdgeLabel[max_j].end(); map_ib_iter++)
	{
		containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}

	//printf("here4 partition 4 node=%d size=%d\n", node, nodeChildren_Partition[node]->size());

	for(int c_i=0; c_i<nodeChildren_Partition[node]->size(); c_i++)
	{
		int g_i = nodeChildren_Partition[node]->at( c_i );
		if(g_i != max_i && g_i != max_j)
		{
			if(score_partiton[pos_i][c_i] >= score_partiton[pos_j][c_i])
			{
				node1[0].push_back(g_i);

				map_int_iter = graphStatLabel[g_i].begin();
				for( ; map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
				{
					if(statLabel_1[0].find(map_int_iter->first) == statLabel_1[0].end())
					{
						statLabel_1[0].insert(pair<int,int>( map_int_iter->first, statLabel_1[0].size()) );
					}
					
				}

				map_ib_iter = graphContainEdgeLabel[g_i].begin();
				for( ; map_ib_iter != graphContainEdgeLabel[g_i].end(); map_ib_iter++)
				{
					if(containEdgeLabel_1[0].find( map_ib_iter->first ) == containEdgeLabel_1[0].end() )
					{
						containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

			}
			else
			{
				node2[0].push_back(g_i);

				map_int_iter = graphStatLabel[g_i].begin();
				for( ; map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
				{
					if(statLabel_2[0].find(map_int_iter->first) == statLabel_2[0].end())
					{
						statLabel_2[0].insert(pair<int,int>( map_int_iter->first, statLabel_2[0].size()) );
					}				
				}

				map_ib_iter = graphContainEdgeLabel[g_i].begin();
				for( ; map_ib_iter != graphContainEdgeLabel[g_i].end(); map_ib_iter++)
				{
					if(containEdgeLabel_2[0].find( map_ib_iter->first ) == containEdgeLabel_2[0].end() )
					{
						containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

				
			}
		}
	
	}
	//printf("here4 partition 4.5 node=%d\n", node);


	MAP_IB * statNeiVertex_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiVertex_2 = new MAP_IB[ statLabel_2[0].size() ];

	MAP_IB * statNeiEdge_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiEdge_2 = new MAP_IB[ statLabel_2[0].size() ];

	//printf("here4 partition 5 node=%d\n", node);

	for(int i=0; i<node1[0].size(); i++)
	{
		int g_i = node1[0].at(i);
		
		for( map_int_iter=graphStatLabel[g_i].begin(); map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_1[0][label];

			for(map_ib_iter=graphStatNeiVertex[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiVertex[g_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_1[pos_2].find(neiVL) == statNeiVertex_1[pos_2].end())
				{
					statNeiVertex_1[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=graphStatNeiEdge[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiEdge[g_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_1[pos_2].find(neiEL) == statNeiEdge_1[pos_2].end())
				{
					statNeiEdge_1[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}

		}
		
	}

	for(int i=0; i<node2[0].size(); i++)
	{
		int g_i = node2[0].at(i);
		
		for( map_int_iter=graphStatLabel[g_i].begin(); map_int_iter != graphStatLabel[g_i].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_2[0][label];
			
			for(map_ib_iter=graphStatNeiVertex[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiVertex[g_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_2[pos_2].find(neiVL) == statNeiVertex_2[pos_2].end())
				{
					statNeiVertex_2[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=graphStatNeiEdge[g_i][pos_1].begin(); map_ib_iter!=graphStatNeiEdge[g_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_2[pos_2].find(neiEL) == statNeiEdge_2[pos_2].end())
				{
					statNeiEdge_2[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}
		}
		
	}
	//printf("here4 partition 6 node=%d\n", node);
	//nodeSize.push_back(0);

	delete [] nodeChildren_Partition[node];
	nodeChildren_Partition[node] = node1 ;
	
	if(parent == -1)
	{
		rootNode_Partition.push_back( GN + nodeChildren_Partition.size() );
	}
	else
	{
		nodeChildren_Partition[parent]->push_back( GN + nodeChildren_Partition.size() );
	}
	

	nodeChildren_Partition.push_back( node2 );


	delete [] nodeStatLabel[node];
	delete [] nodeContainEdgeLabel[node];
	delete [] nodeStatNeiVertex[node];
	delete [] nodeStatNeiEdge[node];

	nodeStatLabel[node] = statLabel_1;
	nodeContainEdgeLabel[node] = containEdgeLabel_1;
	nodeStatNeiVertex[node] = statNeiVertex_1;
	nodeStatNeiEdge[node] = statNeiEdge_1;

	nodeStatLabel.push_back(statLabel_2);
	nodeContainEdgeLabel.push_back(containEdgeLabel_2);
	nodeStatNeiVertex.push_back(statNeiVertex_2);
	nodeStatNeiEdge.push_back(statNeiEdge_2);

	//printf("here4 partition 7 node=%d\n", node);

	splitMul_sub_Partition(parent);

	//nodeChildren.push_back( & rootNode );

}

void GraphReader::splitMul_sub_Partition(int node)
{
	LISTINT * children;
	if(node == -1)
	{
		children = & rootNode_Partition;
	}
	else
	{
		children = nodeChildren_Partition[node];
	}

	if(children->size() <= maxChildren)
	{
		return;
	}

	int max = 1000;
	int max_i = 0;
	int max_j = 1;

	int pos_i = 0;
	int pos_j = 0;

	for(int c_i=0; c_i < (children->size()-1); c_i++)
	{
		int n_i = children->at( c_i ) % GN;

		for(int c_j=c_i+1; c_j < children->size(); c_j++)
		{
			int n_j = children->at( c_j ) % GN;
			//printf("haha3.1\n");
			score_partiton[ c_i ][ c_j ] = jaccardPartition( nodeStatLabel[n_i], nodeStatNeiVertex[n_i], nodeStatNeiEdge[n_i], nodeContainEdgeLabel[n_i], 
															 nodeStatLabel[n_j], nodeStatNeiVertex[n_j], nodeStatNeiEdge[n_j], nodeContainEdgeLabel[n_j]);
			
			//printf("haha3\n");
			score_partiton[c_j][c_i] = score_partiton[c_i][c_j];
			if(max > score_partiton[ c_i ][ c_j ])
			{
				max = score_partiton[c_i][c_j];
				max_i = n_i;
				max_j = n_j;

				pos_i = c_i;
				pos_j = c_j;
			}
		}
	}

	LISTINT * node1 = new LISTINT [1];
	LISTINT * node2 = new LISTINT [1];;
	
	node1->push_back(max_i+GN);
	node2->push_back(max_j+GN);


	MAP_INT * statLabel_1 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_1 = new MAP_IB[1];
	
	MAP_INT_Iter map_int_iter = nodeStatLabel[max_i][0].begin();

	for( ; map_int_iter != nodeStatLabel[max_i]->end(); map_int_iter++)
	{
		statLabel_1[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
		
	}

	MAP_IB_Iter map_ib_iter = nodeContainEdgeLabel[max_i]->begin();
	for( ; map_ib_iter != nodeContainEdgeLabel[max_i]->end(); map_ib_iter++)
	{
		containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}

	
	MAP_INT * statLabel_2 = new MAP_INT[1];
	MAP_IB  * containEdgeLabel_2 = new MAP_IB[1];
	
	map_int_iter = nodeStatLabel[max_j][0].begin();
	for( ; map_int_iter != nodeStatLabel[max_j][0].end(); map_int_iter++)
	{
		statLabel_2[0].insert(pair<int,int>( map_int_iter->first, map_int_iter->second) );
	}

	map_ib_iter = nodeContainEdgeLabel[max_j][0].begin();
	for( ; map_ib_iter != nodeContainEdgeLabel[max_j][0].end(); map_ib_iter++)
	{
		containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false) );
	}



	for(int c_i=0; c_i<children->size(); c_i++)
	{
		int n_i = children->at( c_i ) % GN;

		if(n_i != max_i && n_i != max_j)
		{
			if(score_partiton[pos_i][c_i] >= score_partiton[pos_j][c_i])
			{
				node1[0].push_back(n_i+GN);

				map_int_iter = nodeStatLabel[n_i][0].begin();
				for( ; map_int_iter != nodeStatLabel[n_i][0].end(); map_int_iter++)
				{
					if(statLabel_1[0].find(map_int_iter->first) == statLabel_1[0].end())
					{
						statLabel_1[0].insert(pair<int,int>( map_int_iter->first, statLabel_1[0].size()) );
					}
					
				}

				map_ib_iter = nodeContainEdgeLabel[n_i][0].begin();
				for( ; map_ib_iter != nodeContainEdgeLabel[n_i][0].end(); map_ib_iter++)
				{
					if(containEdgeLabel_1[0].find( map_ib_iter->first ) == containEdgeLabel_1[0].end() )
					{
						containEdgeLabel_1[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

			}
			else
			{
				node2[0].push_back(n_i+GN);

				map_int_iter = nodeStatLabel[n_i][0].begin();
				for( ; map_int_iter != nodeStatLabel[n_i][0].end(); map_int_iter++)
				{
					if(statLabel_2[0].find(map_int_iter->first) == statLabel_2[0].end())
					{
						statLabel_2[0].insert(pair<int,int>( map_int_iter->first, statLabel_2[0].size()) );
					}				
				}

				map_ib_iter = nodeContainEdgeLabel[n_i][0].begin();
				for( ; map_ib_iter != nodeContainEdgeLabel[n_i][0].end(); map_ib_iter++)
				{
					if(containEdgeLabel_2[0].find( map_ib_iter->first ) == containEdgeLabel_2[0].end() )
					{
						containEdgeLabel_2[0].insert(pair<int,bool>( map_ib_iter->first, false ));
					}
				}

				
			}
		}
	
	}

	MAP_IB * statNeiVertex_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiVertex_2 = new MAP_IB[ statLabel_2[0].size() ];

	MAP_IB * statNeiEdge_1 = new MAP_IB[ statLabel_1[0].size() ];
	MAP_IB * statNeiEdge_2 = new MAP_IB[ statLabel_2[0].size() ];


	for(int i=0; i<node1[0].size(); i++)
	{
		int n_i = node1[0].at(i) % GN;
		
		for( map_int_iter=nodeStatLabel[n_i][0].begin(); map_int_iter != nodeStatLabel[n_i][0].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_1[0][label];

			for(map_ib_iter=nodeStatNeiVertex[n_i][pos_1].begin(); map_ib_iter!=nodeStatNeiVertex[n_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_1[pos_2].find(neiVL) == statNeiVertex_1[pos_2].end())
				{
					statNeiVertex_1[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=nodeStatNeiEdge[n_i][pos_1].begin(); map_ib_iter!=nodeStatNeiEdge[n_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_1[pos_2].find(neiEL) == statNeiEdge_1[pos_2].end())
				{
					statNeiEdge_1[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}

		}
		
	}

	for(int i=0; i<node2[0].size(); i++)
	{
		int n_i = node2[0].at(i) % GN;
		
		for( map_int_iter=nodeStatLabel[n_i][0].begin(); map_int_iter != nodeStatLabel[n_i][0].end(); map_int_iter++)
		{
			int label = map_int_iter->first;
			int pos_1 = map_int_iter->second;

			int pos_2 = statLabel_2[0][label];
			
			for(map_ib_iter=nodeStatNeiVertex[n_i][pos_1].begin(); map_ib_iter!=nodeStatNeiVertex[n_i][pos_1].end(); map_ib_iter++)
			{
				int neiVL = map_ib_iter->first;
				if(statNeiVertex_2[pos_2].find(neiVL) == statNeiVertex_2[pos_2].end())
				{
					statNeiVertex_2[pos_2].insert(pair<int,bool>(neiVL,false));
				}
			}
				
			for(map_ib_iter=nodeStatNeiEdge[n_i][pos_1].begin(); map_ib_iter!=nodeStatNeiEdge[n_i][pos_1].end(); map_ib_iter++ )
			{
				int neiEL = map_ib_iter->first;
				if(statNeiEdge_2[pos_2].find(neiEL) == statNeiEdge_2[pos_2].end())
				{
					statNeiEdge_2[pos_2].insert(pair<int,bool>(neiEL,false));
				}

			}
		}
		
	}


	//nodeSize.push_back(0);
	if(node != -1)
	{
		int parent = nodeParent_Partition[node];

		nodeParent_Partition.push_back(parent);//for node2

		delete [] nodeChildren_Partition[node];
		nodeChildren_Partition[node] = node1;

		if(parent != -1)
		{
			nodeChildren_Partition[parent]->push_back( GN + nodeChildren_Partition.size() );//for node2
		}
		else
		{
			rootNode_Partition.push_back( GN + nodeChildren_Partition.size() );//for node2
		}
		
		nodeChildren_Partition.push_back( node2 );

		delete [] nodeStatLabel[node];
		delete [] nodeContainEdgeLabel[node];
		delete [] nodeStatNeiVertex[node];
		delete [] nodeStatNeiEdge[node];

		nodeStatLabel[node] = statLabel_1;
		nodeContainEdgeLabel[node] = containEdgeLabel_1;
		nodeStatNeiVertex[node] = statNeiVertex_1;
		nodeStatNeiEdge[node] = statNeiEdge_1;

		nodeStatLabel.push_back(statLabel_2);
		nodeContainEdgeLabel.push_back(containEdgeLabel_2);
		nodeStatNeiVertex.push_back(statNeiVertex_2);
		nodeStatNeiEdge.push_back(statNeiEdge_2);

		splitMul_sub_Partition(parent);

	}
	else
	{
		depth2++;
		rootNode_Partition.clear();

		rootNode_Partition.push_back( GN + nodeChildren_Partition.size() );//for node1
		rootNode_Partition.push_back( GN + nodeChildren_Partition.size() +1);//for node2

		nodeParent_Partition.push_back(-1);//for node1
		nodeParent_Partition.push_back(-1);//for node2
	

		nodeChildren_Partition.push_back( node1 );
		nodeChildren_Partition.push_back( node2 );

		nodeStatLabel.push_back(statLabel_1);
		nodeContainEdgeLabel.push_back(containEdgeLabel_1);
		nodeStatNeiVertex.push_back(statNeiVertex_1);
		nodeStatNeiEdge.push_back(statNeiEdge_1);

		nodeStatLabel.push_back(statLabel_2);
		nodeContainEdgeLabel.push_back(containEdgeLabel_2);
		nodeStatNeiVertex.push_back(statNeiVertex_2);
		nodeStatNeiEdge.push_back(statNeiEdge_2);

	}

}

void GraphReader::searchNode_Partition(LISTINT * children, int nodeId, int g_i)
{
		int size = children->size();

		/*
		printf("\nnodeId=%d....\n", nodeId);
		printf("children:\n");
		for(int c_i=0; c_i<size; c_i++)
		{
			printf("%d ",children->at( c_i ) );
		}

		printf("\n");
		*/

		for(int c_i=0; c_i<size; c_i++)
		{
			int n_i = children->at( c_i );
			if(n_i < GN)
			{
				if(n_i == g_i)
				{
					printf("finding parent nodeId=%d\n", nodeId);
				}
			}
			else
			{
				n_i = n_i % GN;
				searchNode_Partition( nodeChildren_Partition[n_i], n_i, g_i );
			}
		}
}

void GraphReader::searchIndex_Partition( LISTINT * children, 	LISTINT & result )
{

		int size = children->size();
		for(int c_i=0; c_i<size; c_i++)
		{
			int n_i = children->at( c_i );
			if(n_i < GN)
			{
				
				if(! partition(n_i) )
				{
					result.push_back(n_i);
				}

			}
			else
			{
				n_i = n_i % GN;
				int ged = distanceFisrtsThreeSteps( n_i );
				if(ged > tau)
				{
					filter_partition++;
					continue;
				}

				searchIndex_Partition( nodeChildren_Partition[n_i], result );

			}
		
		}
}


int GraphReader::distanceFisrtsThreeSteps(int node)//for patition
{
	int ged = 0;//graph edit distance

	
	MAP_IB * GStatNeiVertex = nodeStatNeiVertex[node];
	MAP_IB * GStatNeiEdge = nodeStatNeiEdge[node];

	QVertexVisited = new bool[vq];
	QEdgeVisited = new bool *[vq];

	memset(QVertexUsed, 0, sizeof(bool) * vq);//initial

	memset(QVertexVisited, 0, sizeof(bool) * vq);

	for(int v_i=0; v_i<vq; v_i++)
	{
		memset(QEdgeUsed[v_i], 0, sizeof(bool)*vq);//initial

		QEdgeVisited[v_i] = new bool[vq];
		memset(QEdgeVisited[v_i], 0, sizeof(bool)*vq);//initial
	}


	int label_q;
	MAP_INT_Iter map_int_iter;
	MAP_IB_Iter map_ib_iter;

	int count_used =0;

		for(int v_i_q=0; v_i_q<vq; v_i_q++)//for-1
		{
			int pos_i_q = QVertexId[v_i_q];

			if(!QVertexUsed[v_i_q])//have not been used ---- if-1.1
			{
				label_q = QVertexLabel[pos_i_q];
				map_int_iter = nodeStatLabel[ node ][0].find(label_q);
				if(map_int_iter == nodeStatLabel[ node ][0].end())//if-1.1.1
				{
					ged++;
					QVertexUsed[v_i_q] = true;
					count_used++;
					continue;
				}
				else// if-1.1.1 middle
				{
					int v_label_pos_d = map_int_iter->second;

					int nei_size = QVertexNeiSize[pos_i_q];
					int nei_i = 0;
					for( nei_i=0; nei_i<nei_size; nei_i++)//for-1.1
					{
						int nei_id = QVertexNeighbor[pos_i_q][nei_i];

						if(!QEdgeUsed[v_i_q][nei_id])//if-1.1.1.1
						{
							int edge_label = QEdgeLabel[pos_i_q][nei_i];

							map_ib_iter = nodeContainEdgeLabel[node][0].find(edge_label);
							if(map_ib_iter == nodeContainEdgeLabel[node][0].end())
							{
								ged++;
								QEdgeUsed[v_i_q][nei_id] = true;
								QEdgeUsed[nei_id][v_i_q] = true;
								count_used++;
								continue;
							}

							map_ib_iter = GStatNeiEdge[v_label_pos_d].find(edge_label);
							if(map_ib_iter == GStatNeiEdge[v_label_pos_d].end())
							{
								QEdgeUsed[v_i_q][nei_id] = true;
								QEdgeUsed[nei_id][v_i_q] = true;

								QVertexUsed[v_i_q] = true;
								ged++;
								count_used = count_used + 2;
								break;
							}
							
						}//if-1.1.1.1 end

					}//for-1.1 end

					if(nei_i < nei_size)
					{
						continue;
					}

					for( nei_i=0; nei_i<nei_size; nei_i++)//for-1.2
					{
						int nei_id = QVertexNeighbor[pos_i_q][nei_i];

						int nei_pos = QVertexId[nei_id];

						if(!QVertexUsed[nei_id])
						{
							int nei_label = QVertexLabel[nei_pos];

							map_int_iter = nodeStatLabel[ node ][0].find(nei_label);

							if(map_int_iter == nodeStatLabel[ node ][0].end())
							{
								QVertexUsed[nei_id] = true;
								ged++;
								count_used++;
								continue;
							}

							map_ib_iter = GStatNeiVertex[v_label_pos_d].find(nei_label);

							if(map_ib_iter == GStatNeiVertex[v_label_pos_d].end())
							{
								QVertexUsed[nei_id] = true;
								QVertexUsed[v_i_q] = true;
								count_used = count_used + 3;
								ged++;
								break;
							}
						}

					}//for-1.2 end

					if(nei_i < nei_size)
					{
						continue;
					}

				}//if-1.1.1 end

			}//if-1.1 end 

		}//for-1 end
		return ged;
}