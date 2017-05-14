#include "Computation.h"

Computation::Computation (map<Node, std::vector<Node> >& components, Node start)
{
  this->components = components;
  this->start_node = &start;
}

map<Node, vector<pair <set<string>, Node > > >
Computation::get_map_of_first ()
{
    return map_of_first;
}

map<Node, set<string> >
Computation::get_map_of_follow ()
{
  return map_of_follow;
}

void
Computation::print_follow ()
{
  cout << "Printing the follow set" << endl;

  for (auto it = map_of_follow.begin (); it != map_of_follow.end (); it++)
  {
    cout << "=========================================================" << endl;
    Node cur_node = it->first;
    set<string> follow_set = it->second;
    cout << "Rule" << endl;
    cout << cur_node.get_name () << endl;
    cout << "Terminals " << endl;
    for (auto it2 = follow_set.begin (); it2 != follow_set.end (); it2++)
    {
      cout << *it2 << " ";
    }
    cout << endl;
    cout << "=========================================================" << endl;
  }
  cout << "End printing the follow set" << endl;
}

void
Computation::print_first ()
{
    cout << "=========================================================" << endl;
    cout << "Printing the First sets" << endl;
    for (auto it = map_of_first.begin (); it != map_of_first.end (); it++)
    {
      Node cur_node = it->first;
      vector<pair <set<string>, Node > > vec = it->second;
      cout << "*******************" << endl;
      cout << "Rule: " << cur_node.get_name () << endl;
      for (int i = 0; i < vec.size (); i++)
      {
        set<string> temp_set = vec[i].first;
        cout << "Terminals: ";
        for (auto it2 = temp_set.begin (); it2 != temp_set.end (); it2++)
        {
          cout << *it2 << " ";
        }
        cout <<  endl;
        cout << "Production: " << vec[i].second.get_name () << endl;
      }
      cout << "*******************" << endl;
    }
    cout << "=========================================================" << endl;
}

// build first and put them in map_of_first
void
Computation::construct_first ()
{
  // iterate over symbols
  for (auto it = components.begin(); it != components.end(); it++)
  {
    Node cur_node = it->first;
    vector<Node> cur_node_elems = it->second;

    //cout << "The current production is " <<  cur_node.get_name () << endl;
    vector<pair <set<string>, Node > > accumulated_ans;
    // iterate over the production of the current symbol
    for (int i = 0; i < cur_node_elems.size(); i++)
    {
      set<string> first_of_cur_node;
      vector<Node> cur_prod = cur_node_elems[i].get_components();

      bool finish = false;
      int index = 0;
      // for every production, update the accumulated_ans with the new terminals
      // and their production rule
      while (!finish && index < cur_prod.size ())
      {
        finish = true;
        set<string> ans = get_terminals (cur_prod[index]);
        for (auto it1 = ans.begin(); it1 != ans.end(); it1++)
        {
          string temp = *it1;
          if (temp.compare(epslon) == 0)
          {
            finish = false;

            if (index + 1 != cur_prod.size ())
              continue;

          }
          first_of_cur_node.insert (temp);
        }
        index ++;
      }
      accumulated_ans.push_back (make_pair (first_of_cur_node
        , cur_node_elems[i]));
    }
    map_of_first.insert (make_pair (cur_node, accumulated_ans));
  }
}


set<string>
Computation::get_terminals (Node cur_prod)
{
  stack <Node> s;
  set<string> ans;
  s.push (cur_prod);

  while (!s.empty ())
  {
    Node cur_node = s.top();
    s.pop();
    if (cur_node.is_terminal ())
    {
      ans.insert (cur_node.get_name());
    } else {
      vector<Node> next_node_production = components.at (cur_node);
      for (int i = 0; i < next_node_production.size (); i++)
      {
        s.push (next_node_production[i].get_components()[0]);
      }
    }
  }
  return ans;
}

// hash the right production to their symbols in the production map
void
Computation::hash_production_rules ()
{
  cout << "Enter Hash production" << endl;
  //iterate over the symbols
  for (auto it = components.begin(); it != components.end() ; it++)
  {
    Node cur_node = it->first;
    vector<Node> cur_node_elems = it->second;
    // for every symbol iterate over its productions
    for (int it1 = 0; it1 < cur_node_elems.size(); it1++)
    {
      vector<Node> cur_prod = cur_node_elems[it1].get_components();

      // for every production iterate over its components
      for (int it2 = 0; it2 < cur_prod.size(); it2++)
      {
        Node cur_prod_node = cur_prod[it2];
        if (cur_prod_node.is_terminal ())
          continue;
        // search the production_map for the cur component of the cur production
        // and if found get the vector and add the current production
        // else add the current production to an empty vector

        if (production_map.find (cur_prod_node) != production_map.end ())
        {
            vector<pair<Node, pair<Node, int> >  > temp =
                production_map.at (cur_prod_node);
            temp.push_back (make_pair (cur_node_elems[it1],
                make_pair (cur_node, it2)));
            production_map.erase (cur_prod_node);
            production_map.insert (make_pair (cur_prod_node, temp));
        }
        else
        {
            vector<pair<Node, pair<Node, int> > > temp;
            temp.push_back (make_pair (cur_node_elems[it1],
                make_pair (cur_node, it2)));
            production_map.insert (make_pair (cur_prod_node, temp));
        }
      }
    }
  }

}

void
Computation::print_hash_production_rules ()
{
  cout << "Printing the production map" << endl;
  for (auto it = production_map.begin (); it != production_map.end (); it++)
  {
    Node cur_node = it->first;
    vector<pair<Node, pair<Node, int> >  > cur_node_prods = it->second;
    cout << "Rule " << cur_node.get_name () << endl;
    for (int i = 0; i < cur_node_prods.size (); i++)
    {
      vector<Node> cur_node_prod = cur_node_prods[i].first.get_components ();
      Node prod_rule = cur_node_prods[i].second.first;
      int prod_index = cur_node_prods[i].second.second;
      cout << "cur hash name: " << cur_node_prods[i].first.get_name ()
      << " -> " << prod_rule.get_name () << " -> " << prod_index << endl;
      cout << "components: ";
      for (int j = 0; j < cur_node_prod.size (); j++)
      {
        cout << cur_node_prod[j].get_name () << " ";
      }
      cout << endl;
    }
    cout << "===================================================" << endl;
  }
  cout << "End printing the production map" << endl;

}

void
Computation::construct_follow()
{
  hash_production_rules ();
  //print_hash_production_rules ();

  cout << "Building follow set" << endl;
  // build the order of the iteration
  vector<Node> order_of_iterate = build_order_for_follow ();

  // this bool is used to repeat this procedure until there is no change in any
  // of the follow sets
  bool yarab_ne5ls = true;
  // this val is used to force repeat the iteration for the first time
  int val = 0;
  while (yarab_ne5ls)
  {

    yarab_ne5ls = false;
    // iterate over symbols with the start as first node
    for (int it = 0; it < order_of_iterate.size(); it++)
    {

      set<string> follow_of_cur_node;
      Node cur_node = order_of_iterate[it];

      // add $ to the follow set of the start symbol
      if (it == 0)
        follow_of_cur_node.insert("$");

      // a node with no occurences in right side
      if (production_map.find (cur_node) == production_map.end ()) {
        map_of_follow.insert (make_pair (cur_node, follow_of_cur_node));
        continue;
      }

      vector<pair<Node, pair<Node, int> > > occurences_in_right =
          production_map.at (cur_node);


      // iterate over all the occurences of the current symbol in right side
      // using the production_map produced by hash_production_rules function
      // called at the beginning of this function
      for (int it2 = 0; it2 < occurences_in_right.size(); it2++)
      {

        //map <Node, vector<pair<Node, pair<Node, int> > > > production_map;
        // the symbol of the according production
        // ex: E -> XT cur_node_of_cur_occurence = E
        // vec_of_cur_occurence = XT
        // index of the cur_prod in the production rule
        vector<Node> vec_of_cur_occurence =
            occurences_in_right[it2].first.get_components ();
        Node cur_node_of_cur_occurence = occurences_in_right[it2].second.first;
        int index_in_prod = occurences_in_right[it2].second.second;

        // bool used to determine if we are going to inspect the next symbol
        // in the current production or not based on the existence of an epslon
        // in the first set of the cur symbol of the cur production
        bool finish = false;
        for (int it3 = index_in_prod + 1; it3 < vec_of_cur_occurence.size(); it3++)
        {
            if (vec_of_cur_occurence[it3].is_terminal ()) {
              follow_of_cur_node.insert (vec_of_cur_occurence[it3].get_name ());
              finish = true;
              break;
            }
            vector<pair <set<string>, Node > > cur_first =
                map_of_first.at (vec_of_cur_occurence[it3]);
            finish = true;
            // add the first of the cur symbol of the cur production to the
            // follow of the cur node
            for (int i = 0; i < cur_first.size(); i++)
            {
              set<string> cur_first_elems = cur_first[i].first;

              for (auto it4 = cur_first_elems.begin();
                    it4 != cur_first_elems.end(); it4++)
              {
                // if the current node of cur production is epslon marks the
                // finish bool and continues without adding it to the follow set
                if (*it4 == epslon)
                {
                  finish = false;
                  continue;
                }
                follow_of_cur_node.insert (*it4);
              }
            }
            if (finish)
              break;
        }
        // if last symbol in cur production contains an epslon in its first
        // put the follow of the cur prod to the follow of the current node
        // ex: A -> xB || xBT and T contains epslon in its first set
        // so put the follow of A in the follow of B
        if (!finish)
        {
          if (map_of_follow.find (cur_node_of_cur_occurence)
                != map_of_follow.end())
          {
            set<string> map_of_follow_elems =
                map_of_follow.at (cur_node_of_cur_occurence);
            for (auto it5 = map_of_follow_elems.begin();
                  it5 != map_of_follow_elems.end(); it5++)
            {
              // should not happen
              if (*it5 == epslon)
                cout << "WEIRD" << endl;
              follow_of_cur_node.insert (*it5);
            }
          }
        }
      }

      // if the follow already exists from old iteration update the map and
      // marks the flag if there is an update to repeat the loop
      if (map_of_follow.find (cur_node) != map_of_follow.end())
      {
        set<string> temp_follow = map_of_follow.at (cur_node);
        int follow_set_size = temp_follow.size();

        for (auto it6 = temp_follow.begin(); it6 != temp_follow.end(); it6++)
          follow_of_cur_node.insert (*it6);

        if (follow_set_size != follow_of_cur_node.size())
          yarab_ne5ls = true;
        map_of_follow.erase (cur_node);
      }
      map_of_follow.insert (make_pair (cur_node, follow_of_cur_node));

    }
    // force the repeation of this loop for the first time
    if (val == 0)
      yarab_ne5ls = true;
    val++;
  }
}

// build a vector to make the start the first node which should be seted
// from part 1
vector<Node>
Computation::build_order_for_follow ()
{
  vector<Node> return_vec;
  return_vec.push_back (*start_node);
  for (auto it = components.begin(); it != components.end(); it++)
  {
    Node temp = it->first;
    if (temp != *start_node)
      return_vec.push_back (temp);
  }
  return return_vec;
}
