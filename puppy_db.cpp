#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <map>
#include "transaction.h"
#include "string_delim.h"

using namespace std;

//global transaction stack
vector<Transaction*> trans_stack;
map<string, vector<int> > var_tid_stack;
map<int, vector<int> > val_tid_stack;

void
var_tid_stack_rollback() {
	//cleanse var_tid_stack of all tid entries
	map<string, vector<int> >::iterator it;
	
	for(it = var_tid_stack.begin(); it != var_tid_stack.end();it++) {
		if (it->second.size() != 1)
			it->second.pop_back();
	}	
}

void
val_tid_stack_rollback() {
	//cleanse var_tid_stack of all tid entries
	map<int, vector<int> >::iterator it;
	
	for(it = val_tid_stack.begin(); it != val_tid_stack.end();it++) {
		if (it->second.size() != 1)
			it->second.pop_back();
	}	
}

void 
process_set(vector<string> cmd_str) {
	int val, prev_tid;
	int prev_var_tid;

	int prev_val_tid, pre_val1, pre_val_count;
	int val1, val2;

	int size;
	int prev_valMap_val;
	Transaction* t = trans_stack.back();

	val = atoi(cmd_str[2].c_str());
	t->add_value(cmd_str[1], val);

	if (var_tid_stack.size() == 0) {
		var_tid_stack[cmd_str[1]].push_back(t->get_tid());
		val_tid_stack[val].push_back(t->get_tid());

	}
	else {
		// if its a never seen before variable
		if (var_tid_stack.find(cmd_str[1]) == var_tid_stack.end()) {	

			var_tid_stack[cmd_str[1]].push_back(t->get_tid());

			// if the value added by the new variable was seen before
			if (!val_tid_stack[val].empty()) {
				prev_tid = val_tid_stack[val].back();
				// last count of the value seen before
				pre_val_count = trans_stack[prev_tid]->get_valMap_value(val);
				// only if its a changed when compared to
				if (prev_tid != t->get_tid()) {
					t->set_valMap_value(val, pre_val_count + (t->get_valMap_value(val)));
					val_tid_stack[val].push_back(t->get_tid());
				}				
					
			} else {
				// if the value was never seen before ... add it to the val_tid_stack
				val_tid_stack[val].push_back(t->get_tid());
			}
		} else {

			/* We have seen this variable in other transactions, we need to do two things:
				1) add the val_count of present value by one
				2) negate the prev_val_count by one.
			*/
			size = var_tid_stack[cmd_str[1]].size();
			//store the last tid that var was changed
			int prev_neg_var_tid = var_tid_stack[cmd_str[1]][size - 1];	
			// ensure to add new tid only if last changed tid is not the present tid
			if (var_tid_stack[cmd_str[1]].back() != t->get_tid())						
				var_tid_stack[cmd_str[1]].push_back(t->get_tid());
			
			if (!val_tid_stack[val].empty()) {
				//last tid where val changed
				prev_tid = val_tid_stack[val].back();
				//last value count the val changed to
				pre_val_count = trans_stack[prev_tid]->get_valMap_value(val);

				//if previous changed tid is not present tid, add last val count to present val count
				if (prev_tid != t->get_tid()) {				
					t->set_valMap_value(val, pre_val_count + (t->get_valMap_value(val)));
					val_tid_stack[val].push_back(t->get_tid());
				}				
				
			} else {
				val_tid_stack[val].push_back(t->get_tid());
			}

			//Now negate old value			
			
			//since we push var in var_tid_stack, obtain the second last push.
			// and use it to obtain the val it previously held in that tid			
			if (prev_neg_var_tid != t->get_tid()) {					
				// we get value of var from prev_tid
				val1 = trans_stack[prev_neg_var_tid]->get_value(cmd_str[1]);	

				if (!val_tid_stack[val1].empty()) {
					//obtain the count of var val1
					val2 = trans_stack[prev_neg_var_tid]->get_valMap_value(val1);
					//decrement it				
					t->set_valMap_value(val1, val2-1);

					if (val_tid_stack[val1].back() != t->get_tid()) {
						val_tid_stack[val1].push_back(t->get_tid());					
					}
				}
			}
			
		}
	}

}

int 
process_get(vector<string> cmd_str) {
	int val;
	int prev_tid;

	Transaction* t = trans_stack.back();
	Transaction* prev_t;

	if (t->get_tid() == 0) {
		val = t->get_value(cmd_str[1]);
		return (val);
	} else {

		val = t->get_value(cmd_str[1]);
		if (val != -1) {
			return (val);
		} else {
			// check in previos transaction
			prev_tid = var_tid_stack[cmd_str[1]].back();
			val = trans_stack[prev_tid]->get_value(cmd_str[1]);
			return (val);
		}
	}
}

int
process_numequal(vector<string> cmd_str) {
	int val;
	int prev_tid;

	val = atoi(cmd_str[1].c_str());
	Transaction* t = trans_stack.back();	
	
	// Look for val in present tid, if not , look for last updated val in val_tid_stack
	// if nothing anywhere, -1
	if (t->get_valMap_value(val) == -1) {
		if(!val_tid_stack[val].empty()) {

			prev_tid = val_tid_stack[val].back();
			return (trans_stack[prev_tid]->get_valMap_value(val));
		} else {
			return (-1);
		}
	} else
		return (t->get_valMap_value(val));	
}

void
process_unset(vector<string> cmd_str) {
	
	int val;
	int prev_tid, prev_val_tid;
	int prev_var_val_cnt, prev_var_val;
	
	Transaction* t = trans_stack.back();
	t->clear_value(cmd_str[1]);

	// if the variable was modified last in another transaction, add this transaction to the 
	// var_tid_stack
	if ( (!var_tid_stack[cmd_str[1]].empty()) && (t->get_tid() != var_tid_stack[cmd_str[1]].back())) {

		prev_tid = var_tid_stack[cmd_str[1]].back();
		prev_var_val = trans_stack[prev_tid]->get_value(cmd_str[1]);
		if (!val_tid_stack[prev_var_val].empty()) {
			prev_val_tid = val_tid_stack[prev_var_val].back();
		}
		prev_var_val_cnt = trans_stack[prev_val_tid]->get_valMap_value(prev_var_val);
		var_tid_stack[cmd_str[1]].push_back(t->get_tid());
		

		// we need a new label -9999 to indicate that the variable was unset
		// for process_get aka displaying purposes; use it to update current transaction
		t->add_value(cmd_str[1], -9999);
		
		//if value of var is 0, then look for previous tid count and add that to the present tid

		if (prev_var_val != -1) {
			//the prev var value has changed so add current tid to the val_tid_stack
			val_tid_stack[prev_var_val].push_back(t->get_tid());				
			t->set_valMap_value(prev_var_val, --prev_var_val_cnt);
		}

		
	}	

}

void print_var_tid_stack() {

	map<string, vector<int> >::iterator it;

	for(it = var_tid_stack.begin(); it != var_tid_stack.end(); it++) {

		
		for(int i=0; i < it->second.size(); i++) {
			cout<<"var key: "<<it->first<<" tid:"<<it->second[i]<<endl;
		}
		
	}
}

void print_val_tid_stack() {

	map<int, vector<int> >::iterator it;
	for(it = val_tid_stack.begin(); it != val_tid_stack.end(); it++) {

		
		for(int i=0; i < it->second.size(); i++) {
			cout<<"val key: "<<it->first<<" tid:"<<it->second[i]<<endl;
		}
		
	}
}

void
process_commit() {
	/*
	Loop through all the maps and add to new map in each transaction.
	Once that is done, what we have is the new final transaction.
	Erase trans_stack and add this new final transaction as tid 0
	*/
	vector<Transaction*>::iterator t_stack_it;
	map<string, int> tr_start_varMap;
	map<int,int> tr_start_valMap;
	Transaction *t = new Transaction(0);
	Transaction* tr_start;
	for (t_stack_it = trans_stack.begin(); t_stack_it != trans_stack.end(); ++t_stack_it) {
		tr_start = *t_stack_it;

		// get all varMap and valMapentries from tr_start to t
		tr_start_varMap = tr_start->get_varMap();
		//tr_start_valMap = tr_start->get_valMap();
		t->populate_vmaps(tr_start_varMap);
		
		//clear the copy buffers
		tr_start_valMap.erase(tr_start_valMap.begin(), tr_start_valMap.end());
		tr_start_varMap.erase(tr_start_varMap.begin(), tr_start_varMap.end());

	}

	trans_stack.erase(trans_stack.begin(), trans_stack.end());
	trans_stack.push_back(t);
}

int main() {

	int val;
	int tid = 0;
	string s;
	Transaction *t = new Transaction(tid);
	trans_stack.push_back(t);
	
	vector<string> cmd_vector;

	do {
		getline(cin,s);
		cout<<"input string:"<<s<<endl;
		cmd_vector = get_command(s);
		
		if(!validate_command(cmd_vector)) {			
			continue;
		}		
		
		if (cmd_vector[0] == "SET") {			
			process_set(cmd_vector);
			//print_var_tid_stack();
		} else if (cmd_vector[0] == "GET") {
			val = process_get(cmd_vector);
			if ((val == -9999) || (val == -1))
				cout<<"NULL"<<endl;
			else 
				cout<<val<<endl;
			
		} else if (cmd_vector[0] == "UNSET") {
			process_unset(cmd_vector);
		} else if (cmd_vector[0] == "BEGIN") {
			tid++;			
			t =  new Transaction(tid);

			cout<<"tid:"<<t->get_tid()<<endl;
			trans_stack.push_back(t);
		} else if(cmd_vector[0] == "ROLLBACK") {
			var_tid_stack_rollback();
			val_tid_stack_rollback();
			if(trans_stack.size() != 1) {
				trans_stack.pop_back();
				t = trans_stack.back();
			} else {
				cout<<"NO TRANSACTION"<<endl;
			}
		} else if (cmd_vector[0] == "NUMEQUALTO") {
			val = process_numequal(cmd_vector);
			if (val != -1)
				cout<<val<<endl;
			else
				cout<<"0"<<endl;
		} else if (cmd_vector[0] == "COMMIT") {
			process_commit();
		}
		
		cmd_vector.erase(cmd_vector.begin(), cmd_vector.end());

	}while(s != "END");
	
	return (0);
}
