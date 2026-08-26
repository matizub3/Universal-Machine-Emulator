**************************************************************
* README
*
* Assignment: Universal Machine (HW6)
* Authors: Mateusz Zubrzak (mzubrz01), Niam Lakhani (nlakha01)
* Date: 04/12/25
*
**************************************************************


1. AUTHORS

- Mateusz Zubrzak (mzubrz01)
- Niam Lakhani 

2. ACKNOWLEDGEMENTS

We had the help of the TAs, Darya Bella, Moizes with debugging 
an assert that was failing on midmark.um

3. IMPLEMENTATION REPORT
Everything has been implemented correctly

4. DESIGN CHANGES
We changed our unmapped segment ID structure from a Seq_T to a
Stack_T. We also added a register module which serves as a modular approach
to updating and storing register values, applicable to various types of
projects.

5. SYSTEM ARCHITECTURE
um.c:

- Initializes the machine state, loads input, starts execution.
- Functions called to decode .um binary and set up the memory
and registers.

The um module serves as the main driver of the um program. It reads in the
provided file, initializes segment 0, creates the UM registers, and
UM memory module. um.c implements the MachineState struct which holds 
the registers, memory, and program counter of the UM machine. This MachineState
struct is shared with execute.c.


execute.c:
- Executes the UM instructions in a loop.
- Calls functionality from machine state and controls register and
segment behavior

The execute module serves as the main driver of the um program. It reads in the
provided file, initializes segment 0, creates the UM registers, and
UM memory module. um.c implements the MachineState struct which holds 
the registers, memory, and program counter of the UM machine. This MachineState
struct is shared with execute.c.

registers.c:
- The registers.c file handles the machines eight registers, stores them
as an array of 32-bit integers.
- Creates, frees, gets, and sets registers.

The registers module uses a C array which is kept a secret to all other modules.

memoryLoader.c:
- Implements segments using Seq_T of Segment structs.
- Does mapping, unmapping, accessing, and updating segments.
- Unmaps segment IDs using a Stack_T

Uses a memoryLoader struct to hold the Sequence of sequences which represent
the segmented memory. The memoryLoader struct is known by the um and execute
module. Since the memoryLoader is a public module which is applicable to other
projects and thus is intended for other users, the struct definitions are public
to all.

decode.c:
- Decodes the 32-bit words into Um_Instruction struct.
- Uses bitpacking

Uses an Um_Instruction struct which stores the opcode and regA, B, C values.
The Um_Instruction struct is shared with the execute and um modules.




6. 50 MIL EXECUTION TIME

  The amount of time it takes for 50 million instructions is 5.983 seconds.

  We altered our um program to count the number of instructions and timed the 
  execution of midmark. We then used the ratio to calculate the time for
  50 million instructions.

7. UNIT TESTING


- halt.um:
  Simple test to test that the halt instruction stops the program.


  - load-hello-test.um:
  Loads characters into all 8 registers and prints them. Outputs: Hello!!!.

- output.um:
  Outputs a character to verify the output instruction works.

- input-test.um:
  Tests the input command by expecting an input of Hello on stdin with each
  register storing one character and outputting Hello by outputting every
  register

  halt-verbose.um
  Halt test which combines loading values, outputing them, and halting.

- add.um:
  Adds two small numbers and outputs the result to check addition.

  - add-overflow.um:
  Verifies that addition wraps correctly.

  - multiply.um:
  Multiplies two values and checks the result.

  - divide.um:
  Divides a value and checks for correct result.

  - mult-overflow.um:
  Tests that multiplication wraps around at 2^32. 65536 * 65536 = 0 mod 2^32. 
  Output: 0.

  - conditional-move.um:
  Tests instruction 0. If the condition is nonzero, it should move the value
  to the correct register.

- cmov-both-cases.um:
  There are two versions of cmov: one where the condition is false then
  no move and one where it's true and the move happens.

  - nand.um:
  Verifies bitwise nand logic: ~(0 & 0) = 0xFFFFFFFF and output should be 0.

- segmented-load-test1.um:
  Maps a segment and confirms that the default word value is 0. Should output
  0.

 - segmented-load-test2.um:
  Combines sstore and sload. Stores a value in memory and loads it back to 
  see if it is correct.

  - map-segment.um:
  Maps a segment, stores a value in it, loads it back, and outputs it.

  - map-then-unmap.um:
  Tests map, unmap, and reuse a memory segment ID without errors.
  

  - segmented-store-test.um:
  Stores a value into segment 0, loads it back, and outputs it to confirm.

  - load-program.um:
  Tests loading a new program into segment 0 and doing the execution. Confirms 
  segment replacement and offset works.

  - calculate-time.um:
  Loads a value repeatedly to mimick high instruction count for timings
  
  Abstraction and Secrets:
  Each modules hide how it works, eg The Registers module hides the arrya it 
  uses and memoryLoader hides how segments are stored.
  
  The execute module understands how the instructions instructions work 
  but calls other modules such as memory loader to actually execute the  
8. TIME SPENT
- Analyzing the assignment: 5 hours
- Preparing the design: 7 hours
- Implementing/debugging the UM: 25 hours