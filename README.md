# cs636
There are four implementation of bfs:

1. My original implmentation of bfs, `bfs(graph_t& g, vid_t root)`
2. Parallel version implementation of bfs, `bfs_parallel(graph_t& g, vid_t root)`
3. bottom-up version implementation of bfs, `bfs_bottom_up(graph_t& g, vid_t root)`
4. parallel bottom-up version implementation of bfs, `bottom_up_step_parallel(graph_t& g, vid_t root)`.

## Results
bottom up parallel: 0.0235811ms

bottom up : 0.0446245ms

normal bfs: 0.607461ms

normal bfs parallel: 2.50072ms

## Analysis
1. It is expected that 'normal bfs parallel' is the slowest, because it uses sychronization operation.
2. Parallel version of bottom up is 2 times faster than the naive bottom up version. 

# Kernel
If you don't find pybind11 directory or its files, please run this command.
`git submodule update --init --recursive`

See `https://git-scm.com/book/en/v2/Git-Tools-Submodules` for more information on how to work with sub-modules

## Compilation of Kernel: At the top level directory
```
mkdir build
cd build
cmake ../kernel
make
```
