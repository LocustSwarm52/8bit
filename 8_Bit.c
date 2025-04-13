#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE 256
#define STACK_SIZE 16

// Instruction set
#define LDA 0x01  // Load accumulator from memory
#define STA 0x02  // Store accumulator to memory
#define LDB 0x03  // Load B register from memory
#define STB 0x04  // Store B register to memory
#define ADD 0x05  // Add B to accumulator
#define SUB 0x06  // Subtract B from accumulator
#define JMP 0x07  // Jump to address
#define JZ  0x08  // Jump if zero
#define HLT 0xFF  // Halt execution

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t accumulator;
    uint8_t b_register;
    uint8_t program_counter;
    uint8_t stack[STACK_SIZE];
    uint8_t stack_pointer;
    uint8_t zero_flag;
    uint8_t running;
} Computer;

void init_computer(Computer *c) {
    memset(c->memory, 0, MEMORY_SIZE);
    c->accumulator = 0;
    c->b_register = 0;
    c->program_counter = 0;
    c->stack_pointer = 0;
    c->zero_flag = 0;
    c->running = 1;
}

void execute_instruction(Computer *c, FILE *file) {
    uint8_t opcode = c->memory[c->program_counter];
    uint8_t operand = c->memory[c->program_counter + 1];
    
    switch(opcode) {
        case LDA:
            c->accumulator = c->memory[operand];
            c->program_counter += 2;
            fprintf(file, "LDA 0x%02X: Accumulator = 0x%02X\n", operand, c->accumulator);
            break;
            
        case STA:
            c->memory[operand] = c->accumulator;
            c->program_counter += 2;
            fprintf(file, "STA 0x%02X: Memory[0x%02X] = 0x%02X\n", operand, operand, c->accumulator);
            break;
            
        case LDB:
            c->b_register = c->memory[operand];
            c->program_counter += 2;
            fprintf(file, "LDB 0x%02X: B register = 0x%02X\n", operand, c->b_register);
            break;
            
        case STB:
            c->memory[operand] = c->b_register;
            c->program_counter += 2;
            fprintf(file, "STB 0x%02X: Memory[0x%02X] = 0x%02X\n", operand, operand, c->b_register);
            break;
            
        case ADD:
            c->accumulator += c->b_register;
            c->zero_flag = (c->accumulator == 0);
            c->program_counter++;  // ADD is a single-byte instruction
            fprintf(file, "ADD: Accumulator = 0x%02X\n", c->accumulator);
            break;
            
        case JMP:
            c->program_counter = operand;
            fprintf(file, "JMP to 0x%02X\n", operand);
            break;
            
        case JZ:
            if(c->zero_flag) {
                c->program_counter = operand;
                fprintf(file, "JZ: Jump to 0x%02X (zero flag set)\n", operand);
            } else {
                c->program_counter += 2;
                fprintf(file, "JZ: No jump (zero flag not set)\n");
            }
            break;
            
        case HLT:
            c->running = 0;
            fprintf(file, "HLT: Halt execution\n");
            break;
            
        default:
            fprintf(file, "Unknown opcode: 0x%02X\n", opcode);
            c->running = 0;
    }
}

void run_computer(Computer *c, FILE *file) {
    while(c->running) {
        execute_instruction(c, file);
    }
}

int main() {
    Computer computer;
    init_computer(&computer);
    
    // Open the file for writing
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    
    // Example program (no PRINT):
    uint8_t program[] = {
        LDA, 0x30,
        HLT,
    };
    
    memcpy(computer.memory, program, sizeof(program));
    
    computer.memory[0x30] = 0x05;  // Set value in memory[0x30]
    
    // Run the computer and write results to the file
    run_computer(&computer, file);

    fclose(file);

    return 0;
}
