#include "Nfa.h"
#include <iostream>
using namespace std;

unsigned int Nfa::node_count = 0;

// Initiate new NFA with one transition
Nfa::Nfa (string input) {
  Start =  new Node ();
  End =  new Node ();
  if (input.length () > 1) { // This means multiple chars to be concated
      concat_chars (input);
  }
  else { // Only one char transition
    Start->Next_trans.push_back (make_pair(End, input.at (0)));
  }
}

Nfa::Nfa (Node* start, Node *end) {
  Start = new Node ();
  End = new Node ();
  this->Start = start;
  this->End = end;
}

void Nfa::concat_chars (string str) {
  Node *nodex = new Node ();
  Start->Next_trans.push_back (make_pair (nodex, str.at(0)));
  for (int i = 1; i < str.length(); ++i) {
     Node *temp = new Node ();
     nodex->Next_trans.push_back (make_pair (temp, str.at(i)));
     nodex = temp;
  }

  this->set_end (nodex);
}

Nfa* Nfa::do_concat (Nfa *nfa1, Nfa *nfa2) {
  // Make the end of nfa1 points to the start of nfa2
  nfa1->get_end ()->Next_trans.push_back (make_pair (nfa2->get_start (), '$'));
  // size = nfa1.get_size () + nfa2.get_size ();
  nfa1->set_end (nfa2->get_end());
  return nfa1;
}

Nfa* Nfa::do_or (Nfa *nfa1, Nfa *nfa2) {
  Node *new_start =  new Node();
  Node *new_end =  new Node();
  // make epsilon transitions from the new_start to both NFAs
  new_start->Next_trans.push_back(make_pair(nfa1->get_start (), '$'));
  new_start->Next_trans.push_back(make_pair(nfa2->get_start (), '$'));

  // Make nfa1, nfa2 ends point to the new_end with epsilon transition
  nfa1->get_end ()->Next_trans.
            push_back (make_pair(new_end, '$'));
  nfa2->get_end ()->Next_trans.
            push_back (make_pair(new_end, '$'));

  // size += 2 + nfa1.get_size () + nfa2.get_size ();
  Nfa* final_nfa_result = new Nfa (new_start, new_end);
  return final_nfa_result;
}

Nfa* Nfa::do_zero_more (Nfa *nfa) {
  Node *start = new Node();
  Node *end =  new Node();
  // make transition from new_start to new_end with epsilon transition
  start->Next_trans.push_back (make_pair(end, '$'));

  // Make epsilon transition from new_start to the nfa start
  start->Next_trans.push_back (make_pair(nfa->get_start (), '$'));

  // Make epsilon transition from nfa end to the new_end
  nfa->get_end()->Next_trans.push_back (make_pair(end, '$'));

  // Make epsilon transition from nfa end to nfa start
  nfa->get_end()->Next_trans.push_back (make_pair(nfa->get_start (), '$'));

  // size += 2 + nfa1.get_size () + nfa2.get_size ();
  Nfa* final_nfa_result = new Nfa (start, end);
  return final_nfa_result;
}
/*
  Same as zero or more time,
  but without epsilon transition from new_start to new_end
*/
Nfa* Nfa::do_one_more (Nfa *nfa) {
  Node *start =  new Node();
  Node *end =  new Node();

  // Make epsilon transition from new_start to the nfa start
  start->Next_trans.push_back (make_pair(nfa->get_start (), '$'));

  // Make epsilon transition from nfa end to the new_end
  nfa->get_end()->Next_trans.push_back (make_pair(end, '$'));

  // Make epsilon transition from nfa end to nfa start
  nfa->get_end()->Next_trans.push_back (make_pair(nfa->get_start (), '$'));
  // size += 2 + nfa1.get_size () + nfa2.get_size ();
  Nfa* final_nfa_result = new Nfa (start, end);
  return final_nfa_result;
}

Nfa* Nfa::do_period (vector<char> input) {
  Node *start =  new Node();
  Node *end =  new Node();
  // For each input make a start state and end state with the input transition
  // Do one start state that connects all this new states, with epsilon trans.
  // Do one end state that'd be the end of all transitions
  for(size_t i = 0; i != input.size(); i++) {
    Node *new_start_state = new Node();
    Node *new_end_state = new Node();

    // make new transition with the given input
    // from new_start_state to new_end_state with input
    new_start_state->Next_trans.push_back (make_pair(new_end_state, input[i]));

    // make epsilon transitions between each end of the period
    // with the Nfa end
    new_end_state->Next_trans.push_back (make_pair(end, '$'));

    // make epsilon transitions between start of Nfa
    // with start of each period
    start->Next_trans.push_back (make_pair(new_start_state, '$'));
    // size += 2;
  }
  // size += 2;
  Nfa* final_nfa_result = new Nfa (start, end);
  return final_nfa_result;
}

Nfa* Nfa::clone () {
  return bfs ();
}

Nfa* Nfa::bfs () {
  Node *start =  new Node();
  Node *end =  new Node();
  // start->Next_trans (make_pair (end, '$'));
  Nfa* new_nfa = new Nfa (start, end);
  map <Node*, Node*> visited;

  // Create a queue for BFS
  list<pair <Node*, Node*> > queue;

  // Mark the current node as visited and enqueue it
  visited.insert (make_pair (Start, new_nfa->get_start ()));
  queue.push_back (make_pair (Start, new_nfa->get_start ()));

  Node* node1;
  Node* node2;
  while(!queue.empty())
  {
      // Dequeue a vertex from queue and print it
      node1 = queue.front ().first;
      node2 = queue.front ().second;
      queue.pop_front ();

      // Get all adjacent vertices of the dequeued vertex s
      // If a adjacent has not been visited, then mark it visited
      // and enqueue it
      for(int i = 0; i < node1->Next_trans.size (); ++i)
      {
          map<Node*, Node*>::iterator itr = visited.find (
            node1->Next_trans[i].first);
          Node* n;
          if(!(itr != visited.end()))
          {
              n = new Node();
              visited.insert (make_pair (node1->Next_trans[i].first, n));
              queue.push_back (
                make_pair (node1->Next_trans[i].first, n));
          }
          else {
            n = (*itr).second; // last visited node
          }
          node2->Next_trans.push_back (
            make_pair (n, node1->Next_trans[i].second));
      }
  }
  node2->Next_trans.push_back (make_pair (new_nfa->get_end (), '$'));
  return new_nfa;
}

Nfa::Node* Nfa::get_start () {
  return this->Start;
}

Nfa::Node* Nfa::get_end () {
  return this->End;
}

void Nfa::set_end (Node* new_end) {
  this->End = new_end;
}

Nfa* Nfa::multiple_or (map<std::string, Nfa> &nfa_components) {
  Node *start = new Node ();
  for(map<string, Nfa>::iterator it = nfa_components.begin();
   it != nfa_components.end(); ++it) {
    Nfa nfa = it->second;
    start->Next_trans.push_back (make_pair (nfa.get_start (), '$'));
  }
  Nfa *nfa = new Nfa (start, NULL);
  return nfa;
}
