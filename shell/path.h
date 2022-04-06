//
//  path.h
//  CS232 Command Shell
//
//  Created by Sam Hoogewind on 3/28/22.

#ifndef __CS232_Command_Shell__path__
#define __CS232_Command_Shell__path__

#include <iostream>
#include <vector>
using namespace std;

class Path
{
public:
    Path();
    int find (const string& program) const;
    string getDirectory(int i) const;
private:
    vector<string> directories{};
};

#endif /* defined(__CS232_Command_Shell__path__) */
