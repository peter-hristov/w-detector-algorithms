// Copyright 2017 Peter Hristov
#include <cassert>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
#include <limits>

#include <sys/time.h>

using namespace std;

// u ~> s ~> v
int isKink(int u, int s, int v, const vector<int> &h)
{
   if ((h[s] > h[u] && h[s] > h[v]) || (h[s] < h[u] && h[s] < h[v]))
   {
       return 1;
   }

   return 0;
}

void printArray(const vector<int> &data)
{
     for (int i = 0; i < data.size(); i++)
    //for (const auto &i : data)
    {
        printf("%d - %d\n", i, data[i]);
    }
}

//pair<vector<int>, int> BFS(const vector<vector<int>> &T, const vector<int> &h, const int s)
pair<int, int> BFS(const vector<vector<int>> &T, const vector<int> &h, const int s)
{
    queue<int> q;
    vector<int> p(T.size(), -1);
    vector<int> d(T.size(), -1);
    vector<int> w(T.size(), -1);
    
    int mostDistant = -1;

    int maxW = 0;

    q.push(s);
    p[s] = s;
    w[s] = 0;
    d[s] = 0;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for(auto v : T[u])
        {
            // If v has not been discovered yet.
            if (p[v] == -1)
            {
                q.push(v);
                p[v] = u;
                d[v] = d[u] + 1;

                // Might not need this, but just in case.
                if (p[v] == s)
                {
                    w[v] = 0;
                }
                else
                {
                    w[v] = w[u] + isKink(p[u], u, v, h);
                }

                if (w[v] >= maxW)
                {
                    maxW = w[v];
                    mostDistant = v;

                }

            }
        }
    }

    //return std::make_pair(w, maxW);
    //return maxW;
    return std::make_pair(maxW, mostDistant);
}

/**
 * Write tree grid to STDIN
 */
void printTree(const std::vector<std::vector<int>> &data, const vector<int> &h)
{
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << std::endl
                  << std::setw(4) << i << " (h=" << setw(3) << h[i] << ")"
                  << ": ";
        for (const auto element : data[i])
        {
            std::cout << std::setw(4) << element << ", ";
        }
    }

    std::cout << std::endl;
}

int checkWDiameter(const vector<int> &path, const vector<int> &h)
{
    int wd = 0;

    for (int i = 1; i < path.size() - 1; i++)
    {
        if ((h[path[i]] > h[path[i - 1]] && h[path[i]] > h[path[i + 1]]) || (h[path[i]] < h[path[i - 1]] && h[path[i]] < h[path[i + 1]]))
        {
            wd++;
        }
    }

    return wd;
}

bool isPath(vector<vector<int>> tree, vector<int> path)
{
    for (int u = 0; u < path.size() - 1; u++)
    {
        bool foundEdge = false;

        for (auto v : tree[path[u]])
        {
            if (v == path[u + 1])
            {
                foundEdge = true;
                break;
            }
        }

        if (false == foundEdge)
        {
            return false;
        }
    }

    return true;
}

int findRoot(const vector<vector<int>> &tree)
{
    for (int i = 0; i < tree.size(); i++)
    {
        if (tree[i].size() != 0)
        {
            return i;
        }
    }

    return -1;
}

pair<vector<vector<int>>, vector<int>> readTreeSuperBinary(string filename)
{
    FILE *file;
    file = fopen(filename.c_str(),"r");
    //file.open (filename, ios::in | ios::binary);

    if (!file){ throw "Could not open binary file for writing.\n"; }

    long long vertices;
    long long *rawData;

    long long superNodesCount;
    long long *superArcs;
    long long *superNodes;

    // NumIterations
    fread(&vertices, sizeof(vertices), 1, file);

    // SortOrderNumber
    fread(&vertices, sizeof(vertices), 1, file);

    // Sort order raw data
    rawData = new long long[vertices];
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Nodes number
    fread(&vertices, sizeof(vertices), 1, file);
    // Nodes data
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Arcs number
    fread(&vertices, sizeof(vertices), 1, file);
    // Arcs data
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Superparents number
    fread(&vertices, sizeof(vertices), 1, file);
    // Superparents data
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Read the number of supernodes
    fread(&superNodesCount, sizeof(superNodesCount), 1, file);

    // Read the supernodes
    superNodes = new long long[superNodesCount];
    fread(superNodes, sizeof(superNodes[0]), superNodesCount, file);

    // Read the supernodes again (filler)
    fread(&superNodesCount, sizeof(vertices), 1, file);

    // Read the superArcs
    superArcs = new long long[superNodesCount];
    fread(superArcs, sizeof(superArcs[0]), superNodesCount, file);

    cout << "The tree has " << vertices << " nodes and " << superNodesCount << " supernodes " << endl;

    vector<int> heights(superNodesCount);
    vector<vector<int>> tree(superNodesCount);

    for (int i = 0; i < heights.size(); i++)
    {
        // Unmask
        //rawData[i] = rawData[i] & std::numeric_limits<long long>::max() / 16;
        //This is equivalent to the MaskedIndex function from ./vtkm/filter/scalar_topology/worklet/contourtree_augmented/Types.h
        superArcs[i] = superArcs[i] & (std::numeric_limits<long long>::max() / 16);
        superNodes[i] = superNodes[i] & (std::numeric_limits<long long>::max() / 16);

    }

    for (int i = 0; i < heights.size(); i++)
    {
        // With the superstructure the height is no implicit in the sorting of the vertices
        heights[i] = superNodes[i];

        tree[i].push_back(superArcs[i]);
        tree[superArcs[i]].push_back(i);
    }

    delete [] rawData;
    delete [] superArcs;
    delete [] superNodes;

    return {tree, heights};
}

pair<vector<vector<int>>, vector<int>> readTreeBinary(string filename)
{
    FILE *file;
    file = fopen(filename.c_str(),"r");
    //file.open (filename, ios::in | ios::binary);

    if (!file){ throw "Could not open binary file for writing.\n"; }

    long long vertices;
    long long *rawData;

    // NumIterations
    fread(&vertices, sizeof(vertices), 1, file);

    // SortOrderNumber
    fread(&vertices, sizeof(vertices), 1, file);

    // Sort order raw data
    rawData = new long long[vertices];
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Nodes number
    fread(&vertices, sizeof(vertices), 1, file);
    // Nodes data
    fread(rawData, sizeof(rawData[0]), vertices, file);

    // Arcs number
    fread(&vertices, sizeof(vertices), 1, file);
    // Arcs data
    fread(rawData, sizeof(rawData[0]), vertices, file);

    vector<int> heights(vertices);
    vector<vector<int>> tree(vertices);

    for (int i = 0; i < heights.size(); i++)
    {
        // Unmask
        rawData[i] = rawData[i] & std::numeric_limits<long long>::max() / 16;

    }

    for (int i = 0; i < heights.size(); i++)
    {
        heights[i] = i;
        tree[i].push_back(rawData[i]);
        tree[rawData[i]].push_back(i);
    }

    delete [] rawData;

    return {tree, heights};
}

pair<vector<vector<int>>, vector<int>> readTree()
{
    int vertices = 0;

    cin >> vertices;

    vector<int> heights(vertices);
    vector<vector<int>> tree(vertices);

    int u, v;

    for (int i = 0; i < heights.size(); i++)
    {
        cin >> u >> v;
        heights[v] = u;
        // heights[i] = i;
    }

    while (cin >> u >> v)
    {
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    return {tree, heights};
}

struct timeval startTime;
struct timeval endTime;

int main(int argc, char *argv[])
{
    //auto data = readTreeBinary(argv[1]);
    auto data = readTreeSuperBinary(argv[1]);
    auto tree = data.first;
    auto heights = data.second;

    //printTree(tree, heights);

    int root = findRoot(tree);

    gettimeofday(&startTime, NULL);

    pair<int, int>result1 = BFS(tree, heights, root);
    pair<int, int>result2 = BFS(tree, heights, result1.second);

    gettimeofday(&endTime, NULL);

    long nSeconds = endTime.tv_sec - startTime.tv_sec;
    long nMSeconds = endTime.tv_usec - startTime.tv_usec;

    if (nMSeconds < 0) { // negative microsecond delta
        nSeconds--;
        nMSeconds += 1000000;
    } // negative microsecond delta


    cout << result2.first;
    //printf("%3ld.%06ld\n", nSeconds, nMSeconds);

    
    //cout << endl << "Height of " << s << " is   : " << H[s];
    //cout << endl << "Optimal of " << s << " is  : " << O[s];
    //cout << endl << "Max Children are : ";
    //printArray(L[s]);
    //cout << endl;
    //cout << "-----------------------------------";
    //cout << endl;

    return 0;
}
