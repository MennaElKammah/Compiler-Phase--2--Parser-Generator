#include "Table.h"

Table::Table (map<string, int > col_names, map<Node, vector<Node> > grammar,
  map<Node, vector<pair <set<string>, Node > > > map_of_first,
  map<Node, set<string> > map_of_follow)
{
  this->epslon_node->add_component (*epslon_node);
  this->error_node->add_component (*error_node);
  this->sync_node->add_component (*sync_node);
  col_names.insert (make_pair ("$", col_names.size ()));
  this->col_names = col_names;
  this->construct_prod(grammar);
  this->rows = this->row_names.size();
  this->cols = this->col_names.size();
  this->initialize_table ();
  this->map_of_first = map_of_first;
  this->map_of_follow = map_of_follow;
  this->set_table ();
}

vector< vector<Node> >
Table::get_table ()
{
  return table;
}

void
Table::construct_prod(map<Node, vector<Node> > grammar) {
  int index = 0;
  for (auto it = grammar.begin(); it != grammar.end(); it++)
  {
    Node temp_node = it->first;
    this->row_names.insert(make_pair(temp_node.get_name(), index++));
  }
}

void
Table::set_node(string row, string col, Node value) {
  this->table[this->row_names[row]][this->col_names[col]] = value;
}

Node
Table::get_node(string row, string col) {
  if (this->row_names.find (row) != this->row_names.end () &&
      this->col_names.find (col) != this->col_names.end ())
  return this->table[this->row_names[row]][this->col_names[col]];
  else
  return *(this->error_node);
}

void
Table::initialize_table() {
  // initialize table with all entries "error"
  for (int i = 0; i < rows; i++)
  {
    vector<Node> temp_vec;
    for (int j = 0; j < cols; j++)
    {
      // error_node is a node of name error
      temp_vec.push_back (*error_node);
    }
    table.push_back (temp_vec);
  }
}

void
Table::print_table ()
{
  ofstream output_stream("table.txt");
   if (output_stream.is_open ()) {
      output_stream << "******Printing Table******" << endl;
      output_stream << setw(60) << left << "\t\t";
      for (auto it = col_names.begin (); it != col_names.end (); it++)
      {
        output_stream << setw(60) << left << it->first << "   ";
      }
      output_stream << endl;
      for (auto it = row_names.begin (); it != row_names.end (); it++)
      {
        output_stream << setw(60) << left << it->first << "   ";
        for (auto it2 = col_names.begin (); it2 != col_names.end (); it2++)
        {
          output_stream << setw(60) << left << this->get_node (it->first, it2->first).get_name () << "   ";
        }
        output_stream << endl;
      }
      output_stream << "******End printing Table******" << endl;
  }
  output_stream.close ();
}

void
Table::set_table() {
  // map<Node, vector<pair <set<string>, vector<Node> > > > map_of_first;
  // map<Node, set<string> > map_of_follow;
  for (auto it = map_of_first.begin(); it != map_of_first.end(); it++)
  {
    Node cur_node = it->first;
    string row_name = cur_node.get_name();
    vector<pair <set<string>, Node > > cur_row_first = it->second;
    for (int it2 = 0; it2 < cur_row_first.size(); it2++)
    {

      pair <set<string>, Node > cur_terminals = cur_row_first[it2];
      set<string> terminals = cur_terminals.first;
      Node cur_prod_node = cur_terminals.second;
      for (auto it3 = terminals.begin (); it3 != terminals.end (); it3++)
      {
        string terminal = *it3;
        if (terminal.compare(epslon) != 0)
          this->set_node(row_name, terminal, cur_prod_node);
        else
        {
          //find entry of follow of current row (it->first) in the map_of_follow
          set<string> followers = map_of_follow.at (cur_node);

          for (auto it4 = followers.begin (); it4 != followers.end (); it4++)
          {
            string cur_follower = *it4;
            if (this->get_node(row_name,
              cur_follower).get_name().compare((*error_node).get_name()) == 0)
              this->set_node(row_name, cur_follower, *epslon_node);
            else {
              cout << "Error! Entering a follower in a non-empty cell" << endl;
            }
          }
        }
      }
    }
    // find follow entry of this current row (it->first)
    set<string> followers = map_of_follow.at (cur_node);
    for (auto it5 = followers.begin (); it5 != followers.end (); it5++)
    {
      string follower = *it5;
      if (this->get_node(row_name,
        follower).get_name().compare((*error_node).get_name()) == 0)
        this->set_node(row_name, follower, *sync_node);
    }
  }
}
