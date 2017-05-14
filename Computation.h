#ifndef COMPUTATION_H
#define COMPUTATION_H
#include <bits/stdc++.h>
#include "Node.h"
using namespace std;

class Computation {
public:
  Computation (map<Node, std::vector<Node> >&, Node);
  map<Node, std::vector<Node> > components;
  Node *start_node;
  string epslon = "eps";
  void construct_first ();
  void construct_follow ();
  void print_first ();
  void print_follow ();
  map<Node, vector<pair <set<string>, Node > > > get_map_of_first ();
  map<Node, set<string> > get_map_of_follow ();
private:
  map<Node, vector<pair <set<string>, Node > > > map_of_first;
  map<Node, set<string> > map_of_follow;
  map <Node, vector<pair<Node, pair<Node, int> > > > production_map;
  void print_hash_production_rules ();
  void hash_production_rules ();
  set<string> get_terminals (Node);
  vector<Node> build_order_for_follow ();
};

#endif // COMPUTATION_H
