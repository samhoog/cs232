//
//  shell.h
//  CS232 Command Shell
//
//  Created by Sam Hoogewind on 3/28/22.

#ifndef __CS232_Command_Shell__shell__
#define __CS232_Command_Shell__shell__

#include <iostream>
#include <string>
#include "commandline.h"
#include "path.h"
using namespace std;

class Shell
{
public:
    Shell();
    void run();
private:
    Path myPath;
};

#endif /* defined(__CS232_Command_Shell__shell__) */
