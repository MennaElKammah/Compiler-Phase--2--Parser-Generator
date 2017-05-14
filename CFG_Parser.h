#ifndef CFG_PARSER_H
#define CFG_PARSER_H
// parse file
// class hyb2a m3aya obj menha ht2sm el file stor w kol str byshawr 3ly b3do, w el satr 3bara 3n key (lhs string)
// main function reads file production and makes a map key, value of the lhs and rhs
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#define FILE_PATH "./CFG.txt"
using namespace std;

class Prod { //Production
private:
    string key;
    vector< vector<string> > values;
public:
    Prod(string);
    static string trim(string);
    string get_key();
    vector< vector<string> > get_values();
    bool is_terminal(string);
    string remove_quotes(string);
};

class Prod_Itr {
private:
    vector<string> prod;
    int itr;
public:
    Prod_Itr();
    bool has_next();
    Prod* get_next();
};

#endif // CFG_PARSER_H
