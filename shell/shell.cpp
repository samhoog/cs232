//
//  shell.cpp
//  CS232 Command Shell
//
//  Created by Sam Hoogewind on 3/28/22.

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include "shell.h"
#include "path.h"
#include "prompt.h"
#include "commandline.h"

Shell::Shell()
{

}

void Shell::run()
{
    // loop forever
    while (1) {
        // display the prompt
        Prompt p;
        cout << p.get();
        
        // instigate the commandline
        CommandLine* cl = new CommandLine(cin);

        // get the command argument if it is valid
        if (cl->getCommand() != NULL) { 
            string command = cl->getCommand();

            // handle cd command
            if (command == "cd") {
                // change the directory
                int newDirectory = chdir(cl->getArgVector(1)); // https://man7.org/linux/man-pages/man2/chdir.2.html
                // if that changed directory is valid
                if (newDirectory != -1) {
                    // display the new directory
                    Prompt myPrompt;
                } else {
                    // otherwise display an error
                    cout << "No such file or directory" << endl;
                }
            }
            
            // handle pwd command
            else if (command == "pwd") {
                // print out the current directory
                cout << get_current_dir_name() << endl; // https://man7.org/linux/man-pages/man3/getcwd.3.html
            
            }
             
            // handle exit command
            else if (command == "exit") {
                exit(0);
        
            } 

            // handle every other command
            else {
                // get the index of the directory
                int index = myPath.find(command);
                if (index == -1 ) {
                    cout << "Command not found." << endl;
                }
                else {
                    // create the child pid
                    pid_t childpid = fork(); // https://man7.org/linux/man-pages/man2/fork.2.html

                    // if you are the child
                    if (childpid == 0) {
                        // get the directory of the command
                        string directory = myPath.getDirectory(index) + "/" + command;
                        // run that command
                        execve(directory.c_str(), cl->getArgVector(), NULL); // https://www.tutorialspoint.com/unix_system_calls/execve.htm
                        // exit
                        exit(1);
                    
                    // if you are the parent
                    } else {
                        // if there isn't an ampersand
                        if (cl->noAmpersand()) {
                            // wait for the child
                            waitpid(childpid, NULL, 0); // https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end
                        } else {
                            // do nothing and return to the top of the loop
                        }
                    }
                }
            }
        }
        // delete the CommandLine
        delete cl;
    }
}

