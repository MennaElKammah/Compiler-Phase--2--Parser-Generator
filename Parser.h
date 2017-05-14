#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include "Nfa.h"
class Parser {
  public:
    Parser ();
    void parse (void);
    std::vector<std::string> get_tokens ();
  private:
    std::vector<std::string> out_tokens;
    std::map<std::string, Nfa> cache;
    std::map<std::string, Nfa> components;
    std::map<std::string, int> op_prior;
    std::set<std::string> symbol_table;
    std::map<char, int> input_index;
    std::map<std::string, std::string> name_to_output;
    std::map<std::string, std::string> acceptance_to_expr;
    std::map<std::string, int> acceptance_priority;
    std::map<int, std::string> index_to_state;
    int state_name;
    int acc_prior;
    std::string clean_type (std::string);
    std::vector<std::string> check_type (std::vector<std::vector <std::string> > &,
       std::string, std::map<std::string, int> &);
    std::vector<std::string> split_on_whitespaces (std::string);
    std::string concatenate(std::vector<Nfa::Node*>&);
    std::string remove_backslash (std::string);
    std::string hash_name (std::vector <std::string>  &);
    std::string sort_the_nodes_name (std::set<int> &, std::vector<Nfa::Node*> &);
    void clean_transition_table (std::vector<std::vector <std::string> > &,
          std::map<std::string, int> &);
    bool has_higher_prior(std::string, std::string);
    bool is_closed_paren(std::string);
    bool is_binary(std::string);
    bool is_operator(std::string);
    bool has_higher_acceptance (std::string, std::string);
    bool is_transition_epslon (char);
    void construct_op_prior();
    void construct_input ();
    void parse_input_file (std::vector<std::vector <std::string> > &
      , std::map<std::string, int> &);
    void print_groups (std::map<std::string, std::vector<std::string> > &);
    void add_fay_node (std::vector<std::vector <std::string> > &,
      std::map<std::string, int> &);
    void update_input_map (std::map<char, std::set<Nfa::Node*> > &i,
        char , Nfa::Node*);
    void get_big_node_epslon (std::map<char, std::set<Nfa::Node*> > &,
          std::vector<std::vector <std::string> > &,
          std::map<std::string, int> &, std::string,
          std::map<std::string, std::vector<Nfa::Node*> > &,
          std::queue<std::string> &);
    void get_esplon_closure_for_input (std::vector<Nfa::Node*> &,
          std::set<Nfa::Node*> &);
    void add_node_to_queue (std::queue<std::string> &,
          std::map<std::string, std::vector<Nfa::Node*> > &, std::string,
          std::vector<Nfa::Node*> &);
    void insert_elem_in_acceptance_state (std::vector<std::string> &);
    void insert_acceptance_state (std::string, int);
    void inspect_big_node_children (std::vector<Nfa::Node*> &,
        std::map<char, std::set<Nfa::Node*> > &);
    void inspect_children (Nfa::Node*, std::map<char, std::set<Nfa::Node*> > &);
    void print_tokens (std::vector<std::string> &);
    void split_line (std::vector<std::string> &, std::string);
    void evaluate_exp(vector<string> &);
    void print(Nfa::Node *);
    void print_levels(Nfa::Node*);
    void build_dfa(Nfa::Node*, std::vector<std::vector <std::string> > &,
        std::map<std::string, int> &);
    void get_epslon_closure(Nfa::Node*, std::vector<Nfa::Node* >&);
    void map_name_to_output (std::string);
    void print_acceptance_state ();
    void minimize_dfa (std::vector<std::vector <std::string> > &
          ,std::map<std::string, int> &);
    Nfa* construct_NFA(Nfa *, Nfa *, std::string);
    Nfa* build_period (Nfa *, Nfa*);
    void add_node_to_transition_table (std::vector<std::vector <std::string> > &,
        std::map<std::string, int> &, std::string, std::string, char);
    void initialize_transition_table (std::vector<std::vector <std::string> > &,
        int);
    void group_states (std::map<std::string, std::vector<std::string> > &);
    void print_transition_table (std::vector<std::vector <std::string> > &);
    };

#endif // PARSER_H
