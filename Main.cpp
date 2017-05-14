#include <iostream>
#include "LL_Grammar.h"
#include "Computation.h"
#include "Matcher.h"
#include "Parser.h"


vector<string>
read_from_file (string file_name)
{
	vector<string> vec;
	std::ifstream inp_stream(file_name);
  std::string line;
  if (inp_stream.good () && inp_stream.is_open ()) {
      while (std::getline(inp_stream, line)) {
          vec.push_back (line);
      }
  } else {
    std::cout << "Fail to open input file" << std::endl;
		exit (0);
  }
	return vec;
}

int main(int argc, char const *argv[]) {
	cout << "TEST MAIN" << endl << endl;
	string file_name;
	Parser *parser = new Parser ();
	if (argc > 1) {
		file_name = argv[1];
		cout << "file name is " << file_name << endl;
	}
	if (argc <= 1) {
		cout << "IN  IN" << endl;
		parser->parse ();
	}

	LL_Grammar *g = new LL_Grammar ();
  g->build_prods_as_nodes ();
  map<Node, vector<Node> > grammar = g->get_clean_grammar ();
  //g->print_gramm_as_nodes ();
	Computation *comp = new Computation (grammar, g->get_start_node ());
	comp->construct_first ();
	//comp->print_first ();
	comp->construct_follow ();
	//comp->print_follow ();
	Table *table = new Table (g->get_col_names (), grammar,
	comp->get_map_of_first (), comp->get_map_of_follow ());
	table->print_table ();
	Matcher *matcher = new Matcher ();
	if (argc > 1)
		matcher->match_input (*table, *(comp->start_node), read_from_file (file_name));
	else
		matcher->match_input (*table, *(comp->start_node), parser->get_tokens ());
	return 0;
}
