#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
using namespace std;

class Transaction {
	int t_id;
	map<string, int> varMap;
	map<int, int> valMap;
	
public:
	Transaction(int);
	int get_tid();
	void add_value(string, int);
	int get_value(string);
	int get_valMap_value(int);
	void set_valMap_value(int,int);
	void clear_value(string);
	void print_map();
	void print_vmap();

	map<string,int> get_varMap();
	map<int, int> get_valMap();
	void populate_vmaps(map<string,int>);
	
};

Transaction::Transaction(int id) {
	t_id = id;
}

int
Transaction::get_tid() {
	return (t_id);
}

int
Transaction::get_valMap_value(int key) {
	if (valMap.find(key) != valMap.end())
		return(valMap[key]);
	else
		return (-1);
}

void
Transaction::set_valMap_value(int key, int new_val) {
	valMap[key] = new_val;
}

void 
Transaction::add_value(string var_name, int val) {
	//first find the val in map, we need to clear out the valMap
	if (varMap.find(var_name) == varMap.end()) {
		varMap[var_name] = val;
		valMap[val]++;
	}
	else {
		//cout<<"val:"<<val<<"varMap:"<<varMap[var_name]<<endl;
		if (val != varMap[var_name]) {
			// a new value for same var, need to change valMap
			if (valMap[varMap[var_name]] != 1)
				valMap[varMap[var_name]]--;
			else
				valMap.erase(varMap[var_name]);
			
			varMap[var_name] = val;
			valMap[val]++;
			cout<<"valmap addition:"<<valMap[val]<<endl;
		}
		
	}

}

int
Transaction::get_value(string var_name) {
	if(varMap.find(var_name) != varMap.end()) {
		return(varMap[var_name]);
	} else
		return (-1);
}

void 
Transaction::clear_value(string var_name) {
	// change the valMap accordingly, prior to removing from varMap	
	

	if(varMap.find(var_name) != varMap.end()) {
		if (valMap[varMap[var_name]] > 0)
			valMap[varMap[var_name]]--;
		else
			valMap.erase(varMap[var_name]);		
	} else {
		valMap.erase(varMap[var_name]);
	}
	varMap.erase(var_name);
	//cout<<"val hamp:"<<valMap[varMap[var_name]]<<endl;
}

void
Transaction::print_map() {
	map<string, int>::iterator it;
	for(it = varMap.begin(); it !=  varMap.end(); it++) {
		cout<<"key:"<<it->first<<" val:"<<it->second<<endl;
	}
}

void
Transaction::print_vmap() {
	map<int, int>::iterator it;
	for(it = valMap.begin(); it !=  valMap.end(); it++) {
		cout<<"ValMap key:"<<it->first<<" val:"<<it->second<<endl;
	}
}


map<string,int> 
Transaction::get_varMap() {
	return(varMap);
}

map<int, int>
Transaction::get_valMap() {
	return (valMap);
}

void
Transaction::populate_vmaps(map<string,int> var_map) {
	// copy var_map to our local varMap
	map<string, int>::iterator varmap_it;
	int old_val;

	for(varmap_it = var_map.begin(); varmap_it != var_map.end(); ++varmap_it) {

		if (varMap.find(varmap_it->first) == varMap.end()) {
			varMap[varmap_it->first] = varmap_it->second;
			valMap[varmap_it->second]++;
		}
		else {
			// we need to note down the old val to adjust the 
			// count in valMap
			old_val = varMap[varmap_it->first];
			valMap.erase(old_val);
			varMap[varmap_it->first] = varmap_it->second;
			valMap[varmap_it->second]++;

		}
		

		
	}

	
}
