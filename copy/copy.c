/*
* copy.c copies the content of one file to another
* Name: Sam Hoogewind
* Date: January 19, 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int copy(const char *src, const char *dest) {
    struct stat st; // create a stat structure http://codewiki.wikidot.com/c:system-calls:stat
    stat(src, &st);
    FILE *source_file, *destination_file;
    if (S_ISREG(st.st_mode)) { // check if the file is regular https://www.linuxquestions.org/questions/programming-9/how-to-know-if-it%27s-regular-file-in-c-217832/
        if (access(dest, F_OK) != -1) { // check if the file exists, if it does print an error https://linux.die.net/man/2/access{
            fprintf(stderr, "Error: This destination file already exists"); // https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
            return -1;
        }
        else {
            source_file = fopen(src, "r"); // open the files https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
            if (source_file == NULL) { // catch any errors https://en.cppreference.com/w/c/io/perror
                perror("fopen() failed");
                return -1;
            }
            
            destination_file = fopen(dest, "w");
            
            char c = fgetc(source_file); // read the file in, then put it into the destination file https://stackoverflow.com/questions/4823177/reading-a-file-character-by-character-in-c
            while (c!= EOF) {
                fputc(c, destination_file); // https://www.tutorialspoint.com/c_standard_library/c_function_fputc.htm
                c = fgetc(source_file); // https://www.tutorialspoint.com/c_standard_library/c_function_fgetc.htm
            }
            
            fclose(source_file); // close the files
            fclose(destination_file);

            return 0;

       }
    }
    else { // otherwise the file is not regular, so print an error and return -1
        fprintf(stderr, "Error: This is not a regular file"); // https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
        return -1;
    }

}

int main(int argc, char **argv) {
    copy(argv[1], argv[2]); // pass copy the arguments from the shell https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
}
