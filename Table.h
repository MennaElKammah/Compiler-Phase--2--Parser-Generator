#ifndef TABLE_H
#define TABLE_H
#include <bits/stdc++.h>
#include "Node.h"

using namespace std;

class Table {
private:
  void initialize_table();
  void set_table();
  map<string, int > row_names;
  map<string, int > col_names;
  vector< vector<Node> > table;
  int rows;
  int cols;
  string epslon = "eps";
  map<Node, vector<pair <set<string>, Node > > > map_of_first;
  map<Node, set<string> > map_of_follow;

public:
  Table(map<string, int >, map<Node, vector<Node> >,
    map<Node, vector<pair <set<string>, Node > > > ,
    map<Node, set<string> >);
  vector< vector<Node> > get_table ();
  void construct_prod(map<Node, vector<Node> >);
  void set_node(string, string, Node);
  Node get_node(string, string);
  void print_table ();
  Node *error_node = new Node(true, true, "error");
  Node *sync_node = new Node(true, true, "sync");
  Node *epslon_node = new Node(true, true, "eps");
};

#endif // TABLE_H
