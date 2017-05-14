#ifndef NFA_H
#define NFA_H
#include <utility>      // std::pair
#include <vector>      // std::vector
#include <string>
#include <stdlib.h>
#include <map>
#include <list>

using namespace std;

class Nfa {
public:
  static unsigned int node_count;
  typedef struct Node {
    int state_no;
    string accept_state;
    Node () {
      state_no = Nfa::node_count++;
      accept_state = "-";
    }
    // holds next node with the transition input char to it.
    vector <pair<struct Node*, char> > Next_trans;
  } Node;
  //int size = 0;
  Nfa (string input);
  Nfa (Node *start, Node *end);
  Node *Start;
  Node *End;
  static Nfa* do_concat (Nfa *, Nfa *);
  static Nfa* do_or (Nfa *, Nfa *);
  static Nfa* do_zero_more (Nfa *);
  static Nfa* do_one_more (Nfa *);
  static Nfa* do_period (vector<char>);
  static Nfa* multiple_or (map<std::string, Nfa> &);
  void concat_chars (string);
  Nfa* clone ();
  Nfa* bfs ();

  Node* get_start ();
  Node* get_end ();
  void set_end (Node* new_end);

};

#endif // NFA_H
