import queue
import numpy as np
import collections
import pygraph as pg

import datetime

def memoryview_to_np(memview, nebr_dt):
    arr = np.array(memview, copy=False)
    #a = arr.view(nebr_dt).reshape(nebr_reader.get_degree())
    a = arr.view(nebr_dt)
    a = [x[0] for x in a]
    return a

def csr():
    outdir = ""
    num_sources = 1 
    num_threads = 2 
    graph  = pg.init(1,1, outdir, num_sources, num_threads) # Indicate one pgraph, and one vertex type
    
    tid0 = graph.init_vertex_type(1000, True, "gtype"); # initiate the vertex type
    edge_dt = np.dtype([('src', np.int32), ('dst', np.int32)])
    flags = pg.enumGraph.eUdir;
    pgraph = graph.create_schema(flags, tid0, "friend", edge_dt); #initiate the pgraph
    
    ifile = "smallworld.txt";
    dd = np.zeros(1024, edge_dt); 
    edge_count = 0
    with open(ifile) as f:
        for line in f: # read rest of lines
            x = line.split();
            #print(x);
            if x[0] != "#": 
                dd[edge_count] = (x[0], x[1]);
                edge_count += 1;
                if (edge_count == 1024):
                    pgraph.add_edges(dd, 1024); # You can call this API many times, if needed
                    edge_count = 0;

    #print(edge_count);
    pgraph.add_edges(dd, edge_count); # You can call this API many times, if needed
    pgraph.wait(); # required for the time-being. You cannot add edges after this API.
   

    offset_csr1, offset_csc1, nebrs_csr1, nebrs_csc1 = pg.create_csr_view(pgraph);
    offset_dt = np.dtype([('offset', np.int32)])
    csr_dt =  np.dtype([('dst', np.int32)])

    offset_csr = memoryview_to_np(offset_csr1, offset_dt);
    offset_csc = memoryview_to_np(offset_csc1, offset_dt);
    nebrs_csr  = memoryview_to_np(nebrs_csr1, csr_dt);
    nebrs_csc  = memoryview_to_np(nebrs_csc1, csr_dt);
    return offset_csr, nebrs_csr, offset_csc, nebrs_csc

def bfs(offset_csr, nebrs_csr, start_node=0):
    visited = set()
    queue = []

    visited.add(start_node)
    queue.append(start_node)

    while queue:
        s = queue.pop(0)
        neighbours = nebrs_csr[offset_csr[s]:offset_csr[s+1]]
      
        for neighbour in neighbours:
            if neighbour not in visited:
                visited.add(neighbour)
                queue.insert(0,neighbour)

def bfs(offset_csr, nebrs_csr, start_node=0):
    visited = []
    queue = []

    visited.append(start_node)
    queue.append(start_node)
    print("Level 0:", len(queue), start_node)
    layer_count = 0
    while queue:
        layer_count+=1
        neighbours = []
        for s in queue:
            neighbours.append(nebrs_csr[offset_csr[s]:offset_csr[s+1]])

        queue.clear()
        count = 0
        for neighbour in neighbours:
            for neigh in neighbour:
                if neigh not in visited:
                    visited.append(neigh)
                    queue.append(neigh)
                    count+=1
        if count != 0:
            #print("Level {}:".format(layer_count), count, visited[-count:])
            print("Level {}:".format(layer_count), count)

if __name__ == "__main__":
    offset_csr, nebrs_csr, offset_csc, nebrs_csc = csr()
    bfs(offset_csr, nebrs_csr, start_node=0)