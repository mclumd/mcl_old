#include "mc_utils/dijkstra.h"
#include <iostream>

using namespace umbc;

#define INF   999999
#define UNDEF -99999

void extract_min(list<int>& S,list<int>& Q,
		 int* dist,
		 int* cost_mx,int num_pts,
		 int* start,int* end) {
  int mc=INF;
  for (list<int>::iterator Qi = Q.begin(); Qi != Q.end(); Qi++) {
    for (list<int>::iterator Si = S.begin(); Si != S.end(); Si++) {
      int edgeva = dist[*Si]+cost_mx[(*Qi)*num_pts+(*Si)];
      // this one is broken: cost_mx[(*Qi)*num_pts+(*Si)]
      if (edgeva < mc) {
	*start=*Si;
	*end  =*Qi;
	mc = edgeva;
      }
    }
  }
  // cout << "pulling " << *start << "->" << *end << " cost=" << mc << endl;
}

list<int> trace(int* prev,int num,int src,int v,int u) {
  list<int> r;
  if (u!=src) {
    r.push_front(u);
    // cout << "adding " << u << " to trace." << endl;
  }
  if (v!=src) {
    r.push_front(v);
    // cout << "adding " << v << " to trace." << endl;
  }
  while((prev[v] != src) && (prev[v]!=UNDEF)) {
    // cout << "adding " << prev[v] << " to trace." << endl;
    r.push_front(prev[v]);
    v=prev[v];
  }
  return r;
}

list<int> dijkstra::shortestPath(int* cost_mx,int num_pts,int source,int dest,int *cost) {
  int* dist = new int[num_pts];
  int* prev = new int[num_pts];
  list<int> Q;
  list<int> S;
  for (int i=0;i<num_pts;i++) {
    dist[i]=INF;
    prev[i]=UNDEF;
    Q.push_back(i);
  }
  dist[source]=0;
  Q.remove(source);
  S.push_back(source);
  while (!Q.empty()) {
    int v,u;
    extract_min(S,Q,dist,cost_mx,num_pts,&v,&u);
    // cout << "extracted: v=" << v << ",u=" << u << endl;
    if (u == dest) {
      list<int> rv = trace(prev,num_pts,source,v,u);
      // dist[u];
      delete[] dist;
      delete[] prev;
      if (cost != NULL) {
	*cost=dist[v]+cost_mx[u*num_pts+v];
      }
      return rv;
    }
    Q.remove(u);
    S.push_front(u);
    // cout << "DIJ: " << v << " is added, arc from " << u << endl;
    dist[u]=dist[v]+cost_mx[u*num_pts+v];
    prev[u]=v;
  }
  list<int> brv;
  // cout << "uh oh, dijkstra failed." << endl;
  if (cost != NULL) *cost=-1;
  return brv;
}
