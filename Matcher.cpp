#include "Matcher.h"


void
Matcher::match_input(Table table, Node start, vector<string> tokens) {

  Node *end_of_input = new Node(true, true,"$");
  tokens.push_back("$");
  stack<Node> nodes;
  nodes.push(*end_of_input);
  nodes.push(start);
  string input;
  ofstream out_file;
  out_file.open (OUT_FILE);
  if (!tokens.empty())
  {
    int idx = 0;
    while (idx < tokens.size())
    {
      input = tokens[idx];
      Node curr = nodes.top();
      if (is_equal(curr, *end_of_input) && input.compare("$") != 0)
      {
        out_file << "Extra Input :" << endl;
        while(idx < tokens.size())
          out_file << tokens[idx++] << endl;
        return;
      }
      if (curr.is_terminal() && !is_equal(curr, *(table.sync_node))
          && !is_equal(curr, *(table.error_node))
          && !is_equal(curr, *(table.epslon_node))) // TOS is terminal
      {
        nodes.pop();
        if (!is_equal(curr, input))
        {
            // missing input to match with TOS
            out_file << "Missing " << curr.get_name() << endl;
            continue;
        }
        // TOS matched with input
        out_file << "Matched: " << curr.get_name() << endl;
        idx++;
      }
      else
      {
        //TOS is non-terminal
        if (is_equal(curr, *(table.sync_node))) // sync
        {
            out_file << "Missing first of " << curr.get_name() << endl;
            nodes.pop();
            continue;
        }
        // table entry is error
        // Note: if end of input (i.e, "$"),
        // then keep it in input but report missing first of the current state
        else if (is_equal(curr, *(table.error_node))) // menna
        {
          //????  if (!is_equal(curr, end_of_input)) // not end of input
            if (input.compare("$") != 0)
            {
              out_file << "Extra " << input << endl;
              idx++;
            }
            else // input is "$" and stack is not empty
            {
              out_file << "Missing input!" << endl;
            }
            nodes.pop ();
        }
        else if (is_equal(curr, *(table.epslon_node))) // epslon
        {
           out_file << "Epslon" << endl;
           nodes.pop();
        }
        else
        {
          // pop current TOS and push its R.H.S. components
          out_file << curr.get_name() << endl;
          nodes.pop();
          Node next = table.get_node(curr.get_name(), input);
          vector<Node> comp = next.get_components();
          vector<Node>::reverse_iterator it = comp.rbegin();
          for (; it != comp.rend(); ++it)
            nodes.push(*it);
        }
      }
    }
    /*
    handle non empty stack and empty input
    while(!nodes.empty())
    {
      Node curr = nodes.top();

    }


    // supposed to be handled above  menna ???????????
    */
  }
  else
  {
  	out_file << "Empty input file";
  }
  out_file.close();
}

bool
Matcher::is_equal(Node first, Node second)
{
  return (first.get_name().compare(second.get_name()) == 0);
}
