#include "Parser.h"

Parser::Parser ()
{
  construct_op_prior();
  construct_input ();
  state_name = 0;
  acc_prior = 1;
}

std::vector<std::string>
Parser::get_tokens ()
{
  return out_tokens;
}

/*
  Main flow of the code
*/
void
Parser::parse (void)
{
  // open file
  std::ifstream regex_stream("regex.txt");
  std::string line;
  if (regex_stream.is_open ()) {
    while (std::getline(regex_stream, line)) {
      std::vector<std::string> tokens;
      //std::cout << line << std::endl;

      split_line (tokens, line);
      insert_elem_in_acceptance_state (tokens);
      evaluate_exp (tokens);
    }
    insert_acceptance_state ("-", acc_prior++);
    std::vector<std::vector <std::string> > transition_table;
    std::map<std::string, int> name_to_index;
    build_dfa (Nfa::multiple_or (components)->Start,
        transition_table, name_to_index);
    //print_acceptance_state ();
    regex_stream.close();
    std::cout << "Finish Regex" << std::endl;
    //print_acceptance_state ();
    parse_input_file (transition_table, name_to_index);
    std::cout << "Finish Input" << std::endl;
  } else {
    std::cout << "Fail to open regex file" << std::endl;
  }
}

/*
  Parse the input file
*/
void
Parser::parse_input_file (
  std::vector<std::vector <std::string> > &transition_table,
  std::map<std::string, int> &name_to_index)
{
  std::ifstream inp_stream("in.txt");
  std::ofstream out_stream("out.txt");
  std::string line;
  if (inp_stream.is_open () && out_stream.is_open ()) {
      while (std::getline(inp_stream, line)) {
          std::vector<std::string> tokens = split_on_whitespaces (line);
          for (int i = 0; i < tokens.size (); i++) {
            std::vector<std::string> types =
                check_type (transition_table, tokens[i], name_to_index);

            for (int j = 0; j < types.size (); j++) {
              out_stream << types[j] << std::endl;
              out_tokens.push_back (types[j]);
            }
          }
      }
  } else {
    std::cout << "Fail to open input file" << std::endl;
  }
}

std::vector<std::string>
Parser::check_type (
  std::vector<std::vector <std::string> > &transition_table, std::string token,
  std::map<std::string, int> &name_to_index)
{
  std::vector<std::string> types_so_far;
  while (true) {
    int last_accept_index = -1;
    std::string last_accept_type;
    int transition_table_indI = 0;
    int transition_table_indJ = 0;
    int index = 0;
    int last_match = -1;
    int start_index = 0;
    while (index < token.size()) {

      if (input_index.find (token[index]) == input_index.end ())
      {
         if (last_accept_index != -1) {
            types_so_far.push_back (last_accept_type);
            if (last_accept_type == "id") {
              std::cout << "id " << token.substr(start_index,
                last_accept_index - start_index + 1) << std::endl;
              symbol_table.insert (token.substr(start_index,
                last_accept_index - start_index + 1));
            }
            start_index = last_accept_index + 1;
            index = last_accept_index;
            last_accept_index = -1;
         } else {
          types_so_far.push_back ("error");
          if (last_match != -1)
          	index = start_index;
          start_index = index + 1;
         }
         last_match = -1;
         transition_table_indI = 0;
      } else {
        transition_table_indJ = input_index.at (token[index]);
        std::string temp =
          transition_table[transition_table_indI][transition_table_indJ];
        transition_table_indI = name_to_index.at (temp);

        if (transition_table_indI != transition_table.size () - 1) {
          std::string temp_state =  clean_type (acceptance_to_expr.at (temp));
          if (temp_state != "-")  {
            last_accept_index = index;
            last_accept_type = temp_state;
          }
          last_match = index;
        } else {
          if (last_accept_index != -1) {
            types_so_far.push_back (last_accept_type);
            if (last_accept_type == "id") {
              std::cout << "id " << token.substr(start_index,
                last_accept_index - start_index + 1) << std::endl;
              symbol_table.insert (token.substr(start_index,
                last_accept_index - start_index + 1));
            }
            start_index = last_accept_index + 1;
            index = last_accept_index;
            last_accept_index = -1;
          } else {
            types_so_far.push_back ("error");
            if (last_match != -1)
          		index = start_index;
            start_index = index + 1;
          }
          last_match = -1;
          transition_table_indI = 0;
        }
      }
      index++;
    }

    if (last_accept_index != -1) {
    	types_so_far.push_back (last_accept_type);
      if (last_accept_type == "id") {
        std::cout << "id " << token.substr(start_index,
          last_accept_index - start_index + 1) << std::endl;
        symbol_table.insert (token.substr(start_index,
          last_accept_index - start_index + 1));
      }
      if ((last_accept_index + 1) >= token.size())
      	break;
    	token = token.substr(last_accept_index + 1,
        token.size() - (last_accept_index + 1) + 1);
    } else
    	break;

  }
  return types_so_far;
}

std::string
Parser::clean_type (std::string inp)
{
    std::string new_inp = "";
    for (int i = 0; i < inp.size (); i++) {
      if (inp[i] == '~') break;
      new_inp += inp[i];
    }
    return new_inp;
}

std::vector<std::string>
Parser::split_on_whitespaces (std::string line)
{
  istringstream iss(line);
  std::vector<std::string> tokens;
  copy(std::istream_iterator<std::string>(iss),
     std::istream_iterator<std::string>(),
     back_inserter(tokens));
  return tokens;
}

/*
  insert expression and keyword in a map according to their priority
*/
void
Parser::insert_elem_in_acceptance_state (std::vector<std::string> &tokens)
{
  // keyword
  if (tokens[0] == "{" || tokens[0] == "[") {
    for (int i = 1; i < tokens.size() - 1; i++)
      insert_acceptance_state (remove_backslash(tokens[i]), 0);
  }
  // expression
  else if (tokens[1] == ":")
    insert_acceptance_state (tokens[0], acc_prior++);
}

/*
  utility function insert in acceptance map
*/
void
Parser::insert_acceptance_state (std::string token, int priority)
{
    acceptance_priority.insert (make_pair (token, priority));
}

/*
  utility function print the splitted tokens
*/
void
Parser::print_tokens (std::vector<std::string> &tokens)
{
  std::cout << "The tokens :" << std::endl;
  for (int i = 0; i < tokens.size(); i++) {
    std::cout << "token is : ";
    std::cout << tokens[i] << std::endl;
  }
  std::cout << std::endl;
}

/*
  utility function print frist level of the nfa
*/
void
Parser::print(Nfa::Node * src)
{
  if (src == NULL || src->Next_trans.size() == 0)
  return;
  cout << src->Next_trans[0].second << endl;
  print(src->Next_trans[0].first);
}

/*
  utility function print the nfa using bfs
*/
void
Parser::print_levels(Nfa::Node* src)
{
  std::set <Nfa::Node*> vis;
  std::queue<pair<Nfa::Node*, int> > q;
  q.push (make_pair(src, 0));
  vis.insert (src);
  while (!q.empty()) {
    Nfa::Node * cur = q.front().first;
    int lev = q.front().second;
    q.pop();

    for (int i = 0; i < cur->Next_trans.size(); i++){
      std::pair<Nfa::Node*, char> p = cur->Next_trans[i];
      if (vis.find (p.first) == vis.end()) {
        vis.insert (p.first);
        q.push (make_pair(p.first, lev + 1));
      }
      std:cout << "level is " << lev << " " << p.second << " ";
    }
    std::cout << std::endl;
  }
}

/*
  remove all backslash in token
*/
std::string
Parser::remove_backslash (std::string token)
{
  ssize_t len;
  while ((len = token.find("\\")) != std::string::npos)
    token = token.replace(len, 1, "");
  return token;
}

/*
  split the regex line on delimeters
*/
void
Parser::split_line (std::vector<std::string> &tokens, std::string line)
{
  int start_index = 0, len = 0, i = 0;
  std::string token;

  for (; i < line.length(); i++) {
    if (line[i] == ' ' || line[i] == '|' || line[i] == ':' ||
    line[i] == '{' || line[i] == '}' || line[i] == ']' ||
    line[i] == '[' ||
    (line[i - 1] != '\\' && line[i] == '*') ||
    (line[i - 1] != '\\' && line[i] == '+') ||
    (line[i - 1] != '\\' && line[i] == '(') ||
    (line[i - 1] != '\\' && line[i] == ')') ||
    (line[i] == '-') ){

      // len of current token
      len = i - start_index;
      token = line.substr(start_index, len);
      start_index = i + 1;

      // string isn't empty nor space
      if (token != " " && token.length() > 0) {
        if (token == "\\L")
        token = "$";
        if ((start_index - len != 1) && (token == "(" ||
        token == "digits" || token == "."))
        tokens.push_back ("@");
        tokens.push_back (token);
      }
      token = line[i];

      // string isn't empty nor space
      if (token != " " && token.length() > 0) {
        if ((start_index - len != 1) && (token == "(" ||
        token == "digits" || token == "."))
        tokens.push_back ("@");
        tokens.push_back (token);
      }
    }
  }
  len = i - start_index;
  if (len > 0) {
    token = line.substr(start_index, len);
    tokens.push_back (token);
  }
  if (tokens[tokens.size() - 1] == "-" || tokens[tokens.size() - 1] == "/")
    tokens[tokens.size() - 1] = "\\" + tokens[tokens.size() - 1];
}

/*
  evaluate expression using two stacks according to operators priority
*/
void
Parser::evaluate_exp(vector<string> &exp)
{
  std::stack<Nfa> operands;
  std::stack<std::string> operators;
  if (exp[0] != "{" && exp[0] != "[")
  {
    std::string key = exp[0];
    bool is_exp = (exp[1] == ":");
    for (int i = 2; i < exp.size(); i++)
    {
      std::string curr_token = exp[i];
      Nfa *value = NULL;
      if (is_operator(curr_token))
      {
        while (!operators.empty() &&
        !has_higher_prior(curr_token, operators.top()))
        {
          std::string op = operators.top();
          operators.pop();
          Nfa *second_nfa = &operands.top();
          operands.pop();
          Nfa *first_nfa = NULL;
          if (is_binary(op))
          {
            first_nfa = &operands.top();
            operands.pop();
          }
          value = construct_NFA(first_nfa, second_nfa, op);
          operands.push(*value);
        }
        if (is_closed_paren(curr_token))
        operators.pop();
        else
        operators.push(curr_token);
      }
      else
      {
        if (cache.find(curr_token) != cache.end())
        operands.push(*(cache.at(curr_token).clone()));
        else
        operands.push(*(new Nfa(remove_backslash(curr_token))));
      }
    }
    while (!operators.empty())
    {
      std::string op = operators.top();
      operators.pop();
      Nfa *second_nfa = &operands.top();
      operands.pop();
      Nfa *first_nfa = NULL;
      if (is_binary(op))
      {
        first_nfa = &operands.top();
        operands.pop();
      }
      Nfa *value = construct_NFA(first_nfa, second_nfa, op);

      operands.push(*value);
    }
    if (is_exp) {
      components.insert(std::pair<std::string, Nfa> (key, operands.top()));
      operands.top().get_end ()->accept_state = exp[0];
    }
    else
    cache.insert(std::pair<std::string, Nfa> (key, operands.top()));

    operands.pop();
  }
  else
  {
    for (int i = 1; i < exp.size() - 1; i++)
    {
      std::string temp_str = remove_backslash (exp[i]);
      Nfa *temp_nfa = new Nfa (temp_str);
      components.insert(std::pair<std::string, Nfa> (temp_str, *temp_nfa));
      temp_nfa->get_end ()->accept_state = temp_str;
    }
  }
}

/*
  return if the token is operator or not
*/
bool
Parser::is_operator(std::string token)
{
  return (token == "@" || token == "|" || token == "-" ||
  token == "+" || token == "*" || token == "(" || token == ")");
}

/*
  return if operator is unary(0) or binary(1)
*/
bool
Parser::is_binary(std::string token)
{
  return (token == "@" || token == "|" || token == "-");
}

/*
  return if the token is )
*/
bool
Parser::is_closed_paren(std::string token)
{
  return (token == ")");
}

/*
  construct the operators priority
*/
void
Parser::construct_op_prior() {
  /* () * - + @ | */
  op_prior.insert(std::pair<std::string, int> ("|", 1));
  op_prior.insert(std::pair<std::string, int> ("@", 2));
  op_prior.insert(std::pair<std::string, int> ("+", 3));
  op_prior.insert(std::pair<std::string, int> ("-", 4));
  op_prior.insert(std::pair<std::string, int> ("*", 5));
}

/*
  compare between 2 operators and return the one with the higher priority
*/
bool
Parser::has_higher_prior(std::string first_op, std::string second_op)
{

  if (first_op == "(" || second_op == "(")
    return true;
  else if (first_op == ")")
    return false;
  else
    return op_prior[first_op] > op_prior[second_op];
}

/*
  call different function to construct the NFA according to op
*/
Nfa*
Parser::construct_NFA(Nfa *first_nfa, Nfa *second_nfa, std::string op)
{
  if (op == "*") {
    return Nfa::do_zero_more (second_nfa);
  }
  else if (op == "+") {
    return Nfa::do_one_more (second_nfa);
  }
  else if (op == "|") {
    return Nfa::do_or (first_nfa, second_nfa);
  }
  else if (op == "@") {
    return Nfa::do_concat (first_nfa, second_nfa);
  }
  else if (op == "-"){
    return build_period (first_nfa, second_nfa);
  }
  else {
    cout << "Error!" << endl;
  }
}

/*
  construct vector for period operator from start char to end char
*/
Nfa*
Parser::build_period (Nfa *first_nfa, Nfa *second_nfa)
{
  char start = first_nfa->Start->Next_trans[0].second;
  char end = second_nfa->Start->Next_trans[0].second;

  std::vector<char> inp;
  inp.push_back (start);

  while (start != end){
    start += 1;
    inp.push_back (start);
  }
  return Nfa::do_period (inp);
}

/*
  build the DFA of the given NFA
*/
void
Parser::build_dfa(Nfa::Node* src,
  std::vector<std::vector <std::string> > &transition_table,
  std::map<std::string, int> &name_to_index)
{
  /*
    Note that: equivalent nodes are gathered in one node with one name
    new node name = concated nodes numbers
   */
  std::queue<std::string> q;
  // map the result of concatenating the nodes_no to the nodes
  std::map<std::string, std::vector<Nfa::Node*> > str_to_nodes;
  // epslone of the src
  std::vector<Nfa::Node*> epslon;
  get_epslon_closure(src, epslon);
  std::string src_name = concatenate (epslon);
  q.push (src_name);
  str_to_nodes.insert (make_pair (src_name, epslon));
  name_to_index.insert (make_pair (src_name, 0));
  initialize_transition_table (transition_table, 0);
  map_name_to_output (src_name);

  while (!q.empty()) {
    std::string cur_str = q.front();
    q.pop();

    // new big node
    std::vector<Nfa::Node*> cur_nodes = str_to_nodes.at(cur_str);
    // map contains the input char and the corresponding set of nodes
    // wihtout there epslon
    std::map<char, std::set<Nfa::Node*> > inp;

    inspect_big_node_children (cur_nodes, inp);

    get_big_node_epslon (inp, transition_table,
        name_to_index, cur_str, str_to_nodes, q);
  }

  //print_acceptance_state ();

  add_fay_node (transition_table, name_to_index);

  minimize_dfa (transition_table, name_to_index);

  clean_transition_table (transition_table, name_to_index);

  print_transition_table (transition_table);
}


/*
  get epslon closure for the given node using dfs
*/
void
Parser::get_epslon_closure(Nfa::Node* src, std::vector<Nfa::Node* > &nodes)
{
  std::stack <Nfa::Node*> st;
  std::set<Nfa::Node*> visited;
  st.push(src);
  visited.insert(src);
  nodes.push_back(src);
  while (!st.empty()){
    Nfa::Node* temp = st.top();
    st.pop();
    for (int i = 0; i < temp->Next_trans.size(); i++)
    {
      if (temp->Next_trans[i].second == '$' &&
      !(visited.find(temp->Next_trans[i].first) != visited.end()))
      {
        st.push(temp->Next_trans[i].first);
        visited.insert(temp->Next_trans[i].first);
        nodes.push_back(temp->Next_trans[i].first);
      }
    }
  }
}

/*
  add fay node to different data structure
*/
void
Parser::add_fay_node (std::vector<std::vector <std::string> > &transition_table,
  std::map<std::string, int> &name_to_index)
{
  name_to_index.insert (make_pair ("-", acceptance_to_expr.size ()));
  initialize_transition_table (transition_table, acceptance_to_expr.size ());
  acceptance_to_expr.insert (make_pair ("-", "-"));
  map_name_to_output ("-");
}

/*
 iterate over the input char to node to get their epslon
*/
void
Parser::get_big_node_epslon (std::map<char, std::set<Nfa::Node*> > &inp,
  std::vector<std::vector <std::string> > &transition_table,
  std::map<std::string, int> &name_to_index, std::string cur_str,
  std::map<std::string, std::vector<Nfa::Node*> > &str_to_nodes,
  std::queue<std::string> &q)
{
  for (std::map<char, std::set<Nfa::Node*> >::iterator it = inp.begin();
  it != inp.end(); it++) {
      char cur_char = it->first;
      std::set<Nfa::Node*> cur_set_nodes = it->second;

      std::vector<Nfa::Node*> output_nodes;

      get_esplon_closure_for_input (output_nodes, cur_set_nodes);

      std::string child_name = concatenate (output_nodes);

      add_node_to_transition_table (transition_table, name_to_index,
      cur_str, child_name, cur_char);

      add_node_to_queue (q, str_to_nodes, child_name, output_nodes);
  }
}

/*
  add node to queue if not visited
*/
void
Parser::add_node_to_queue (std::queue<std::string> &q,
  std::map<std::string, std::vector<Nfa::Node*> > &str_to_nodes,
  std::string child_name, std::vector<Nfa::Node*> &output_nodes)
{
  if (str_to_nodes.find (child_name) == str_to_nodes.end()) {
    q.push (child_name);
    str_to_nodes.insert (make_pair (child_name, output_nodes));
  }
}
/*
  get epslon closure for the given input
*/
void
Parser::get_esplon_closure_for_input (std::vector<Nfa::Node*> &output_nodes,
  std::set<Nfa::Node*> &cur_set_nodes)
{
  for (std::set<Nfa::Node*>::iterator set_it = cur_set_nodes.begin();
      set_it != cur_set_nodes.end(); set_it++)
        get_epslon_closure (*set_it, output_nodes);
}

/*
  inspect the child of the given big node
*/
void
Parser::inspect_big_node_children (std::vector<Nfa::Node*> &cur_nodes,
    std::map<char, std::set<Nfa::Node*> > &inp)
{
  // iterate over node in current state
  for (int i = 0; i < cur_nodes.size(); i++) {
    inspect_children (cur_nodes[i], inp);
  }
}

/*
 iterate over transition of the current node and insert the transitions
 with the non-epslon value
*/
void
Parser::inspect_children (Nfa::Node* cur_node,
    std::map<char, std::set<Nfa::Node*> > &inp)
{
  for (int j = 0; j < cur_node->Next_trans.size(); j++) {
    pair<Nfa::Node*, char> child = cur_node->Next_trans[j];

    if (!is_transition_epslon (child.second))
      update_input_map (inp, child.second, child.first);
  }
}

/*
  update the child of the given input
*/
void
Parser::update_input_map (std::map<char, std::set<Nfa::Node*> > &inp,
    char transition, Nfa::Node* child)
{
  std::map<char, std::set<Nfa::Node*> >::iterator input
  = inp.find (transition);

  // new input
  if (input == inp.end()) {
    std::set<Nfa::Node*> trans_nodes;
    trans_nodes.insert (child);
    inp.insert (make_pair (transition, trans_nodes));
  }
  // update data for an existing input
  else {
    input->second.insert (child);
    inp.insert (make_pair (transition, input->second));
  }
}

/*
  check transition type
*/
bool
Parser::is_transition_epslon (char child)
{
  return child == '$';
}

/*
  map the hashed name of the node to S + digit
*/
void
Parser::map_name_to_output (std::string child_node_name)
{
  std::stringstream ss;
  ss << state_name;
  state_name++;
  std::string name = std::string("S") + ss.str();
  name_to_output.insert (make_pair (child_node_name, name));
}

/*
  utility function to print the transition table
*/
void
Parser::print_transition_table (
    std::vector<std::vector <std::string> > &transition_table)
{
  std::ofstream trans_tab ("trans_table.txt");
  if (trans_tab.is_open ()) {
    trans_tab << "\t\t*** transition table ***" << std::endl;

    for (int i = 0; i < transition_table.size(); i++) {
      if (index_to_state.find (i) != index_to_state.end()) continue;
      trans_tab << "S" << i << "\t";
      for (int j = 0; j < transition_table[i].size(); j++) {
        if (transition_table[i][j] == "-") {
          trans_tab << "-\t";
          continue;
        }
        trans_tab << name_to_output.at (transition_table[i][j]) << " ";
      }
      trans_tab << std::endl;
    }
    trans_tab.close();
  } else {
    std::cout << "Fail to write to file" << std::endl;
  }
}


void
Parser::clean_transition_table (
  std::vector<std::vector <std::string> > &transition_table,
  std::map<std::string, int> &name_to_index)
{
  index_to_state.clear();
  std::map<std::string,
      std::vector<std::string> > grouping_states;
  group_states (grouping_states);
  int x = 0;
  for (int i = 0; i < transition_table.size (); i++) {
    for (int j = 0; j < transition_table[i].size (); j++) {
      std::string cur_state = transition_table[i][j];
      std::vector<std::string> temp =
          grouping_states.at (acceptance_to_expr.at (cur_state));
      std::string group_represented_name = temp[0];
      if (cur_state != group_represented_name) {
        transition_table[i][j] = group_represented_name;
        index_to_state.insert (make_pair (name_to_index.at (cur_state), "~"));
      }

    }
  }

}

/*
  add node to differnet data structures
*/
void
Parser::add_node_to_transition_table (
    std::vector<std::vector <std::string> > &transition_table,
    std::map<std::string, int> &name_to_index, std::string cur_node_name,
    std::string child_node_name, char inp)
{
  int curent_node_index = name_to_index.at(cur_node_name);
  int inp_index = input_index.at(inp);

  // new node -> insert in map and add a new row in transition table
  if (name_to_index.find (child_node_name) == name_to_index.end()) {
    initialize_transition_table (transition_table, name_to_index.size ());
    name_to_index.insert (make_pair (child_node_name, name_to_index.size ()));
    map_name_to_output (child_node_name);
  }

  transition_table[curent_node_index][inp_index] = child_node_name;
}

/*
  initialize transition table with dashes
*/
void
Parser::initialize_transition_table (
    std::vector<std::vector <std::string> > &transition_table, int index)
{
  std::vector<std::string> tmp;
  for (int i = 0; i < input_index.size(); i++) {
    tmp.push_back ("-");
  }
  transition_table.push_back (tmp);
}


/*
  build the input map
*/
void
Parser::construct_input ()
{
  // 0 - 9 a-z A-Z + - * / . ; , ( ) { } = ! > <
  int index = 0;
  for (char i = '0'; i <= '9'; i++)
    input_index.insert (make_pair (i, index++));

  for (char i = 'a'; i <= 'z'; i++)
      input_index.insert (make_pair (i, index++));

  for (char i = 'A'; i <= 'Z'; i++)
      input_index.insert (make_pair (i, index++));

  input_index.insert (make_pair ('+', index++));
  input_index.insert (make_pair ('-', index++));
  input_index.insert (make_pair ('*', index++));
  input_index.insert (make_pair ('/', index++));
  input_index.insert (make_pair ('.', index++));
  input_index.insert (make_pair (';', index++));
  input_index.insert (make_pair (',', index++));
  input_index.insert (make_pair ('(', index++));
  input_index.insert (make_pair (')', index++));
  input_index.insert (make_pair ('{', index++));
  input_index.insert (make_pair ('}', index++));
  input_index.insert (make_pair ('=', index++));
  input_index.insert (make_pair ('!', index++));
  input_index.insert (make_pair ('>', index++));
  input_index.insert (make_pair ('<', index++));

}

/*
  return the regex with higher acceptance state
*/
bool
Parser::has_higher_acceptance (std::string cur_acc, std::string new_acc)
{
    return acceptance_priority.at (new_acc) < acceptance_priority.at (cur_acc);
}

/*
  concatenate the nodes' name
*/
std::string
Parser::concatenate(std::vector<Nfa::Node*> &nodes)
{
  std::set<int> temp;
  std::string expr = sort_the_nodes_name (temp, nodes);

  // hash the set to a string
  std::string name = "";
  std::stringstream ss;
  for (std::set<int>::iterator it = temp.begin(); it != temp.end(); it++) {
    ss.str(std::string()); // reset stringstream
    ss << (*it);
    name = name + "~" + ss.str();
  }
  acceptance_to_expr.insert (make_pair (name, expr));
  return name;
}

/*
  sort the data according to state_no
*/
std::string
Parser::sort_the_nodes_name (std::set<int> &temp,
  std::vector<Nfa::Node*> &nodes)
{
  std::string expr = "-";
  for (int i = 0; i < nodes.size (); i++) {
    temp.insert (nodes[i]->state_no);
    if (has_higher_acceptance (expr, nodes[i]->accept_state)) {
      expr = nodes[i]->accept_state;
    }
  }
  return expr;
}

/*
  utility function to print the acceptance state of the states
*/
void
Parser::print_acceptance_state ()
{
  std::cout << "Acceptance State" << std::endl;
  for (std::map<std::string, std::string>::iterator it = acceptance_to_expr.begin();
        it != acceptance_to_expr.end(); it++) {
            std::cout << "State name " << name_to_output.at ((*it).first) << " "
            << " type " << (*it).second << std::endl;
        }
}

/*
  minimize the DFA
*/
void
Parser::minimize_dfa (std::vector<std::vector <std::string> > &transition_table
      ,std::map<std::string, int> &name_to_index) {

  bool e3ref_lew7dk = false;

  // iterate until there is no change
  while (!e3ref_lew7dk) {
      e3ref_lew7dk = true;
      // map grouping the states by their acceptance state
      std::map<std::string, std::vector<std::string> > grouping_states;
      // iterate over the states and group them accorfing to their acceptance state
      group_states (grouping_states);

      // new acceptance_to_expr
      std::map<std::string, std::string> new_acceptance_to_expr;
      // iterate over each group
      for (std::map<std::string, std::vector<std::string> >::iterator
        it = grouping_states.begin(); it != grouping_states.end(); it++) {
          //map contains the new parent in the current group
          std::map<std::string, std::string > group_to_parent;
          // states in the group
          std::vector<std::string> curr_vec = it->second;
          int unique = 0;
          // iterate over the states in the group
          for (int i = 0; i < curr_vec.size (); i++) {
             // vector contains the next states for every input for the cur state
             std::vector <std::string> curr_vec_out =
             transition_table[name_to_index.at (curr_vec[i])];
             // hash the name of this vector
             std::string hash = it->first + "~" + hash_name (curr_vec_out);

             std::map<std::string, std::string>::iterator val
             = group_to_parent.find (hash);
             // new parent
             if (val == group_to_parent.end()) {
               if (unique > 0) e3ref_lew7dk = false;
               stringstream ss;
               ss << unique;
               // create new parent
               std::string new_name = it->first  + "~" + ss.str ();
               group_to_parent.insert (make_pair (hash, new_name));
               new_acceptance_to_expr.insert (make_pair (curr_vec[i], new_name));
               unique++;
             } else { // old parent
                new_acceptance_to_expr.insert (make_pair (curr_vec[i], val->second));
             }
          }
        }
        acceptance_to_expr = new_acceptance_to_expr;
  }
}


void
Parser::group_states (std::map<std::string,
    std::vector<std::string> > &grouping_states)
{
  for (std::map<std::string, std::string>::iterator
    it = acceptance_to_expr.begin(); it != acceptance_to_expr.end(); it++) {
      std::map<std::string, std::vector<std::string> >::iterator elem
      = grouping_states.find (it->second);

      if (elem != grouping_states.end ()) {
          elem->second.push_back (it->first);
          grouping_states.insert (make_pair (it->second, elem->second));
      } else {
          std::vector<std::string> temp;
          temp.push_back (it->first);
          grouping_states.insert (make_pair (it->second, temp));
      }
  }
  //print_groups (grouping_states);
}

/*
  utility function to print the groups of the state
*/
void
Parser::print_groups (std::map<std::string,
    std::vector<std::string> > &grouping_states)
{
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            << std::endl;
  for (std::map<std::string, std::vector<std::string> >::iterator
    it = grouping_states.begin(); it != grouping_states.end(); it++) {
      std::cout << "Size of group " << it->first << " is " << it->second.size ()
      << std::endl;
    }
    std::cout << "Size of MINI " << grouping_states.size () << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
              << std::endl;
}

/*
  hash the name of the state
*/
std::string
Parser::hash_name (std::vector <std::string>  &curr_vec_out)
{
  std::string hash_name = curr_vec_out[0] == "-" ?
        "-" : acceptance_to_expr.at (curr_vec_out[0]);

  for (int i = 1; i < curr_vec_out.size (); i++) {
      std::string temp = curr_vec_out[i] == "-" ?
            "-" : acceptance_to_expr.at (curr_vec_out[i]);
      hash_name += "~" + temp;
  }
  return hash_name;
}
