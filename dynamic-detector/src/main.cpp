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

vector<int> parent;
vector<int> O;
vector<int> H;
vector<set<int>> L;

vector<vector<int>> Levels;

bool isKink(int s, int u, int v, const vector<int> &h)
{
    return (h[s] > h[u] && h[s] > h[v]) || (h[s] < h[u] && h[s] < h[v]);
}

void printArray(const set<int> &data)
{
    // for (int i = 0; i < data.size(); i++)
    for (const auto &i : data)
    {
        printf("%3d ", i);
    }
}

void ComputeOptimalValues(const vector<vector<int>> &T, const vector<int> &h, const int s)
{
    if (1 == T[s].size() && s != parent[s])
    {
        O[s] = 0;
        H[s] = 0;

        return;
    }


    // See if children are not defined.
    for (auto u : T[s])
    {
        if (u == parent[s]) { continue; }

        if (H[u] == -1 || O[u] == -1)
        {
            //assert(H[u] != -1 && O[u] != -1);
            //cout << "At " << s << " and " << u << " is not defined. Parent - " << parent[s] << endl;
            //cout << "HALP!!!";
        }
    }


    // Calculate Height.
    for (auto u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        if (L[u].empty())
        {
            H[s] = max(H[s], H[u]);
        }
        else
        {
            for (auto v : L[u])
            {
                H[s] = max(H[s], H[u] + isKink(u, s, v, h));
            }
        }
    }

    // Build L[u]
    for (auto u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        if (L[u].empty())
        {
            if (H[s] == H[u])
            {
                L[s].insert(u);
            }
        }
        else
        {
            for (auto v : L[u])
            {
                if (H[s] == H[u] + isKink(u, s, v, h))
                {
                    L[s].insert(u);
                }
            }
        }
    }

    int maxCombine = 0;

    // Find the max combine.
    for (const auto &u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        for (const auto &v : T[s])
        {
            if (v == parent[s] || v == u)
            {
                continue;
            }

            int temp = H[u] + H[v];

            // Find one which is kink
            if (!L[u].empty())
            {
                for (auto t : L[u])
                {
                    if (isKink(u, t, s, h))
                    {
                        temp++;
                        break;
                    }
                }
            }

            // Find one which is kink
            if (!L[v].empty())
            {
                for (auto t : L[v])
                {
                    if (isKink(v, t, s, h))
                    {
                        temp++;
                        break;
                    }
                }
            }

            if (isKink(s, u, v, h))
            {
                temp++;
            }

            maxCombine = max(maxCombine, temp);
        }
    }

    maxCombine = max(H[s], maxCombine);

    // Get max optimal solution for children.
    for (const auto &u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        O[s] = max(O[s], O[u]);
    }

    O[s] = max(O[s], maxCombine);


    //cout << endl << "Height of " << s << " is   : " << H[s];
    //cout << endl << "Combine of " << s << " is  : " << maxCombine;
    //cout << endl << "Optimal of " << s << " is  : " << O[s];
    //cout << endl << "Max Children are : ";
    //printArray(L[s]);
    //cout << endl;
    //cout << "-----------------------------------";
    //cout << endl;
}


void ComputeWDiameter(const vector<vector<int>> &T, const vector<int> &h)
{
    int n = T.size() - 1;

    while(n >= 0)
    {
        for(const int &s : Levels[n])
        {
            ComputeOptimalValues(T, h, s);
        }

        n--;
    }
}

void BFS(const vector<vector<int>> &T, const int s)
{
    queue<int> q;
    vector<int> dist(T.size(), -1);
    //vector<int> parent(T.size(), -1);

    parent[s] = s;
    dist[s] = 0;
    q.push(s);

    Levels[0].push_back(s);

    while(!q.empty())
    {
        int u = q.front();
        q.pop();

        for(const int &v: T[u])
        {
            if(-1 == parent[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + 1;

                Levels[dist[v]].push_back(v);

                q.push(v);
            }
        }
    }
}

void DFS(const vector<vector<int>> &T, const vector<int> &h, const int s)
{
    cout << "Forwards at  : " << s << endl;

    // BASE CASE
    // When at a leaf which is not the root
    if (1 == T[s].size() && s != parent[s])
    {
        return;
    }

    // Forwards
    for (auto v : T[s])
    {
        if (-1 == parent[v])
        {
            parent[v] = s;
            DFS(T, h, v);
        }
    }

    // No Leaves can go here.
    cout << "Backwards at : " << s << endl;

    // Calculate Height.
    for (auto u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        if (L[u].empty())
        {
            H[s] = max(H[s], H[u]);
        }
        else
        {
            for (auto v : L[u])
            {
                H[s] = max(H[s], H[u] + isKink(u, s, v, h));
            }
        }
    }

    // Build L[u]
    for (auto u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        if (L[u].empty())
        {
            if (H[s] == H[u])
            {
                L[s].insert(u);
            }
        }
        else
        {
            for (auto v : L[u])
            {
                if (H[s] == H[u] + isKink(u, s, v, h))
                {
                    L[s].insert(u);
                }
            }
        }
    }

    int maxCombine = 0;

    // Find the max combine.
    for (const auto &u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        for (const auto &v : T[s])
        {
            if (v == parent[s] || v == u)
            {
                continue;
            }

            int temp = H[u] + H[v];

            // Find one which is kink
            if (!L[u].empty())
            {
                for (auto t : L[u])
                {
                    if (isKink(u, t, s, h))
                    {
                        temp++;
                        break;
                    }
                }
            }

            // Find one which is kink
            if (!L[v].empty())
            {
                for (auto t : L[v])
                {
                    if (isKink(v, t, s, h))
                    {
                        temp++;
                        break;
                    }
                }
            }

            if (isKink(s, u, v, h))
            {
                temp++;
            }

            maxCombine = max(maxCombine, temp);
        }
    }

    maxCombine = max(maxCombine, H[s]);

    // Get max optimal solution for children.
    for (const auto &u : T[s])
    {
        if (u == parent[s])
        {
            continue;
        }

        O[s] = max(O[s], O[u]);
    }

    O[s] = max(O[s], maxCombine);


    //cout << endl << "Height of " << s << " is   : " << H[s];
    //cout << endl << "Combine of " << s << " is  : " << maxCombine;
    //cout << endl << "Optimal of " << s << " is  : " << O[s];
    //cout << endl << "Max Children are : ";
    //printArray(L[s]);
    //cout << endl;
    //cout << "-----------------------------------";
    //cout << endl;
}


/**
 * Write tree grid to STDIN
 */
void printTree(const std::vector<std::vector<int>> &data, const vector<int> &h)
{
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << std::endl
                  << std::setw(4) << i << " - (" << setw(3) << h[i] << ")"
                  << ":";
        for (const auto element : data[i])
        {
            std::cout << std::setw(4) << element;
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
    auto data = readTreeBinary(argv[1]);
    auto tree = data.first;
    auto heights = data.second;

    O = vector<int>(tree.size(), -1);
    H = vector<int>(tree.size(), -1);
    L = vector<set<int>>(tree.size());

    Levels = vector<vector<int>>(tree.size());

    parent = vector<int>(tree.size(), -1);

    gettimeofday(&startTime, NULL);

    int root = findRoot(tree);
    //int root = 10;
    
    //cout << root;

    parent[root] = root;

    //printTree(tree, heights);

    //DFS(tree, heights, root);
    BFS(tree, root);
    ComputeWDiameter(tree, heights);

    //printTree(Levels, heights);

    int s = root;

    gettimeofday(&endTime, NULL);

    
    long nSeconds = endTime.tv_sec - startTime.tv_sec;
    long nMSeconds = endTime.tv_usec - startTime.tv_usec;

    if (nMSeconds < 0) { // negative microsecond delta
        nSeconds--;
        nMSeconds += 1000000;
    } // negative microsecond delta


    cout << O[s];
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
