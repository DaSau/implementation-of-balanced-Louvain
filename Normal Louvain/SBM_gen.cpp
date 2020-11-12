#include <iostream>
#include <random>
#include <vector>
#include <unordered_set>
#include <algorithm>    // std::shuffle

#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>

using namespace std;
/*
ATTENTION !!!!
Dernière ligne du graphe -1 -1 pour testGraph.c
Doit être supprimé pour expe.c
l 223
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/


double BinarySearch(vector < long double > &a, int M, int m,
		    long double v){
//    cout << M << " " << m << " " << v << " " << a[M] << flush << endl;
    //printf("biinary ! %d %d %Lf, %Lf\n", M, m, v, a[(m+M)/2]);
    if(M-m <= 1)
	    return (a[m] < v)?M:m;
    int middle = (m+M)/2;
    if(a[middle] < v)
        return BinarySearch(a, M, middle, v);
    return     BinarySearch(a, middle, m, v);
    
    //code balayge pour debugger
    /*
    int r = 0;
    //printf("%Lf : ", v);
    while(r < M-1 && a[r] < v)
    {
        printf("%Lf, ", a[r]);
        r++;
    }
    printf("\n");
    return r;
    */
}




void SampleWithoutReplacement(int N, int k, int elt, vector <int> &list,
			      vector < int > &a, default_random_engine& gen){
    // Sample k elements different from elt from a list without replacement
    // k should be <= N
    
    int end = N-1;
    for (int i = 0; i < k ; i++){
	    int v =  uniform_int_distribution<>(0, end)(gen);
        if(list[v] == elt)
        {
            //put list[v] at the end, move the end but not i
            a[i] = list[v];
            list[v] = list[end];
            list[end] = a[i];
            i--;
            end--;
        }
        else
        {
            a[i] = list[v];
            list[v] = list[end];
            list[end] = a[i];
            end--;
        }
    }
    return;
}

void GetCDF(vector < long double > &a, int N,
	    string filename = "tmp_cdf_p"){
    a.resize(N);
    std::ifstream infile(filename);
    long double t;
    int c = 0;
    while (infile >> t && c < N){
	a[c++] = t;
    }
    while(c < N){
	a[c++] = a[c-1];
    }
}

void GenSBM(int n1, int n2, int size, // long double p, long double q,
	    vector < vector < int > > &adj,
	    int seed = 1337,
	    string cdf_p_n1 = "tmp_cdf_p_n1",
	    string cdf_p_n2 = "tmp_cdf_p_n2",
	    string cdf_q = "tmp_cdf_q"){
    //printf("%d %d %d %d %s %s %s\n", n1, n2, size, seed, cdf_p_n1.c_str(), cdf_p_n2.c_str(), cdf_q.c_str());
    int N = n1+n2;
    
    vector < long double > CDF_p_n1;
    vector < long double > CDF_p_n2;
    vector < long double > CDF_q;
    vector < int > degrees, community1, community2;
  
    GetCDF(CDF_p_n1, n1, cdf_p_n1);
    GetCDF(CDF_p_n2, n2, cdf_p_n2);
    GetCDF(CDF_q,    n2, cdf_q);
	
    //for(int i = 0; i < CDF_p_n1.size(); i++)
    //    printf("%f ", CDF_p_n1[i]);
    //printf("\n");
        
    adj.resize(N);
    for (int i = 0; i < N; ++i)
	    adj[i].resize(size, -1);
    
    community1.resize(n1);
    community2.resize(n2);
    //initialize community vector to sample from
    for (int i = 0; i < n1; i++)
        community1[i] = i;
    for(int i = 0; i < n2; i++)
        community2[i] = i+n1;

    degrees.resize(N, 0);

    std::default_random_engine rand_engine(seed);

    
    for (int i = 0; i < n1; i++){
        uniform_real_distribution<long double> dis(0.0, 1.0);
        long double rand_n1 = dis(rand_engine);
        long double rand_n2 = dis(rand_engine);
        
    //	cout << rand_n1 << flush << endl;
        int nb_neighbors_n1 = BinarySearch(CDF_p_n1, n1,
					   0, rand_n1);
        int nb_neighbors_n2 = BinarySearch(CDF_q, n2,
					   0, rand_n2);
        
        
        //printf("%d, %d %d\n", i, nb_neighbors_n1, nb_neighbors_n2);

        vector < int >  neighbors_n1;
        neighbors_n1.resize(size);
        //printf("\t %d %d  neig\n", nb_neighbors_n1, nb_neighbors_n2);
        SampleWithoutReplacement(n1, nb_neighbors_n1, i, community1,
                     neighbors_n1, rand_engine);
        //printf("ok\n");
        //printf("%d %d\n", nb_neighbors_n1, neighbors_n1.size());
    	//cout << nb_neighbors_n1 << neighbors_n1.size() << flush << endl;
        vector < int >  neighbors_n2;
        neighbors_n2.resize(size);
        SampleWithoutReplacement(n2, nb_neighbors_n2, i, community2,
                     neighbors_n2, rand_engine);
        
        for (int j = 0; j < nb_neighbors_n1; j++){
            //printf("\t%d\n", neighbors_n1[j]);
            int neighbor = neighbors_n1[j];
            adj[i][degrees[i]+j] = neighbor;
            adj[neighbor][degrees[neighbor]] = i;
            degrees[neighbor] +=1;
            if ( degrees[neighbor] > size){
                cout << "Failed too many neighbors" << endl;
                return ;
            }
	    }
        //printf("ok %d\n", nb_neighbors_n2);
        for (int j = 0; j < nb_neighbors_n2; j++){
            //printf("\t%d\n",neighbors_n2[j]);
            int neighbor = neighbors_n2[j];
            adj[i][degrees[i]+j+nb_neighbors_n1] = neighbor;
            //printf("\tok %d %d\n", neighbor, degrees[neighbor]);
            adj[neighbor][degrees[neighbor]] = i;
            degrees[neighbor] +=1;
            if ( degrees[neighbor] > size){
                cout << "Failed too many neighbors" << endl;
                return ;
            }
	    }
        //printf("ok\n");
        degrees[i] += nb_neighbors_n1+nb_neighbors_n2;
    }

    for (int i = 0; i < n2; i++){
        uniform_real_distribution<long double> dis(0.0, 1.0);
        long double rand_n2 = dis(rand_engine);
        int nb_neighbors_n2 = BinarySearch(CDF_p_n2, n2,
					   0, rand_n2);
        //printf("%d, %d\n", i+n1, nb_neighbors_n2);
    	int v = i+n1;
        vector < int > neighbors_n2;
        neighbors_n2.resize(size);
	    SampleWithoutReplacement(n2, nb_neighbors_n2, v, community2,
				 neighbors_n2, rand_engine);
        for (int j = 0; j < nb_neighbors_n2; j++){
            int neighbor = neighbors_n2[j];
            //printf("\t%d\n", neighbor);
            adj[v][degrees[v]+j] = neighbor;
            adj[neighbor][degrees[neighbor]] = v;
            degrees[neighbor] +=1;
            if ( degrees[neighbor] > size){
                cout << "Failed too many neighbors" << endl;
                return ;
            }
	    }
        degrees[v] += nb_neighbors_n2;
    }
    /*int degTot = 0;
    for(int i = 0; i < N; i++)
        degTot += degrees[i];
    printf("deg moyen gen (avec biaretes) : %f\n", (float)degTot/N);
    */
}



void PrintGraph(vector < vector < int > > &adj){
    for (int i = 0; i < adj.size(); i++){
	cout << "=============================" << endl;
	cout << i << " : ";
	for (int j = 0; j < adj[i].size(); j++){
	    if (adj[i][j] == -1) break;
	    printf("%d ", adj[i][j]);
	}
	cout << endl << endl;
    }
}

void SaveGraph(vector < vector<int> > &adj, ofstream& output)
{
    auto hash = [](const pair<int, int>& p){ return p.first * 81667 + p.second; };
    unordered_set<pair<int, int>, decltype(hash)> edges(8, hash);
 
    for (int i = 0; i < adj.size(); i++)
	    for (int j = 0; j < adj[i].size(); j++){
	        if (adj[i][j] == -1) break;
            if(edges.find(make_pair(adj[i][j], i)) == edges.end() && 
               edges.find(make_pair(i, adj[i][j])) == edges.end())
            {
                output << i << " " << adj[i][j] << endl;
                edges.emplace(i, adj[i][j]);
            }
        }
    //output << "-1 -1" << endl;
}



int main (int argc, char* argv[])
{
//parameters: n1 n2 p q "file for p CDF n1" "file for p CDF n2" "file for q CDF"
    int n1 = atoi(argv[1]), n2 = atoi(argv[2]);
    float p = atof(argv[3]), q = atof(argv[4]);

    vector < vector < int > > adj;
    if (argc > 5)
	GenSBM(n1, n2, int((n1+n2)*p*10),
	       adj, time(NULL), argv[5], argv[6], argv[7]);
    else
	    GenSBM(n1, n2, int((n1+n2)*p*10), adj);
	
    ofstream output;
    output.open("graph" + to_string(n1) + to_string(n2)+to_string(int(p*1000))+to_string(int(q*1000)));
    //PrintGraph(adj);
    SaveGraph(adj, output);
    //cout << "Generation Done." << endl << flush;
    return 0;
}
