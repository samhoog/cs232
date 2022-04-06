//
//  path.cpp
//  CS232 Command Shell
//
//  Created by Sam Hoogewind on 3/28/22.

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <dirent.h>
#include <libgen.h>
#include <cstring>
#include "path.h"

Path::Path()
{
    // get the PATH variable path
    string path = getenv("PATH"); // https://www.cplusplus.com/reference/cstdlib/getenv/
    
    // we want to split along the colons
    char delim = ':';

    // put the path in a stream
    stringstream sstream(path);
    string word;
    
    // getline from that stream and split it by the delim
    while (getline(sstream, word, delim)) {// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
        // add each directory path to the vector
        directories.push_back(word);
    }

    // printing each path for testing
    // for (const auto &directory : directories) {
    //     cout << directory << endl;
    // }
}

int Path::find (const string& program) const
{
    // instigate directory variables
    DIR *dir;
    struct dirent *ent; // http://www.cplusplus.com/forum/beginner/267857/

    // loop through all the items in the vector
    for (unsigned i = 0; i < directories.size(); i++) {
        
        // if the directory can be opened
        if (dir = opendir (directories.at(i).c_str())) {
            
            // while you can read items in that directory
            while ((ent = readdir (dir)) != NULL) { // https://cpp.hotexamples.com/examples/-/-/readdir/cpp-readdir-function-examples.html
                
                // if a name in the directory is the name you're looking for
                if ((ent->d_name) == program) { 
                    return i; // return the index
                }
            }
            closedir (dir); // close the directory
        }
    }
    return -1; // the program couldn't be found, so return -1
}

string Path::getDirectory (int i) const
{
    // check if given index is valid
    if (i > directories.size() || i < 0) {
        return "Given index isn't valid";
    }
    else {
        // if it is, return the directory at that index
        return directories.at(i);
    }
}