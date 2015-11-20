#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

vector<string>
get_command(string line) {
	vector<string> v;
	istringstream ss(line);
	string token;

	while(getline(ss, token, ' ')) {
		v.push_back(token);		
	}

	return v;
}

bool 
validate_command(vector<string> cmd_str) {

	if (cmd_str.size() > 3)
		return false;

	if (cmd_str[0] != "SET" &&
		cmd_str[0] != "GET" &&
		cmd_str[0] != "UNSET" &&
		cmd_str[0] != "NUMEQUALTO" &&
		cmd_str[0] != "BEGIN" &&
		cmd_str[0] != "END" &&
		cmd_str[0] != "ROLLBACK" &&
		cmd_str[0] != "COMMIT") {
			return false;		
	}

	return true;
}
