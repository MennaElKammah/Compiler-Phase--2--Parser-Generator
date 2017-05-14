#include "CFG_Parser.h"

string
Prod::trim(string str)
{
	int low;
	int high;
	int itr = 0;
	while (itr < str.size() && str[itr] == ' ')
		itr++;
	low = itr;
	itr = str.size() - 1;
	while (itr >= 0 && str[itr] == ' ')
		itr--;
	high = itr + 1;
	return str.substr(low, high - low);
}

Prod::Prod(string str)
{
	string delim = "=";
	size_t key_last_idx = str.find(delim);
	this->key = str.substr(0, key_last_idx);
	this->key = trim(this->key);
	// remove key part
	str = trim(str.substr(key_last_idx + 1));
	delim = "|";
	size_t low = 0; //indicates production start
	while (!str.empty()) {

	  		low = str.find(delim);
	  		//current production
	  		string curr_prod = (low == string::npos)? trim(str) : trim(str.substr(0, low));
	  		//cout << curr_prod << endl;
	  		vector<string> tokens;
	  		//scan tokens
	  		while (!curr_prod.empty())
	  		{
		  		curr_prod = trim(curr_prod);
		  		string token_delim = " ";
		  		size_t token_last_idx = curr_prod.find(token_delim);
		  		if (token_last_idx == string::npos)
		  		{
		  			tokens.push_back(curr_prod);
		 			break;
		  		}
		  		tokens.push_back(curr_prod.substr(0, token_last_idx));
		  		curr_prod = curr_prod.substr(token_last_idx);
		  	}
		  	this->values.push_back(tokens);
	  		str = (low == string::npos)? str.substr(str.size()) : str.substr(low + 1);
	}
/*
	for (int i = 0; i < this->values.size(); i++) {
	    for (int j = 0; j < this->values[i].size(); j++) {
	     	cout << this->values[i][j] << '\n';
	    }
	}
*/
}

string
Prod::get_key()
{
	return this->key;
}

vector< vector<string> >
Prod::get_values()
{
	return this->values;
}

bool
Prod::is_terminal(string str)
{
	return str.size() > 0 && (str[0] == '\'') && (str[str.size() - 1] == '\'');
}

string
Prod::remove_quotes(string str)
{
	return (str.size() >= 2)? str.substr(1, str.size() - 2) : str;
}

Prod_Itr::Prod_Itr ()
{
  // read CFG input file
  string line;
  string lines;
  ifstream CFG_file(FILE_PATH);
  if (CFG_file.is_open())
  {
    while (getline(CFG_file,line))
    {
      lines += line;
    }
    CFG_file.close();
  }
  else
  {
  	cout << "Unable to open CFG file";
  }
  //removes all newline characters
  lines.erase(remove(lines.begin(), lines.end(), '\n'), lines.end());
  //replace all "‘" characters
 // replace(lines.begin(), lines.end(), "‘" , "'");
  //removes all newline characters
 // replace(lines.begin(), lines.end(), "’" , "'");
  //produce each production as a token
  string delim = "#";
  size_t low; //indicates production start
  size_t high; //indicates production end
  while ((low = lines.find(delim)) != string::npos) {

  		high = lines.find(delim, low + 1);
  		//current production
  		string curr_prod = (high == string::npos)? lines.substr(low + 1) : lines.substr(low + 1, high - low - 1);
  		this->prod.push_back(curr_prod);
  		lines = (high == string::npos)? lines.substr(lines.size()) : lines.substr(high);
  }
  this->itr = 0;
  /*
  for (int i = 0; i < this->prod.size(); i++) {
  		cout << "------------------------" << endl;
  		cout << this->prod[i] << '\n';
  		new Prod(this->prod[i]);
  }
  */
}


bool
Prod_Itr::has_next()
{
	return (this->itr < this->prod.size());
}

Prod *
Prod_Itr::get_next()
{
	return new Prod(this->prod[this->itr++]);
}
