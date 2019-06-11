/**
 * A main file to facilitate testing/grading of HW
 * 
 * Copyright (C) 2019 sorkinmp@miamiOH.edu
 */

#include <string>
#include "Server.h"

int main(int argc, char *argv[]) {
    Server httpd;
    if (argc > 1) {
        httpd.runServer();
    } else {
        // Process 1 request from cin/cout for functional testing
        httpd.serveClient(std::cin, std::cout);
    }
    return 0;
}
