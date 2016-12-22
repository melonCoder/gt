#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <cstdlib>
#include <dirent.h>
#include <string>
#include <vector>

#include <iostream>
using namespace std;

static bool DEBUG = false;
static char path[MAXPATHLEN];
static unsigned int max_depth;

void sys_err(const char* err) {
    if(!DEBUG) return;
    cout << "Error: " << err << endl;
}

void sys_err(const string s){
    sys_err(s.c_str());
}

void log(const string s){
    if(!DEBUG) return;

    cout << "Log: " << s << endl; 
}

bool end_with_str(const string from, const string to) {
    int lenFrom = from.length();
    int lenTo = to.length();
    if(lenTo == 0 || lenFrom < lenTo) return false;

    for(int i_ = 1; i_ <= lenTo; ++i_){
        if(from[lenFrom - i_] != to[lenTo - i_]) return false;
    }
    return true;
}

bool start_with_str(const string from, const string to) {
    int lenFrom = from.length();
    int lenTo = to.length();
    if (lenFrom < lenTo) return false;

    for (int i_ = 0; i_ < lenTo; ++i_) {
        if(from[i_] != to[i_]) return false;
    }
    return true;
}

string get_last_path(const string& path){
    int idx = path.find_last_of('/');
    return path.substr(idx + 1);
}

string get_path(const string& path) {
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

void goto_dir(vector<string>* const matchedPath, const string destDir, 
        vector<string>* pathVector) {
    DIR *dp;
    struct dirent* dirp;
    struct stat    statBuf;

    string cwd = vectorToPath(pathVector);

    if (end_with_str(cwd, destDir)){
        matchedPath->push_back(cwd);
    }

    if (pathVector->size() >= max_depth) return;

    const char *cwd_tmp = cwd.c_str();
    if (lstat(cwd_tmp, &statBuf) < 0){
        sys_err("failed to stat " + cwd);
    } 

    // first process lnk
    string cwd_link = cwd;
    while(S_ISLNK(statBuf.st_mode) != 0){
        ssize_t len = readlink(cwd_link.c_str(), path, sizeof(path) - 1);
        path[len] = '\0';
        if (len == -1){
            sys_err("Read link " + cwd + "failed");
        }

        // readlink return abs path if links to other partitions
        // otherwise, it returns relative path
        // TODO: link path includes the link devices, it should be removed
        // before getting the real path
        string realPath;
        if (path[0] == '/') {
            realPath = path;
        } else {
            realPath = get_path(cwd) + path;
        }
        if (lstat(realPath.c_str(), &statBuf) < 0) {
            sys_err(string("failed to stat ") + realPath);
            break;
        }
        if (S_ISDIR(statBuf.st_mode) == 0) {
            log(string("read leaf link ") + cwd);
            return;
        }
    }

    // reaches non-dir nodes, treat as leaf and leave
    if (S_ISDIR(statBuf.st_mode) == 0){
        log("read leaf " + cwd);
        return;
    }

    if(!(dp = opendir(cwd.c_str()))) {
        sys_err("failed to open " + cwd);
        return;
    }
    
    while((dirp = readdir(dp))) {
        log(string("reading dir ") + dirp->d_name);
        if("." == string(dirp->d_name) || ".." == string(dirp->d_name))
            continue;
        // temporary not support hidden path/file
        if('.' == *(dirp->d_name))
            continue;
        pathVector->push_back(dirp->d_name);
        goto_dir(matchedPath, destDir, pathVector);
        pathVector->pop_back();
    }
}

void usage() {
    cout << "Usage:" << endl;
    cout << "    gt [DIR]" << endl;
}

int main(int argc, char* argv[]){
    string destDir;
    if(argc == 1) {
        sys_err("Not enough arguments");
        usage();
        exit(1);
    } else if(argc == 2){
        if(string(argv[1]).length() <= 1) {
            sys_err("Argument not valid");
            usage();
            exit(1);
        }

        destDir = argv[1];
        log(destDir);
    } else if(argc >= 3){
        if(start_with_str(argv[1], "-max-depth=")) {
            max_depth = stoi(*(argv + 1) + 11);
        }
        destDir = argv[2];
    }
    // DEBUG enabled?
    if (string("-D") == argv[argc - 1]) {
        DEBUG = true;
    }

    // this part should be opt
    if(!getcwd(path, MAXPATHLEN)) {
        sys_err("getcwd failed");
        exit(1);
    }

    vector<string> matchedPath;
    vector<string> curPath;
    curPath.push_back(string(path));
    goto_dir(&matchedPath, destDir, &curPath);

    string delmeter = "";
    if (matchedPath.size() > 1){
        delmeter = "\t";
        cout << "There are more than one candidates." << endl;
    }
    for(vector<string>::iterator it = matchedPath.begin(); it != matchedPath.end();
            ++it){
        cout << delmeter << *it << endl;
    }
}
