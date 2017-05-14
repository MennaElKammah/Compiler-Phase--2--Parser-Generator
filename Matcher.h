#ifndef MATCHER_H
#define MATCHER_H
#include <bits/stdc++.h>
#include "Node.h"
#include "Table.h"
#include "CFG_Parser.h"

#define OUT_FILE "output.txt"
using namespace std;

class Matcher {
public:
void match_input(Table, Node, vector<string>);
bool is_equal(Node, Node);
};

#endif // MATCHER
