#include "um.h"

#define UM_FILE_COMMANDLINE_INDEX 1

int main(int argc, char *argv[])
{
    assert(argc == NUM_OF_COMMANDS);

    FILE *input = fopen(argv[UM_FILE_COMMANDLINE_INDEX], "rb");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return EXIT_FAILURE;
    }

    MachineState ms = intialize_machine_state(input);
    fclose(input);

    run_um(ms);  

    return EXIT_SUCCESS; 
}