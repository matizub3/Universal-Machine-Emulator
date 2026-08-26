/**************************************************************
 *
 *                     registers.h
 *
 *     Date:       04/12/25
 *
 *
 *         The interface for the register module for the Universal Machine.
 *         Provides functions to create, access, update, and free
 *         a set of eight 32-bit registers.
 *
 **************************************************************/


#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "assert.h"
#include "mem.h"

typedef struct Registers *Registers;

Registers registers_create(int numRegisters);
void registers_set(Registers regs, int regIndex, uint32_t value);
uint32_t registers_get(Registers regs, int regIndex);
void registers_free(Registers regs);

#endif
