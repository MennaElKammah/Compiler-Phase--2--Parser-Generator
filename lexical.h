#include <iostream>
class NFA {
  public:
    typedef struct Node {
      struct Node *start;
      struct Node *end;
      char input;
    }Node;

    construct_nfa (char op, string[] operands);
    Node* get_start ();
    Node* get_end ();
  private:
    Node *start;
    Node *end;

};
