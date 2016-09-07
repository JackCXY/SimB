
#include "GraphReader.h"
#include <time.h>


int main (int argc, char* argv[])
{
	/*
	if(argc!=3)
	{
		printf("Error input!\n");
		return 0;
	}
	char * fileName_G=argv[1];
	char * fileName_Q=argv[2];
	*/
	
	char* fileName_G = "../dataset/AIDS/AIDO99SD2_Convert.txt_clean";	

	
	GraphReader gr;

	gr.tau = 3;
	
	printf("reading graph...\n");

	gr.DGraphReaderIndex(fileName_G);

	clock_t t1= clock();
	gr.BuildInex(fileName_G);

	printf("over1.\n");
	
	gr.BuildTree();

	printf("begin\n");
	
	
	clock_t t2= clock();
	
	
	LISTINT result;
	
	int candidates = 0;
	int candidates_p = 0;
	int real = 0;
	for(int i=1; i<=20; i++)
	{
		
		stringstream ss;
		ss<<i;
		string str_t = ss.str();

		
		string str_name = "../dataset/AIDS/query/q" + str_t+".txt";
		//string str_name = "../dataset/protein/query/q" + str_t+".txt";
		//string str_name = "../dataset/AIDS/query40k/q" + str_t+".txt";
		//string str_name = "../dataset/ER/query10k/degs" + str_t+".txt";
		//string str_name = "../dataset/SF/query10k/q" + str_t+".txt";


		printf("%d %s\n", i, str_name.data());
		
		char* p =const_cast<char*>( str_name.data());

		gr.QIndex(p);

		gr.filter_branch = 0;
		//printf("hah\n");
	
		//gr.searchIndex(& gr.rootNode, result);
		
		
		for(int g_i=0; g_i<gr.GN; g_i++)
		{	
			
			if(! gr.partition(g_i) )
			{
					candidates_p++;
					
			}
			else
			{
					continue;
			}
			
			int edt = gr.graphEditDistance(g_i);
			
			if(edt == -1)
			{
					continue;
			}
					
			real++;
			printf("%d = %d\n",g_i, edt );
		 
		}
			
		
	}
	
	
	clock_t t3= clock();
	
	printf("Index time was:  %f\n", (double)(t2 -t1)/CLOCKS_PER_SEC); 
	printf("Search time was:  %f\n", (double)(t3 -t2)/(CLOCKS_PER_SEC*20)); 
	printf("candidates size=%d\n", result.size()/20);
	printf("candidates size=%d %d real=%d\n", candidates/20, candidates_p/20, real/20);


	
	getchar();
	

	return 1;
}//main end