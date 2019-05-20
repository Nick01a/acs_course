#include "ga_lib.h"


int main() {
    srand((unsigned)time(0));

    run_genetic_algorithm(8, 20, 3, 4, 150);

    return 0;
}
