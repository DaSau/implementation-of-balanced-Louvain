Code accompanying NeurIPS'20 paper "On the Power of Louvain in the Stochastic Block Model"



To run the code, go into the corresponding folder, then do:

make

./expe

this will write on the standard output the results of the experiments, with the following syntax:

n p q (p-q)/sqrt(p) "gen" t1 "louvain" t2 "total" t3

where n, p, and q are the parameters of the experiment (respectively the number of nodes, the intra-cluster density and the inter-cluster density), t1 is the time to generate the SBM, t2 is the time taken by Louvain's algorithm and t3 the total running time.



To find those p and q values, our algorithm makes a binary search. All calls make to Louvain algorithm can be tracked in the file history.



The initial implementation of Louvain is from Jean-Loup Guillaume https://github.com/jlguillaume/louvain
