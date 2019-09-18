
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

struct ids {/*stack*/
	string id;
	string c;
	ids *prev;
};

struct list_t {
	string country;
	ids *id;
	list_t *next;
};
//if its new node
list_t *add(list_t *last, string country, string id, string c) {
	list_t *foradd;
	
	foradd = new list_t;
	
	foradd->country = country;
	ids *put = new ids;
	put->id = id;
	put->c = c;
	put->prev = NULL;

	foradd->id = put;
	foradd->next = NULL;
	last->next = foradd;
	return foradd;
}

list_t* search_in_list(list_t* tl, string cname) {
	list_t *curr = tl;
	while (curr != NULL) {
		if (curr->country == cname) { 
	//		cout << "The such node has been found: " << curr->country << endl;
			return curr; 
		}
		curr = curr->next;
	}
	return NULL;
}

void add_id(list_t *where, string id, string c) {
	ids *top = where->id, *put;
	put = new ids;
	
	put->id = id;
	put->c = c;
	put->prev = top;
	where->id = put;

	auto curr = where->id;
//	cout << "List of ids:" << endl;
	while (curr != NULL) {
		//cout << "id: " << curr->id << endl;
		curr = curr->prev;
	}
}

void devastate(list_t *head) {
	list_t *curr = head;
	if (curr == NULL) {
		cout << "Devastate: list is empty\n";
		return;
	}
	while (curr != NULL) {

		//cout << "Current node is " << curr->country << endl;

		ids* get_top = curr->id;
		while (get_top != NULL) {
			//cout << "ids are " << get_top->id << endl;
			auto next = get_top->prev;
			delete get_top;
			get_top = next;
		}
		auto next = curr->next;
		delete curr;
		curr = next;
	}

}

void stat(list_t *head) {
	list_t *curr = head;
	int unique = 0, sum_count = 0;
	while (curr != NULL) {
		cout << curr->country << " ";

		ids* get_top = curr->id;
		while (get_top != NULL) {
			sum_count += atoi(get_top->c.c_str());
			unique += atoi(get_top->c.c_str()) == 1;
			get_top = get_top->prev;
		}
		cout << sum_count << " " << unique << endl;
		sum_count = unique = 0;
		curr = curr->next;
	}
}

int main() {
	list_t *main = NULL; /*Overlapped name*/
	list_t *last = main; /*For now last is head*/

	string input;
	while (getline(cin, input)) {
		if (!input.size()) {
			break;
		} 
		auto it = input.begin();
		auto jt = it;
		for (; *it != ';'; it++);
		string id_s(jt, it++);
		jt = it;
		for (; *it != ';'; it++);
		string count_s(jt, it);
		string country(it + 1, input.end());

	//	cout << "input: " << country << " " << id_s << " " << count_s << endl;

		if (main == NULL) {
			main = new list_t;
			main->country.clear();
			main->id = NULL;
			main->next = NULL;


			main->country = country;

			ids *put = new ids;
			put->id = id_s;
			put->c = count_s;
			put->prev = NULL;

			main->id = put;
		
			last = main;
			continue;
		}

		list_t *get_the_entry = search_in_list(main, country);
		if (get_the_entry == NULL) { /*Node with the country name havn't been created yet!*/
			last = add(last, country, id_s, count_s);
		}
		else { /*just add ids*/
			add_id(get_the_entry, id_s, count_s);
		}
	}


	stat(main);

	devastate(main);
	return 0;
}
