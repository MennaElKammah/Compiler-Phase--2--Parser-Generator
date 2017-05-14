#include "Node.h"

Node::Node (bool atomic, bool terminal, string name) {
  this->atomic = atomic;
  this->terminal = terminal;
  this->name = name;
}
Node::Node (string name) {
  this->name = name;
  this->atomic = true;
  this->terminal = false;
}

Node::Node (vector<Node> node_components) {
  this->atomic = false;
  this->terminal = false;
  this->name = "";
  for (int i = 0; i < node_components.size (); ++i) {
    components.push_back (node_components[i]);
    this->name += "." + node_components[i].get_name ();
  }
}

string Node::get_name() {
  return name;
}

bool Node::is_atomic() {
  return atomic;
}

bool Node::is_terminal() {
  return terminal;
}

vector<Node> Node::get_components() {
  return components;
}

void Node::add_component(Node node) {
  components.push_back (node);
}

void Node::remove_component (int i) {
  components.erase (components.begin () + i);
}

void Node::set_name (string new_name) {
  name = new_name;
}
void Node::set_atomic (bool atomic) {
  this->atomic = atomic;
}
void Node::set_terminal (bool terminal) {
  this->terminal = terminal;
}
