#include <cassert>
#include <iostream>
#include "csr.h"

#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>

using std::cout;
using std::endl;
using std::vector;
//offset 1001
void bottom_up_step(csr_t* csr, vector<vid_t>& frontier, vector<vid_t>& next, vector<int>& parents)
{
    for(int v=0; v<csr->get_vcount(); v++) 
    {
        if (parents[v] == -1) {
            for (vid_t n_idx=*(csr->offset+v); n_idx<*(csr->offset+v+1); n_idx++) {
                vid_t n = *(csr->nebrs+n_idx);
                if (frontier[n] == 1) {
                    parents[v] = n;
                    next[v] = 1;
                    break;
                }
            }
        }
    }
}

void bottom_up_step_parallel(csr_t* csr, vector<vid_t>& frontier, vector<vid_t>& next, vector<int>& parents)
{
#pragma omp parallel
{
    //for (int i=id; i<neighbours.size(); i+=num_threads) {
    
    // int id = omp_get_thread_num();
    // int num_threads = omp_get_num_threads();   

    // for(int v=id; v<csr->get_vcount(); v+=num_threads) 
    #pragma omp for
    for(int v=0; v<csr->get_vcount(); v++)
    {
        if (parents[v] == -1) {
            for (vid_t n_idx=*(csr->offset+v); n_idx<*(csr->offset+v+1); n_idx++) {
                vid_t n = *(csr->nebrs+n_idx);
                if (frontier[n] == 1) {
                    parents[v] = n;
                    next[v] = 1;
                    break;
                }
            }
        }
    }
}
}

void bfs_bottom_up(graph_t& g, vid_t root, bool parallel, bool verbose=false)
{
    csr_t* csr = &g.csr;
    vector<vid_t> frontier(csr->get_vcount(), 0);
    vector<vid_t> next(csr->get_vcount(), 0);
    vector<int> parents(csr->get_vcount(), -1);
    frontier[root] = 1;

    int layer_count = 0;
    while (std::count(frontier.begin(), frontier.end(), 1)!=0) {
        if (verbose) {
            cout << "Level " << layer_count <<": " << std::count(frontier.begin(), frontier.end(), 1) << endl;
        }
        if (parallel) {
            //bottom_up_step_parallel(csr, &frontier[0], &next[0], &parents[0]);    
            bottom_up_step_parallel(csr, frontier, next, parents);    
        } else {
            bottom_up_step(csr, frontier, next, parents);
        }
        
        frontier.swap(next);
        std::fill(next.begin(), next.end(), 0);
        layer_count++;
    }
}

void bfs(graph_t& g, vid_t root)
{
    csr_t* csr = &g.csr;
    //csr_t* csc = &g.csc;
    
    //TODO
	vector<vid_t> visited;
    vector<vid_t> queue;

    visited.push_back(root);
    queue.push_back(root);
    vid_t layer_count = 0;

    while (queue.size()>0) {
        layer_count ++;
        vector<vid_t> neighbours;

        for(auto s:queue) {
            for (vid_t n=*(csr->offset+s); n<*(csr->offset+s+1); n++) {
                neighbours.push_back(*(csr->nebrs+n));
            }
        }

        queue.clear();
        int count = 0;

        for (auto neighbour: neighbours) {
            if (!std::count(visited.begin(), visited.end(), neighbour)) {
                visited.push_back(neighbour);
                queue.push_back(neighbour);
                count++;
            }
        }

        // if (count!=0) 
        //     cout << "Level " << layer_count <<": " << queue.size() << endl;
    }
    //cout << "root = " << root << endl; 
    //print bfs tree here
    //i.e. how many vertex in each level
}


void bfs_parallel(graph_t& g, vid_t root) {
    csr_t* csr = &g.csr;
    //csr_t* csc = &g.csc;
    
    //TODO
	vector<vid_t> visited;
    vector<vid_t> queue;

    visited.push_back(root);
    queue.push_back(root);
    //cout << "Level 0: " << queue.size() << endl;
    vid_t layer_count = 0;

    while (queue.size()>0) {
        layer_count ++;
        vector<vid_t> neighbours;

        for(auto s:queue) {
            for (vid_t n=*(csr->offset+s); n<*(csr->offset+s+1); n++) {
                neighbours.push_back(*(csr->nebrs+n));
            }
        }

        queue.clear();
        int count = 0;
        
#pragma omp parallel
{
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        for (int i=id; i<neighbours.size(); i+=num_threads) {

            vid_t neighbour = neighbours[i];

            #pragma omp critical
            if (!std::count(visited.begin(), visited.end(), neighbour)) {
                visited.push_back(neighbour);
                queue.push_back(neighbour);
                count++;
            }
        }
}
        // if (count!=0) 
        //     cout << "Level " << layer_count <<": " << queue.size() << endl;
    }
    //cout << "root22 = " << root << endl; 
}

void run_bfs(graph_t& g, vid_t root)
{
    const int iter = 1000;

    auto t1=std::chrono::steady_clock::now();

    for (int i=0;i<iter;i++)
    {
        bfs_bottom_up(g, root, true);
    }
    auto t2=std::chrono::steady_clock::now();

    for (int i=0;i<iter;i++)
    {
        bfs_bottom_up(g, root, false);
    }
    auto t3=std::chrono::steady_clock::now();
    for (int i=0;i<iter;i++)
    {
        bfs(g, root);
    }
    auto t4=std::chrono::steady_clock::now();

    for (int i=0;i<iter;i++)
    {
        bfs_parallel(g, root);
    }
    auto t5=std::chrono::steady_clock::now();
    cout<<"bottom up parallel: " << chrono::duration<double,std::milli>(t2-t1).count()/iter<< "ms"<<endl ;
    cout<<"bottom up : " << chrono::duration<double,std::milli>(t3-t2).count()/iter<<"ms"<<endl ;
    cout<<"normal bfs: " << chrono::duration<double,std::milli>(t4-t3).count()/iter<<"ms"<<endl ;
    cout<<"normal bfs parallel: " <<chrono::duration<double,std::milli>(t5-t4).count()/iter<<"ms"<<endl ;

   
    //print bfs tree here
    //i.e. how many vertex in each level
    bfs_bottom_up(g, root, true, true);
}