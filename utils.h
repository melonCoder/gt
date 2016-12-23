#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>

using namespace std;

void sys_err(const char* err);

void sys_err(const string& s);

void log(const string& s);

bool end_with_str(string const& from, string const& to);

bool start_with_str(const string& from, const string& to);

string get_prefix_path(const string& path);

string vectorToPath(const vector<string>* const pathVector);

#endif
