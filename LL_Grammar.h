#ifndef LL_Grammar_H
#define LL_Grammar_H

#include <vector>
#include <string>
#include <stdlib.h>
#include <map>
#include <regex>
#include <iostream>
#include <utility>
#include "CFG_Parser.h"
#include "Node.h"

using namespace std;

class LL_Grammar {
private:
  static unsigned int dash_node_num;
  Prod_Itr *prod_itr;
  map<Node, vector<Node> > grammar;
  map<string, Node> added_nodes; //holds the added nodes to the grammar map, as <name, node>
  // void build_prods_as_nodes (); //fills the first version of the grammar map
  vector<Node> eliminate_left_fact (Node, vector<Node>,
    map<Node, vector<Node> >::iterator); //eliminates left factoring from grammar and edites the map
  vector<Node> substitute (vector<Node>, Node);
  vector<string> split(string, regex);
  string concate_strings (vector<string>); //concats elements separated by "_"
  pair <string, vector<int> > find_common (vector<string>);//finds the longest common prefix of the given strings vector
  string find_common_util (string, string);
  Node eliminate_imm_left_rec (Node, vector<Node>);
  bool has_imm_left_rec (Node lhs, vector<Node>);
  string name_dash ();
  void add_dashes_to_grammar (vector<Node>);
  vector<string> nodes_to_name (vector<Node>);
  Node *start_node;
  map<string, int> col_names;
public:
  LL_Grammar ();
  Node get_start_node ();
  void print_gramm_as_nodes ();
  map<string, int> get_col_names ();
  map<Node, vector<Node> > get_grammar ();
  //returns the map after eliminating left factoring and left recursion
  map<Node, vector<Node> > get_clean_grammar ();
  void build_prods_as_nodes (); //fills the first version of the grammar map

};

#endif // LL_Grammar_H
