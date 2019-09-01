#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

/*This program is the solve of 
 https://www.hackerrank.com/challenges/attribute-parser/problem
 
 Program gets 4/6 success cases, may be because I don't make additional checks that tags corresond to each other
 
 The program uses a finite machine*/


using namespace std;

#define ERROR cout << "finite machine has gotten wrong state!\n"; exit(1);

string analize(string str) {
    istringstream proc(str);
    string result, temp;
    proc >> temp;
    result += temp + "~";
    while(proc >> temp)
        result += temp;
    return result;
}

string take_out_tag(string str) {
    istringstream proc(str);
    string result;
    proc >> result;
    return result;
}

int main() {
    int N, Q; 
    string first;

    getline(cin, first);
    istringstream in(first);
    in >> N;
    in >> Q;
    
    if (N > 20 || Q > 20) exit(1);

    string input;
    vector <string> q;

    while (N--) {
        string s;
        getline(cin, s);
        if (s.size() > 200) exit(1);
        input += s;
    }

    while (Q--) {
        string s;
        getline(cin, s);
        if (s.size() > 200) exit(1);
        q.push_back(s);
    }

    char fm_state = '0';
    string current_state;
    string created_node;
    vector <string> nodes;
    bool closed = 0;
    string what_find;
    string::iterator it;
    string::reverse_iterator itt;
    string taken;
    for (auto i : input) {
        
        switch (fm_state) {
        case '0':
            switch (i) {
            case '<':
                fm_state = '<';
                continue;
            default:
                ERROR
            }
        case '<':
            switch (i) {
            case '>':
                current_state.empty() ? nodes.push_back(analize(created_node)) : nodes.push_back(current_state + analize(created_node));
                taken = take_out_tag(created_node);
                current_state += taken + ".";
                created_node.clear();
                fm_state = '>';
                continue;
            case '/':
                current_state.pop_back();
                itt = current_state.rbegin();
                while (*itt != '.') {
                    current_state.pop_back(); itt++;
                    if (current_state.empty()) break;
                }
                fm_state = '/';
                continue;
            default:
                created_node += i;
                continue;

            }
        case '>':
            switch (i) {
            case '<':
                fm_state = '<';
                continue;
            default:
                ERROR
            }
        case '/':
            switch (i) {
            case '>':
                fm_state = '>';
                continue;
            case '/' || '<':
                ERROR
            default: continue;
            }
        }
    }



    for (auto what_find : q) {
        bool found = false;
        auto k = find(what_find.begin(), what_find.end(), '~');
        string tag_value(k + 1, what_find.end());
        string the_such_tag(what_find.begin(), k);
        the_such_tag += "~";

        for (auto where_find : nodes) {
            /*find the tag*/
            auto it = search(where_find.begin(), where_find.end(), the_such_tag.begin(), the_such_tag.end());
            if (it == where_find.end()) continue;
            if (it == where_find.begin()) {

                auto iter = search(where_find.begin(), where_find.end(), tag_value.begin(), tag_value.end());
                if (iter != where_find.end()) {
                    while (*iter++ != '"');
                    while (*iter != '"') {
                        cout << *iter; iter++;
                    }
                    found = true;
                    cout << endl;
                }

            }
        }
        if (!found) cout << "Not Found!\n";

    }
}
