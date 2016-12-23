#include "utils.h"
#include "parameter.h"
#include <iostream>

using namespace std;

void sys_err(const char* err) {
    if(!DEBUG) return;
    cout << "Error: " << err << endl;
}

void sys_err(const string& s){
    sys_err(s.c_str());
}

void log(const string& s){
    if(!DEBUG) return;

    cout << "Log: " << s << endl;
}

bool contain_str_at(const string& from, const string& to, size_t pos, 
        bool reverse = false) {
    size_t len_from = from.length();
    size_t len_to = to.length();
    if ((!reverse && (len_from - pos < len_to)) || 
            (reverse && (pos < len_to))) {
        return false;
    }

    // return to == from.substr(reverse ? pos - to.length() : pos, 
    //                          reverse ? pos - 1 : pos + to.length() - 1);
    size_t start_point = reverse ? pos - len_to : pos;
    for (size_t i_ = 0; i_ < len_to; ++i_) {
        if (from[start_point + i_] != to[i_]) return false;
    }
    return true;
}

bool end_with_str(const string& from, const string& to) {
    return contain_str_at(from, to, from.length(), true);
}

bool start_with_str(const string& from, const string& to) {
    return contain_str_at(from, to, 0);
}

string get_prefix_path(const string& path) {
    int idx = path.find_last_of('/');
    if (idx == -1) return "";
    else return path.substr(0, idx + 1);
}

string vectorToPath(const vector<string>* const pathVector) {
    vector<string>::const_iterator it = pathVector->begin();
    string cwd = *it;
    it++;
    for(; it != pathVector->end(); it++) {
        cwd += "/";
        cwd += *it;
    }
    return cwd;
}
