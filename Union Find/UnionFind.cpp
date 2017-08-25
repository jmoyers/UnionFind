// UnionFind.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace std::experimental::filesystem::v1;

class WeightedQuickUnion
{
private:
  struct Node
  {
    Node *parent{ nullptr };
    int val{ 0 };
    bool open{ false };
    int count{ 1 };
  };

  vector<Node> comps;
  Node top;
  Node bottom;

  int side_length;
  int max_depth = 1;

  Node *root(int p)
  {
    Node *n = &comps[p];
    return root(n);
  }

  Node *root(Node *n)
  {
    while (n->parent != nullptr) n = n->parent;
    return n;
  }

  bool connected(int p, int q)
  {
    if (p == q) return true;
    return root(p) == root(q);
  }

  int rcToIndex(int r, int c)
  {
    return ((r - 1) * side_length) + (c - 1);
  }

  bool isOpen(int r, int c)
  {
    if (r < 1) return false;
    if (c < 1) return false;
    if (r > side_length) return false;
    if (c > side_length) return false;
    int index = rcToIndex(r, c);
    return comps[index].open;
  }

  void join(int p, int q)
  {
    if (p < 0 || q < 0) return;
    if (connected(p, q)) return;

    Node *pParent = root(p);
    Node *qParent = root(q);

    if (pParent->count > qParent->count)
    {
      qParent->parent = pParent;
      qParent->count += pParent->count;
      max_depth = std::max(max_depth, comps[q].count);
    }
    else
    {
      pParent->parent = qParent;
      pParent->count += qParent->count;
      max_depth = std::max(max_depth, comps[p].count);
    }
  }
public:
  WeightedQuickUnion(int side_length) : comps(static_cast<int>(pow(side_length, 2)))
    , top(Node()), bottom(Node()), side_length(side_length)
  {
    int i = 0;

    for (auto &c : comps)
    {
      c = Node();
      c.val = i++;
    }

    // artifically join top and bottom node
    for (i = 0; i < side_length; i++)
    {
      comps[i].parent = &top;
      comps[comps.size() - i - 1].parent = &bottom;
    }
  }

  int getDepth()
  {
    return max_depth;
  }

  int getSideLength()
  {
    return side_length;
  }

  size_t size()
  {
    return comps.size();
  }

  void open(int r, int c)
  {
    int index = rcToIndex(r, c);

    if (index > comps.size()) return;

    comps[index].open = true;

    if (isOpen(r, c - 1))
    {
      join(index, rcToIndex(r, c - 1));
    }
    if (isOpen(r, c + 1))
    {
      join(index, rcToIndex(r, c + 1));
    }
    if (isOpen(r - 1, c))
    {
      join(index, rcToIndex(r - 1, c));
    }
    if (isOpen(r + 1, c))
    {
      join(index, rcToIndex(r + 1, c));
    }
  }

  bool percolates()
  {
    return root(&top) == root(&bottom);
  }
};

int main()
{
  directory_iterator dir(path("percolation"));

  for (auto file : dir) {
    if (file.path().string().find(".txt") == string::npos)
    {
      continue;
    }

    ifstream inf;

    inf.open(file);

    if (!inf)
    {
      return 1;
    }

    vector<pair<int, int>> joins;
    int side_length;

    inf >> side_length;

    while (!inf.eof())
    {
      int q, p;
      inf >> p;
      if (p == 0) {
        inf >> ws;
        continue; // random file format change? thanks bros
      }
      inf >> q;
      inf >> ws;
      joins.push_back(make_pair(p, q));
    }

    WeightedQuickUnion uf(side_length);
    for (auto &j : joins)
    {
      uf.open(j.first, j.second);
    }

    cout << "file: " << file << endl;
    cout << "side length: " << uf.getSideLength() << endl;
    cout << "depth: " << uf.getDepth() << endl;
    cout << "percolates: " << uf.percolates() << endl << endl;

    inf.close();
  }

  cin.get();

  return 0;
}

