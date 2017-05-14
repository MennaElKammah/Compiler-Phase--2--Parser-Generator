#include "LL_Grammar.h"
#define EPS "eps" // epsilon

unsigned int LL_Grammar::dash_node_num = 0;

LL_Grammar::LL_Grammar () {
  this->prod_itr = new Prod_Itr ();
}

void LL_Grammar::print_gramm_as_nodes () {
  cout << "******Printing grammar map*******" << endl;
  typedef map<Node, vector<Node> >::iterator it_type;
  for(it_type itr = grammar.begin(); itr != grammar.end(); itr++) {
    Node temp_node = itr->first;
    string prod_name = temp_node.get_name();
    cout << prod_name << endl;
    vector<Node> prod = itr->second;

    for (int i = 0; i < prod.size (); ++i) {
      if (!prod[i].is_atomic ()) { // print elems of composite node
        vector<Node> comp = prod[i].get_components ();
        cout << "Composite: "<<prod[i].get_name () << endl;
        cout << "printing components" << endl;
        for (int j = 0; j < comp.size (); ++j) {
         cout << comp[j].get_name () << endl;
        }
        cout << "end printing components" << endl;
        // cout << "END HElloooo" << endl;
        // cout<< "\n";
      }
      else {
        cout << "Atomic: " << prod[i].get_name () << endl;
        vector<Node> comp = prod[i].get_components ();
        cout << "printing components" << endl;
        for (int j = 0; j < comp.size (); ++j) {
         cout << comp[j].get_name () << endl;
        }
        cout << "end printing components" << endl;
      }
    }
    cout << "====================" << endl;
  }
}

// return the start node
Node LL_Grammar::get_start_node () {
    return *start_node;
}

map<string, int>
LL_Grammar::get_col_names ()
{
    return col_names;
}

void LL_Grammar::build_prods_as_nodes () {
  while (prod_itr->has_next ()) {
    Prod *curr_prod = prod_itr->get_next ();
    string prod_name = curr_prod->get_key ();
    vector< vector<string> > prod_rhs = curr_prod->get_values ();

    // note that lhs production node is atomic, but not terminal, by default
    Node *new_prod = new Node (true, false, prod_name);

    // set the start node
    if (prod_name.compare ("METHOD_BODY") == 0)
      start_node = new_prod;

    // for each string make a node, and add it to the rhs_nodes
    vector<Node> rhs_nodes;
    for (vector<vector<string> >::size_type i = 0; i < prod_rhs.size (); ++i) {
      // for example if we have A = Ab | Cb | E in the prod_rhs
      // Then betn_or_prods will have <A,b> in the first iter
      // then <C,b> in the second iter, and finally <E> in the last Iter
      vector<string> betn_or_prods = prod_rhs [i];
      // check if the current production is atomic, i.e one production

      bool atomic = (betn_or_prods.size () == 1)? true : false;
      // now make a new node which is a wrapper for each production between the or
      Node *new_entry;
      if (atomic) { // means only one production
        //cout << "I'm atomic: " << betn_or_prods[0] << endl;
        bool terminal = false;
        if (curr_prod->is_terminal (betn_or_prods[0])) {
          terminal = true;
          betn_or_prods[0] = curr_prod->remove_quotes (betn_or_prods[0]);
          col_names.insert (make_pair (betn_or_prods[0], col_names.size ()));
        }
        if (added_nodes.find (betn_or_prods[0]) != added_nodes.end ()) { // have already been created
          // add the new_entry node to the current lhs's procution vector
          rhs_nodes.push_back (added_nodes.find (betn_or_prods[0])->second);
        } else {
          new_entry = new Node (atomic, terminal, betn_or_prods[0]);
          // add the new node to the added_nodes map
          added_nodes.insert (pair<string, Node> (betn_or_prods[0], *new_entry));
          //////////////////////////////////////////////////////////////////////
          // should be checked ???????????
          new_entry->add_component (*new_entry);
          //////////////////////////////////////////////////////////////////////
          // add the new_entry node to the current lhs's procution vector
          rhs_nodes.push_back (*new_entry);
        }
      }
      else { // composite node, i.e several productions concated like "Abd"
        // the new_entry node name is the concatination of all the names forming it

        //first remove quotes from terminals to make the concatenated name without quotes
        vector<string> without_quotes;
        for (int k = 0; k < betn_or_prods.size (); ++k) {
          if (curr_prod->is_terminal (betn_or_prods[k])) {
            without_quotes.push_back (curr_prod->remove_quotes (betn_or_prods[k]));
            col_names.insert (make_pair (
              curr_prod->remove_quotes (betn_or_prods[k]), col_names.size ()));
          }
          else {
            without_quotes.push_back (betn_or_prods[k]);
          }
        }

        string composite_name = concate_strings (without_quotes);
        // check node already have been created
        if (added_nodes.find (composite_name) != added_nodes.end ()) {
          // add the new_entry node to the current lhs's procution vector
          rhs_nodes.push_back (added_nodes.find (composite_name)->second);
        } else {
          new_entry = new Node (false, false, composite_name);
          for (vector<string>::size_type j = 0; j < betn_or_prods.size (); ++j) {
            Node *composite; // Note that atomic is false in all cases
            if (curr_prod->is_terminal (betn_or_prods[j])) {
              composite = new Node (false, true, curr_prod->remove_quotes (betn_or_prods[j]));
            } else {
              composite = new Node (false, false, betn_or_prods[j]);
            }
            new_entry->add_component (*composite);
          }
          // add the new_entry node to the current lhs's procution vector
          rhs_nodes.push_back (*new_entry);
          // add the new node to the added_nodes map
          added_nodes.insert (pair<string, Node> (composite_name, *new_entry));
        }
      }
    }
    // add the key and the value of the current production to the grammar map
    grammar.insert (pair<Node, vector<Node> >(*new_prod, rhs_nodes));
  }
}

map<Node, vector<Node> > LL_Grammar::get_clean_grammar () {
  int i = 0; // map indexing
  typedef map<Node, vector<Node> >::iterator it_type;
  vector<Node> dashes;
  for(it_type itr = grammar.begin(); itr != grammar.end(); ) {
    map<Node, vector<Node> >::iterator this_itr = itr++;
    Node lhs = this_itr->first;
    vector<Node> rhs_terms = this_itr->second;
    i++;

    // start substituting every other previous production to the current one,
    //  with it's values, to the current production
    int j = 0;
    for(it_type itr2 = grammar.begin(); itr2 != grammar.end(); itr2++) {
      if (j == i - 1)
        break;

      Node prev_prod_lhs = itr2->first;
      string prev_prod_lhs_name = prev_prod_lhs.get_name ();
      vector<Node> prev_prod_rhs = itr2->second;

      // iterate over the current production rhs, each time the prev_prod name
      // is found, substitute with its value.
      for (vector<Node>::size_type k = 0; k < rhs_terms.size (); ++k) {
        string term_1st_prod_name = split (
          rhs_terms[k].get_name (), regex ("\\."))[0];

        if (prev_prod_lhs_name.compare (term_1st_prod_name) == 0) {
          //cout << "replace term: " << rhs_terms[k].get_name () << endl;
          //cout << "with: " << prev_prod_lhs_name << endl;
          vector<Node> new_rhs_terms = substitute (prev_prod_rhs, rhs_terms[k]);
          // edit rhs_terms
          rhs_terms.erase (rhs_terms.begin () + k);
          grammar.erase (this_itr);
          // add the new terms reuslted from the substitution to the original rhs_terms
          for (int l = 0; l < new_rhs_terms.size (); ++l) {
            rhs_terms.push_back (new_rhs_terms[l]);
          }
          grammar.insert (pair<Node, vector<Node>> (lhs, rhs_terms));
        }
      }
      j++;
    }
    // eliminate_left_fact (lhs, grammar.find (lhs).second);
    if (has_imm_left_rec (lhs, rhs_terms)) {
      // cout<< "left recurion found"<<endl;
      dashes.push_back (eliminate_imm_left_rec (lhs, grammar.find (lhs)->second));
    }
  }
  // add dashes after finishing the left recursion ??
  add_dashes_to_grammar (dashes);
  dashes.clear ();
  // Eliminate Left factoring for all prods
  bool finish = false;
  while (!finish) {
    for (it_type itr = grammar.begin(); itr != grammar.end();) {
      map<Node, vector<Node> >::iterator this_itr = itr++;
      Node lhs = this_itr->first;
      vector<Node> rhs_terms = this_itr->second;
      vector<Node> new_dashes = eliminate_left_fact (lhs, rhs_terms, this_itr);
      if (new_dashes.size () == 0)
        finish = true;
      for (int i = 0; i < new_dashes.size (); ++i) {
        dashes.push_back (new_dashes[i]);
      }
    }
    add_dashes_to_grammar (dashes);
    dashes.clear ();
  }

  return grammar;
}

bool LL_Grammar::has_imm_left_rec (Node lhs, vector<Node> rhs) {
  string lhs_name = lhs.get_name ();
  for (vector<Node>::size_type i = 0; i < rhs.size (); ++i) {
    Node curr_term = rhs[i];
    string _1st_prod = split (curr_term.get_name (), regex ("\\."))[0];
    if (_1st_prod.compare (lhs_name) == 0) {
      return true;
    }
  }
  return false;
}

/* splits the given string on the given regex delimiter */
vector<string> LL_Grammar::split(string str, regex delim){
  sregex_token_iterator iter(str.begin(),str.end(),delim,-1);
  sregex_token_iterator end;
  vector<string> ans;
  int i = 0;
  for ( ; iter != end; ++iter){
    ans.push_back(*iter);
    ++i;
  }

  return ans;
}

/* Concats the vector of strings elements separated by "\\.", into single string */
string LL_Grammar::concate_strings (vector<string> strings) {
  string ans = strings[0];
  if (strings.size () == 1)
    return ans;

  for (vector<string>::size_type i = 1; i < strings.size (); ++i) {
    ans += "." + strings[i];
  }
  return ans;
}

// replaces the original term first production with the new_terms
// example new_terms = "Aa, b", orig_term = "Sd"
// result "Aad, bd"
vector<Node> LL_Grammar::substitute (vector<Node> new_terms, Node orig_term) {
  // erase the first node from the original term
  vector<Node> h = orig_term.get_components ();
  for (int i = 0; i < h.size(); ++i)
    cout << h[i].get_name () << endl;
  if (!h.empty())
    orig_term.remove_component (0);

  vector<string> orig_name_vec = split (orig_term.get_name (), regex ("\\."));
  //erase the first node name (the one we'll replace)
  //cout << "Original Name before erase: "<< orig_term.get_name () << endl;
  orig_name_vec.erase (orig_name_vec.begin ());
  string orig_name;
  if (orig_name_vec.empty ()) { // means it was an atomic node
    orig_name = "";
  }
  else {
    orig_name = concate_strings (orig_name_vec);
  }
  vector<Node> substituted_terms;
  // add each elem in new terms together with the ori_term, in the substituted_terms
  for (vector<Node>::size_type k = 0; k < new_terms.size (); ++k) {
    // insert the new production substituion names
    // to be before the original production names
    string new_node_name = new_terms[k].get_name ();
    if (orig_name.compare ("") != 0) {
      new_node_name += "." + orig_name;
    }
    // add the new terms production nodes
    Node *new_node;
    vector <Node> new_term_rhs = new_terms[k].get_components ();
    if (new_term_rhs.size () == 1 && orig_name.compare ("") != 0) { // atomic
      // check if it is terminal nodes
      if (new_term_rhs[0].is_terminal ()) {
        new_node = new Node (true, true, new_node_name); // terhminal
      }
      else {
        new_node = new Node (true, false, new_node_name); // non-terhminal
      }
    } else {
      new_node = new Node (false, false, new_node_name);
    }
    for (vector<Node>::size_type i = 0; i < new_term_rhs.size (); ++i) {
      new_node->add_component (new_term_rhs[i]);
    }
    // add the original terms production nodes except for the replaced one
    vector <Node> orig_term_rhs = orig_term.get_components ();
    for (vector<Node>::size_type i = 0; i < orig_term_rhs.size (); ++i) {
      new_node->add_component (orig_term_rhs[i]);
    }
    substituted_terms.push_back (*new_node);
  }
  return substituted_terms;
}

// eliminates immediare left recursion for the given production
// for example if lhs = A and rhs_terms = <A_b, A_c, d, f>
// then the result will change rhs_terms to <d_dash1, f_dash1>
// and create a new node dash1 -> <b_dash1,c_dash_1, eps> and add it to grammar map
Node LL_Grammar::eliminate_imm_left_rec (Node lhs, vector<Node> rhs_terms) {
  vector<Node> alpha;
  vector<Node> beta;
  string lhs_name = lhs.get_name ();
  string dash_node_name = name_dash ();
  // dash_node is a new non-terminal, hence, atomic
  Node *dash_node = new Node (true, false, dash_node_name);
  bool eps_added = false;
  // get the alpha and beta terms from the given rhs_productions
  for (vector<Node>::size_type i = 0; i < rhs_terms.size (); ++i) {
    // erase any thing that begins with lhs name
    // add the erased elems (after erasing lhs) to the alpha list
    vector<string> node_name = split (rhs_terms[i].get_name (), regex ("\\."));
    string rhs_1st_prod_name = node_name[0];
    if (lhs_name.compare (rhs_1st_prod_name) == 0) {
      Node alpha_term = rhs_terms[i];
      alpha_term.remove_component (0);
      node_name.erase (node_name.begin ());
      // reset the name after removing the recursion
      if (rhs_terms[i].is_atomic ()) { // means it was atomic
        eps_added = true;
        alpha_term.set_name (EPS);
        alpha_term.set_atomic (true);
        alpha_term.set_terminal (true);
        alpha_term.add_component (alpha_term);
      }
      else {
        alpha_term.set_name (concate_strings (node_name));
        alpha.push_back (alpha_term);
      }
    }
    else {
      beta.push_back (rhs_terms[i]);
    }
  }

  // iterate through alpha and add the dash_node
  // and add the alpha to the dash_node
  for (vector<Node>::size_type i = 0; i < alpha.size (); ++i) {
    if (alpha[i].get_name ().compare (EPS) != 0) {
      alpha[i].add_component (*dash_node);
      alpha[i].set_name (alpha[i].get_name ()+ "." + dash_node_name);
      dash_node->add_component (alpha[i]);
      alpha[i].set_atomic (false);
    }
  }
  // add epsilon to the dash node
  if (!eps_added) {
    //eps is atomic by default
    Node *eps = new Node (true, true, EPS);
    eps->add_component (*eps);
    dash_node->add_component (*eps);
  }
  // for each beta add the dash node
  for (vector<Node>::size_type i = 0; i < beta.size (); ++i) {
    beta[i].add_component (*dash_node);
    beta[i].set_name (beta[i].get_name () + "." + dash_node_name);
    beta[i].set_atomic (false);
  }
  // finally update the map value to hold the eliminated recursion production
  grammar.erase (lhs);
  added_nodes.erase (lhs.get_name ());
  // update the prod in grammar map
  grammar.insert (pair<Node, vector<Node> > (lhs, beta));
  // update the prod in added_nodes map
  added_nodes.insert (pair<string, Node> (lhs.get_name (), lhs));
  return *dash_node;
}

void LL_Grammar::add_dashes_to_grammar (vector<Node> dashes ) {
  for (int i = 0; i < dashes.size (); ++i) {
    // add the dash_node to the grammar
    grammar.insert (pair<Node, vector<Node> >(dashes[i], dashes[i].get_components ()));
    // add the dash_node to the added_nodes
    added_nodes.insert (pair<string, Node> (dashes[i].get_name (), dashes[i]));
  }
}

string LL_Grammar::name_dash () {
  std::stringstream ss;
  ss << dash_node_num;
  dash_node_num++;
  return std::string("DASH") + ss.str();
}

vector<Node> LL_Grammar::eliminate_left_fact (Node lhs, vector<Node> rhs_terms,
  map<Node, vector<Node> >::iterator this_itr) {
  vector<Node> dashes;
  // this pair holds the name of the longest common prefix (i.e alpha)
  // and the positions in which this alpha was found in the rhs_terms
  pair <string, vector<int> > alpha_name_pos = find_common (nodes_to_name (rhs_terms));
  // while there's lcp, then continue eliminating
  while (alpha_name_pos.first.compare ("") != 0) {
    //print_gramm_as_nodes ();
    vector<int> alpha_positions = alpha_name_pos.second;
    string alpha_name = alpha_name_pos.first;
    // vector<string> alpha_name_splitted = split (alpha_name, regex ("\\."));
    // int alpha_size = alpha_name_splitted.size ();
    Node *dash_node = new Node (false, false, name_dash ());
    // remove alpha terms from rhs_terms
    // put alpha_terms in a new dash_node without the "alpha"

    for (int i = 0; i < alpha_positions.size (); ++i)
    {
      Node without_alpha_term = rhs_terms[alpha_positions[i]];
      string without_alpha_name =
       without_alpha_term.get_name ().erase (0, alpha_name.size ());
      if (without_alpha_name.compare ("") == 0) { // after alpha term is epsilon
        without_alpha_term.set_name (EPS);
        without_alpha_term.set_atomic (true);
        without_alpha_term.set_terminal (true);
        without_alpha_term.add_component (without_alpha_term);
      } else {
        without_alpha_term.set_name (without_alpha_name.erase (0,1)); // erase the "."
      }
      without_alpha_term.remove_component (0);
      dash_node->add_component (without_alpha_term);
    }
    // remove alpha nodes from rhs_terms
    for (int i = alpha_positions.size () - 1; i >= 0; --i) {
      rhs_terms.erase (rhs_terms.begin () + alpha_positions[i]); // will the value in the alpha_terms be erased???? :/
    }
    // build the alpha node to add it with the dash_node to the rhs_terms
    Node *alpha_node = new Node (false, false, alpha_name);
    Node *alpha_dash = new Node (false, false, alpha_name + "." + dash_node->get_name ());
    alpha_dash->add_component (*alpha_node);
    alpha_dash->add_component (*dash_node);
    added_nodes.insert (pair<string, Node> (alpha_dash->get_name (), *alpha_dash));
    // added_nodes.insert (pair<string, Node> (dash_node->get_name (), *dash_node));
    rhs_terms.push_back (*alpha_dash);
    dashes.push_back (*dash_node);
    grammar.erase (this_itr);
    this_itr =
        (grammar.insert (pair<Node, vector<Node> > (lhs, rhs_terms))).first;

    // check if still we have common factors
    alpha_name_pos = find_common (nodes_to_name (rhs_terms));
  }
  return dashes;
}

vector<string> LL_Grammar::nodes_to_name (vector<Node> nodes) {
  vector<string> names;
  // get the name of each node in the rhs
  for (vector<Node>::size_type i = 0; i < nodes.size (); ++i) {
    names.push_back (nodes[i].get_name ());
  }
  return names;
}

// Finds the first common prefix in the given vector<string>
// returns the lcp with the positions in the vector, in which this prefix existed
pair <string, vector<int> > LL_Grammar::find_common (vector<string> strings) {
    vector<int> positions;
    vector<string> firsts;
    // get first string of every rhs term
    for (int i = 0; i < strings.size (); ++i) {
      firsts.push_back (split (strings[i], regex ("\\."))[0]);
    }
    for (int i = 0; i < firsts.size (); ++i) {
      string str = firsts[i];
      bool found_common = false;
      for (int j = i + 1; j < firsts.size (); ++j) {
        if (str.compare (firsts[j]) == 0) { // found common
          positions.push_back (j);
          found_common = true;
        }
      }
      if (found_common) {
        positions.insert (positions.begin (), i);
        return pair <string, vector<int> > (firsts[i], positions);
      }
    }
    return pair <string, vector<int> > ("", positions);
}

map<Node, vector<Node> > LL_Grammar::get_grammar () {
  return grammar;
}
