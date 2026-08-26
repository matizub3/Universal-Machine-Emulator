/**************************************************************
 *
 *                        um.c
 *
 *     Assignment: um
 *     Authors:    Mateusz (mzubrz01) & Niam(nlakha01)
 *     Date:       04/12/25
 *
 *     Purpose:
 *         The implemenation for the main Universal Machine execution setup, 
 *         including initialization,instruction decoding, and I/O functionality.
 *
 **************************************************************/

#include "execute.h"


#define UM_FILE_COMMANDLINE_INDEX 1


#define NUM_OF_REGISTERS 8
#define NUM_OF_COMMANDS 2


#define PROGRAM_SEGMENT_ID 0

#define REG_WIDTH 3
#define OP_WIDTH 4

#define REG_A_LSB 6
#define REG_B_LSB 3
#define REG_C_LSB 0


/********** MachineState **********
 *
 * Represents the structure of a Universal Machine
 *
 *
 * Registers regs:   Registers struct to hold register values of UM
 * MemoryLoader mem: MemoryLoader struct to hold memory of UM
 * programCounter:   Stores the index of the current program being executed
 * numOfPrograms:    Stores the number of programs 
 *
 */
struct MachineState {

        Registers regs;
        MemoryLoader mem;
        uint32_t programCounter;
        uint32_t numOfPrograms;
};

/********** Um_Instruction **********
 *
 * Represents a single Universal Machine instruction.
 *
 * opcode:  The operation code of the instruction (0-13)
 * regA:    Holds value for register A
 * regB:    Holds value for register B
 * regC:    Holds value for register C 
 * value:   Holds value for instruction load value
 *
 */
struct Um_Instruction{

        uint32_t opcode;
        uint32_t regA;
        uint32_t regB;
        uint32_t regC;
        uint32_t value;
};


/********** main **********
 *
 * Entry point for the Universal Machine emulator.
 *
 * Parameters:
 *      int argc:
 *          The number of command-line arguments
 *      char *argv[]:
 *          An array of command-line argument strings
 *
 * Returns:
 *          EXIT_SUCCESS if the emulator runs successfully
 *          EXIT_FAILURE if an error occurs
 *
 *
 * Expects:
 *      Only two command line arguments are passed in
 *      File provided is a valid UM file
 *
 ************************/
int main(int argc, char *argv[])
{
        assert(argc == NUM_OF_COMMANDS);

        /* Check if the file is provided */
        FILE *input = fopen(argv[UM_FILE_COMMANDLINE_INDEX], "rb");

        if (input == NULL) {
                fprintf(stderr, "Error opening input file\n");
                return EXIT_FAILURE;
        }

        /* Initialize the machine state with file data and map segment 0 */
        MachineState ms = intialize_machine_state(input);

        /* Close the provided UM file */
        fclose(input);

        /* Run the UM emulator */
        run_um(&ms);

        /* Free the machine state struct */
        freeMachineState(ms);

        return EXIT_SUCCESS; 
}


/********** intialize_machine_state **********
 *
 * Initializes a MachineState by loading instructions from a file,
 * fills segment 0 with program words, and setting the
 * program counter to 0
 *
 * Parameters:
 *      FILE *input:
 *          A binary file pointer to the UM program (.um)
 *
 * Return:
 *      MachineState:
 *          A fully initialized machine state
 *
 * Expects:
 *      - input is not NULL 
 *      - Each word in the file is 32 bits 4.
 ************************/
MachineState intialize_machine_state(FILE *input)
{
    
        MachineState ms;

        /* Create registers for machine state */
        ms.regs = registers_create(NUM_OF_REGISTERS);

        int num_of_commands = 0;
        uint32_t command_input;
        ms.programCounter = 0;


        /* Create segmented memory for machine state */
        ms.mem = newMemory(0, num_of_commands);


        /*Map segment 0 and convert to endianess*/
        while (fread(&command_input, sizeof(uint32_t), 1, input) == 1) {

                uint32_t converted_input =  ((command_input & 0xFFU) << 24) |
                                        ((command_input & 0xFF00U) << 8) |
                                        ((command_input & 0xFF0000U) >> 8) |
                                        ((command_input & 0xFF000000U) >> 24);

                add(ms.mem, PROGRAM_SEGMENT_ID, converted_input);
                num_of_commands++;
        }

        /*Set the number of programs*/
        ms.numOfPrograms = num_of_commands;

        /* Return initialized machine state */
        return ms;
}


/********** decode_instruct **********
 *
 * Gets and decodes a 32-bit instruction from segment 0
 * at the given program counter
 *
 * Parameters:
 *      MachineState ms:
 *          The current machine state struct with memory and registers
 *
 *      int programCounter:
 *          The index in segment 0 to decode the instruction from
 *
 * Return:
 *      Um_Instruction:
 *          The struct of the decoded instruction in the given registers
 *
 * Expects:
 *      - programCounter is within the bounds of segment 0
 ************************/
Um_Instruction decode_instruct(MachineState ms, int programCounter)
{
        /* Retrieve instruction from segment 0 at program counter index */
        uint32_t *instruction = getWord(ms.mem, PROGRAM_SEGMENT_ID, 
                                        programCounter);
                                        
        uint32_t instruct = *instruction;

        
        /* Decode instructions using Bitpack to extract opcode and registers */
        Um_Instruction um_instruct;
        um_instruct.opcode = Bitpack_getu(instruct, OP_WIDTH, 28);

        if (um_instruct.opcode == 13) {

                um_instruct.regA = Bitpack_getu(instruct, REG_WIDTH, 25);
                um_instruct.value = Bitpack_getu(instruct, 25, 0);

        } else {

                um_instruct.regA = Bitpack_getu(instruct, REG_WIDTH, REG_A_LSB);
                um_instruct.regB = Bitpack_getu(instruct, REG_WIDTH, REG_B_LSB);
                um_instruct.regC = Bitpack_getu(instruct, REG_WIDTH, REG_C_LSB);

        }

        return um_instruct;
}


/********** IO_print **********
 *
 * Outputs a value from register c as a character to stdout
 *
 * Parameters:
 *      uint32_t value:
 *          A value between 0 and 255 to print as an ASCII character
 *
 * Return:
 *      None
 *
 * Expects:
 *      value =< 255
 ************************/
void IO_print(uint32_t value)
{
        assert(value <= 255);

        int result = putchar((char)(uint32_t)value);

        assert(result != EOF);
}


/********** IO_input **********
 *
 * Reads a character from standard input and returns its ASCII value.
 *
 * Return:
 *      uint32_t:
 *          The character read, or 0xFFFFFFFF if EOF occurs.
 *
 * Expects:
 *      - No errors happen while reading from stdin.
 ************************/
uint32_t IO_input()
{
        int readInChar = getchar();

        if (readInChar == EOF) {
                assert(!ferror(stdin));
                return 0xFFFFFFFF;
        }

        return (uint32_t)readInChar;
}


/********** freeMachineState **********
 *
 * Frees all of MachineState memory including registers and memory segments.
 *
 * Parameters:
 *      MachineState ms:
 *          The machine state struct to clean up.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - ms contains valid allocated components.
 ************************/
void freeMachineState(MachineState ms)
{
        registers_free(ms.regs);

        freeMemory(ms.mem);
}
