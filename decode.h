/**************************************************************
 *
 *                     DECODE.H
 *
 *     Date:       04/12/25
 *
 *     Purpose:
 *         Defines the data structures and interface for decoding 
 *         32-bit UM instructions into their components (opcode, 
 *         registers, and value).
 *
 **************************************************************/
#ifndef DECODE_H
#define DECODE_H

#include "um.h"


typedef struct Um_Instruction {
    int opcode;
    uint32_t regA;
    uint32_t regB;
    uint32_t regC;
    uint32_t value; /*for op code 13*/
} Um_Instruction;


Um_Instruction decode_instruction(MachineState ms, int programCounter);

#endif










