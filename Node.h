#ifndef Node_H
#define Node_H

#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;

class Node {
private:
    bool atomic;
    bool terminal;
    vector<Node> components;
public:
    bool operator <(const Node& rhs) const
    {
      string temp = rhs.name;
      return name.compare (temp) < 0;
    }

    bool operator !=(const Node& rhs) const
    {
      string temp = rhs.name;
      return name.compare (temp) != 0;
    }

    bool operator ==(const Node& rhs) const
    {
      string temp = rhs.name;
      return name.compare (temp) == 0;
    }
    string name;
    Node (bool, bool, string);
    Node (string);
    Node (vector<Node> node_components);
    string get_name();
    bool is_atomic();
    bool is_terminal();
    vector<Node> get_components();
    void add_component(Node); // it should add the new_node name to the node name
    void remove_component (int i);
    void set_name (string new_name);
    Node create_node (vector<Node>);
    void set_atomic (bool);
    void set_terminal (bool);
};

#endif // Node_H
