#include "ackerman.h"
#include "my_allocator.h"
#include <stdlib.h>
#include <iostream>
//the following include statements are actually
//required on Linux. Do not remove them!!
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>


void exit_fn() {
    cout << "\nCalled release allocator.\n\n";
    release_allocator();
}

void signalHandler(int signum) {
    exit(signum);

}

int main(int argc, char **argv) {

    // input parameters (basic block size, memory length)
    int b = 128; // smallest block size in bytes, default 128
    int M = 8 * 1024 * 1024 * 16;// + 32 + 128; // size of memory to allocate in bytes, default 512 kB

    int c;
    while ((c = getopt(argc, argv, "b:s:")) != -1)
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 's':
                M = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Invalid command-line argument. Exiting program");
                return 1;
            default:
                abort();
        }
    init_allocator(b, M);

    atexit(exit_fn);
    signal(SIGINT, signalHandler);

    //the following block was used for ease of analysis
//    for (int n = 1; n <= 3; ++n) {
//        for (int m = 1; m <= 8; ++m) {
//            ackerman_main_testing(n, m);
//        }
//    }

    ackerman_main();

    //release allocator handled by atexit().
    return 0;
}