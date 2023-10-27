// Copyright 2017 Peter Hristov
#include <cassert>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

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
int BFS(const vector<vector<int>> &T, const vector<int> &h, const int s)
{
    queue<int> q;
    vector<int> p(T.size(), -1);
    vector<int> d(T.size(), -1);
    vector<int> w(T.size(), -1);
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

                maxW = max(maxW, w[v]);
            }
        }
    }

    //return std::make_pair(w, maxW);
    return maxW;
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
    auto data = readTree();
    auto tree = data.first;
    auto heights = data.second;

    //printTree(tree, heights);

    //int root = findRoot(tree);
    //cout << root << " ";

    //int root = 10;

    //parent[root] = root;


    //int s = root;

    gettimeofday(&startTime, NULL);

    int maxW = 0;

    for(int i = 0 ; i < tree.size(); i++)
    {
        //pair<vector<int>, int> result = BFS(tree, heights, i);
        int resultW = BFS(tree, heights, i);
        maxW = max(maxW, resultW);
    }

    gettimeofday(&endTime, NULL);

    long nSeconds = endTime.tv_sec - startTime.tv_sec;
    long nMSeconds = endTime.tv_usec - startTime.tv_usec;

    if (nMSeconds < 0) { // negative microsecond delta
        nSeconds--;
        nMSeconds += 1000000;
    } // negative microsecond delta


    cout << maxW << endl;
    printf("%3ld.%06ld\n", nSeconds, nMSeconds);

    
    //cout << endl << "Height of " << s << " is   : " << H[s];
    //cout << endl << "Optimal of " << s << " is  : " << O[s];
    //cout << endl << "Max Children are : ";
    //printArray(L[s]);
    //cout << endl;
    //cout << "-----------------------------------";
    //cout << endl;

    return 0;
}
