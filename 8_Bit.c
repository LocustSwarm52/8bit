#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE 256
#define STACK_SIZE 16

#define LDA 0x01  // Load accumulator from memory
#define STA 0x02  // Store accumulator to memory
#define LDB 0x03  // Load B register from memory
#define STB 0x04  // Store B register to memory
#define ADD 0x05  // Add B to accumulator
#define SUB 0x06  // Subtract B from accumulator
#define JMP 0x07  // Jump to address
#define JZ  0x08  // Jump if zero
#define HLT 0xFF  // HALT

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

void execute_instruction(Computer *c) {
    uint8_t opcode = c->memory[c->program_counter];
    uint8_t operand = c->memory[c->program_counter + 1];
    
    switch(opcode) {
        case LDA:
            c->accumulator = c->memory[operand + 0x80];  // Data area starts at 0x80
            c->program_counter += 2; // LDA is a 2-byte instruction
            break;
            
        case STA:
            c->memory[operand + 0x80] = c->accumulator; // Store in data area
            c->program_counter += 2; // STA is a 2-byte instruction
            break;
            
        case LDB:
            c->b_register = c->memory[operand + 0x80];  // Data area starts at 0x80
            c->program_counter += 2; // LDB is a 2-byte instruction
            break;
            
        case STB:
            c->memory[operand + 0x80] = c->b_register; // Store in data area
            c->program_counter += 2; // STB is a 2-byte instruction
            break;
            
        case ADD:
            c->accumulator += c->b_register;
            c->zero_flag = (c->accumulator == 0);
            c->program_counter++;  // ADD is a single-byte instruction
            break;
            
        case JMP:
            c->program_counter = operand; // JMP is a 2-byte instruction
            break;
            
        case JZ:
            if(c->zero_flag) {
                c->program_counter = operand;
            } else {
                c->program_counter += 2;
            }
            break;
            
        case HLT:
            c->running = 0;
            break;
            
        default:
            c->running = 0;  // Unknown opcode
    }
}

void run_computer(Computer *c) {
    while(c->running) {
        execute_instruction(c);
    }
}

int main() {
    Computer computer;
    init_computer(&computer);

    // Setup memory: Program instructions in 0x00 - 0x7F, data in 0x80 - 0xFF
    uint8_t program[] = {
        LDA, 0x00,   // LDA from address 0x80
        ADD, 0x01,   // ADD from address 0x81
        STA, 0x02,   // STA to address 0x82
        HLT           // Halt execution
    };

    uint8_t data[] = {
        0x05,   // Data at address 0x80
        0x03,   // Data at address 0x81
        0x00    // Data at address 0x82 (will store result here)
    };

    // Copy program instructions to memory (starting at 0x00)
    memcpy(computer.memory, program, sizeof(program));

    // Copy data to memory (starting at 0x80)
    memcpy(&computer.memory[0x80], data, sizeof(data));

    // Run the computer
    run_computer(&computer);

    // Output the result (you can inspect the result in memory at address 0x82)
    printf("Result stored at 0x82: 0x%02X\n", computer.memory[0x82]);

    return 0;
}
