#include <iostream>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <algorithm>

using namespace std;

int **adj;  // Matriz de Adjacentes
int **cost; // Matriz de Custos
int minPath = INT_MAX;      // Custo do Melhor Caminho Atual
int BestPath[50];      // Melhor Caminho
int V, E;
#define V_MAX 20
int dp[1<<20][20];

int visited (int city, int hops, const int *path)
{
    for (int i = 0; i < hops; i++)
        if (path[i] == city) return 1;
    return 0;
}

void visit(const int city, int hops, int lenth, int *path)
{
    int visitCity = 0;
			
	if(hops == V + 1 && city == path[0]) visitCity = 1;
	else if(!visited(city, hops, path)) // se não foi visitada
	{
		visitCity = 1;
		path[hops-1] = city;
	}

    if(visitCity)
    {
		if (lenth >= minPath)
        {
            return;
        }
        else if(hops == V + 1)
        {
			#pragma omg critical
			if (lenth < minPath)
            {
                minPath = lenth;
            }
			
            for(int i = 0; i < V; i++)
            {
				#pragma omg critical
                BestPath[i] = path[i];
            }

			return;
        }
		
	    for (int i = 0; i < V; i++)
        {
			if(adj[city][i])
			{
				visit(i, hops+1, lenth+cost[city][i], path);
			}
        }
    }
}

void visit1(const int city, int hops, int lenth)
{
	int path[V][V];
	int count[V];

	fill(&path[0][0],&path[V][0],-1);
	
	for(int i = 0; i < V; i++)
	{
		count[i] = 0;
		for(int j = 0; j < V; j++)
		{
			if(adj[i][j] == 1) count[i]++;
		}
	}
	
	int min = std:: distance(count, std::max_element(count, &count[V]));
	
	#pragma omp parallel for
	for (int i = 1; i < V; i++)
    {
		path[i][0] = min;
		if(adj[city][i])
		{
			visit(i, hops+1, lenth+cost[city][i], path[i]);
		}
    }
    
}

int fast(int vis,int pos)
{
    int r = 1 << 30;
    if(dp[vis][pos] != -1) return dp[vis][pos];
    if(pos == 0 && vis == (1<<V) - 1)
	{
        return 0;
    }
	
    for(int i = 0; i < V; i++)
	{
		if(cost[pos][i] != -1 && !(vis & (1<<i)) )
		{
			r = min(r, fast( vis | (1<<i) ,i) + cost[pos][i]);
		}
    }
 
    return dp[vis][pos] = r;
}

int main()
{
    int a, b, d;
    double begin, end;
    double time_spent;
	int ans;
	
	cin >> V >> E;

	if(V >= V_MAX)
	{
		adj = new int*[V];
		cost = new int*[V];
		for(int i = 0; i < V; i++)
		{
			adj[i] = new int[V];
			cost[i] = new int[V];
		}
		
		for(int i = 0; i < E; i++)
		{
			cin >> a >> b >> d;
			cost[a][b] = d;
			cost[b][a] = d;
			adj[a][b] = 1;
		}
		
		begin = omp_get_wtime();
		
		visit1(0, 1, 0);
		
		ans = minPath;
	}
	else if(V >= 0)
	{
		adj = new int*[V];
		cost = new int*[V];
		for(int i = 0; i < V; i++)
		{
			adj[i] = new int[V];
			cost[i] = new int[V];
		}
		
		fill(&cost[0][0],&cost[V-1][0],-1);
		
		for(int i = 0; i < E; i++)
		{
			cin >> a >> b >> d;
			cost[a][b] = d;
			adj[a][b] = 1;
			cost[b][a] = d;
		}
		
		begin = omp_get_wtime();
		
		fill(&dp[0][0],&dp[1<<V][0],-1);
		
		ans = fast(0,0);
	}
    
    
    end = omp_get_wtime();

    printf("%d\n", ans);
    
    printf ("Elapsed time is: %f s\n\n", end - begin);
	
    return 0;
}


