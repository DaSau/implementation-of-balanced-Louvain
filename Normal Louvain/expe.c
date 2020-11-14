//#include <igraph.h>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <string>
using namespace std;

const int NB_TRIALS = 15;
//const float ERROR_BINARY = 0.00005;
time_t totalGenTime = 0, totalLouvainTime = 0;


void genGraph(int n1, int n2, float p, float q, int cdfKnown)
{
     time_t timeGenStart = time(NULL);
     string pstr = to_string((int)(p*1000)), qstr = to_string((int)(q*1000)), 
            n1str = to_string(n1), n2str = to_string(n2);
     if(!cdfKnown)
     {
         system(("python3 GetCDF.py " + n1str + " " + to_string(1-sqrt(1-p)) 
         + " cdf" + n1str +"-"+ pstr).c_str()); 
         if(n1 != n2)
            system(("python3 GetCDF.py " + n2str + " " + to_string(1-sqrt(1-p)) 
                    + " cdf" + n2str + "-" +pstr).c_str()); 
         system(("python3 GetCDF.py " + to_string(n2) + " " + to_string(q)
         + " cdf"+n2str+qstr).c_str());
     }
    
     system(("./gensbm " + n1str + " " + n2str + " " + to_string(p) + " " + to_string(q) + 
      " cdf"+n1str+"-"+pstr + 
      " cdf"+n2str+"-"+pstr + 
      " cdf"+n2str+qstr
       ).c_str());
      totalGenTime += time(NULL) - timeGenStart;
}

int findPart(int n1, int n2, float p, float q)
{
     time_t timeLouvainStarts = time(NULL);
     //string graphName = "list_edge";
     string graphName = "graph" + to_string(n1) + to_string(n2)+to_string(int(p*1000))+to_string(int(q*1000));
     
     //call louvain, output in partition
     system(("./louvain "+graphName+" partition > /dev/null").c_str());
     //system(("./bilouvain "+graphName+" partition > /dev/null").c_str());
     system(("rm "+graphName).c_str()); 
     //check if output is correct
     
     FILE * partition = fopen("partition","r");
     int bla = 0, c1 = 0, c2 = 0;
     fscanf(partition, "%d %d", &bla, &c1);
     for(int i = 1; i < n1; i++)
     {
        int ci = 0;
        fscanf(partition, "%d%d", &bla, &ci);
        if(ci != c1)
        {
            fclose(partition);
            return false;
        }
     }
     fscanf(partition, "%d %d", &bla, &c2);
     if(c1 == c2)
         return false;

     for(int i = n1+1; i < n1+n2; i++)
     {
        int ci = 0;
        fscanf(partition, "%d%d", &bla, &ci);
        if(ci != c2)
        {
            fclose(partition);
            return false;
        }
     }
     
     fclose(partition);
     totalLouvainTime += time(NULL) - timeLouvainStarts;
     return true; 
}

int recovered(int n, float p, float q, FILE* output)
{
    int nbSuccess = 0;
    int cdfKnown = false;
    for(int i = 0; i < NB_TRIALS; i++)
    {
        genGraph(n/2, n/2, p, q, cdfKnown);
        //ATTENTION Imbalance aussi bipartition.c l327 340
        //genGraph(n/2 + n/10, n/2 - n/10, p, q, cdfKnown);
        
        cdfKnown = true;

        if(findPart(n/2, n/2, p, q))
        //if(findPart(n/2 + n/10, n/2 - n/10, p, q))
        {
            nbSuccess++;
            fprintf(output, "\t%d, %lf, %lf, %lf, %d\n", n, p, q, (p-q)/sqrt(p), true);
        }
        else
            fprintf(output, "\t%d, %lf, %lf, %lf, %d\n", n, p, q, (p-q)/sqrt(p), false);
    }
    
    //remove the generate CDF
    string pstr = to_string((int)(p*1000)), qstr = to_string((int)(q*1000)), 
            n1str = to_string(n/2), n2str = to_string(n/2);
    system(("rm cdf"+n1str+"-"+pstr + 
      //next line only when imbalance
      //" cdf"+n2str+"-"+pstr + 
      " cdf"+n2str+qstr).c_str());

    if (nbSuccess > NB_TRIALS / 2)
        return true;
    return false;
}

void runExpe(int init, int end, int step, FILE* output, float scale, float ERROR_BINARY, float density)
{
     for(int n = init; n < end; n+= step)
     {
         time_t timeStart = time(NULL);
         totalGenTime = 0, totalLouvainTime = 0;
         //bynary search on p with ration p/q constant
         //init with 
         float p = log(n)/sqrt(n)*scale, q = log(n)/sqrt(n)*scale/2;

         if(p > 1) 
             q = q / p, p = 1;
         float pmin = 0;
		 bool findStartPoint = false;
         //not 100% sure of success at the end, need to check that p changed 
         while (p - pmin > ERROR_BINARY)
         {
             //printf("%f %f\n", p, pmin);
             float m = (p+pmin)/2;
             bool res = recovered(n, m+density, m/2+density, output);
             fprintf(output, "%d p %lf pmin %lf m %lf res %d\n", n, p, pmin, m+density, res); 
             findStartPoint = findStartPoint || res;

			 if(res)
                p = m;
             else if(findStartPoint)
                pmin = m;
             else
                pmin = m, p = (p*2<1)?p*2:1;
             fprintf(output, "%lf %lf\n", p, pmin);
         }
         printf("%d %lf %lf %lf, gen %ld louvain %ld total %ld\n", n, (p+pmin)/2+density, (p+pmin)/4+density, (p+pmin)/4/sqrt((p+pmin)/2+density), totalGenTime, totalLouvainTime, time(NULL) - timeStart);
     }
}

int main(void)
{
     FILE * output = fopen("history", "w");

     runExpe(1000, 10*1000+1, 1000, output, 1.0/4.0, 0.001, 0);
     runExpe(15*1000, 50*1000+1, 5000, output, 1.0/6.0, 0.0001, 0);
     runExpe(100*1000, 100*1000+1, 5000, output, 1/8.0, 0.00005, 0);

     return 0;
}

