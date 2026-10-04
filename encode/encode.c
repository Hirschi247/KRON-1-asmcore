//
// Created by simon on 10.09.2026.
//

#include "encode.h"
#include "isa.h"

#include <stdio.h>
#include <stdlib.h>

static uint64_t instruction_word = 0;  // Global variable to hold the encoded instruction word

// Each formatclass in isa.h needs a corresponding encoder function.
// Each encoder function takes the token line and the isa entry, extracts the operands,
// and constructs the 32-bit instruction word according to the bit layout specified in the ISA documentation.

static void encode_rrr(Token *token_line, const Isa_Entry *entry) {
    instruction_word = (entry->Prefix << 30)
    | (entry->Subopcode << 23)
    | (token_line[1].value << 18) // rd
    | (token_line[2].value << 13) // rs1
    | (token_line[3].value << 8); // rs2
}

static void encode_alu(Token *token_line, const Isa_Entry *entry) {
    uint64_t rd = (uint64_t)(token_line[1].value & KRON_MASK(KRON_REGISTER_WIDTH));
    rd <<= KRON_RD_POS;
    uint64_t rs1 = (uint64_t)(token_line[2].value & KRON_MASK(KRON_REGISTER_WIDTH));
    rs1 <<= KRON_RS1_POS;

    if (entry->Controls & (1ULL << KRON_ALU_IMM_BIT))
    {
        // Immediate value is used instead of rs2
        uint64_t imm = (uint64_t)(token_line[3].value & KRON_MASK(KRON_OPERAND_WIDTH));
        imm <<= KRON_OPERAND_POS;

        instruction_word |= imm;
    }
    else
    {
        uint64_t rs2 = (uint64_t)(token_line[3].value & KRON_MASK(KRON_REGISTER_WIDTH));
        rs2 <<= KRON_ALU_RS2_POS;

        instruction_word |= rs2;
    }

    instruction_word |= rd;
    instruction_word |= rs1;
}

static void encode_memory(Token *token_line, const Isa_Entry *entry) {
    uint64_t rd_rsrc = (uint64_t)(token_line[1].value & KRON_MASK(KRON_REGISTER_WIDTH));
    rd_rsrc <<= KRON_MEM_RD_RSRC_POS;

    instruction_word |= rd_rsrc;

    if (entry->Subopcode == MEM_LI) {
        uint64_t imm = (uint64_t)(token_line[2].value & KRON_MASK(KRON_OPERAND_WIDTH));
        imm <<= KRON_OPERAND_POS;

        instruction_word |= imm;
    }
    else
    {
        uint64_t offset = (uint64_t)(token_line[2].value & KRON_MASK(KRON_OPERAND_WIDTH));
        offset <<= KRON_OPERAND_POS;

        uint64_t rbase = (uint64_t)(token_line[3].value & KRON_MASK(KRON_REGISTER_WIDTH));
        rbase <<= KRON_MEM_RBASE_POS;

        instruction_word |= rbase;
        instruction_word |= offset;
    }
}

static void encode_jump(Token *token_line, const Isa_Entry *entry) {
    if (entry->Controls & (1ULL << KRON_JUMP_IND_BIT))
    {
        uint64_t reg = (uint64_t)(token_line[1].value & KRON_MASK(KRON_REGISTER_WIDTH));
        reg <<= KRON_JUMP_REG_POS;

        instruction_word |= reg;
    }
    else
    {
        uint64_t address = (uint64_t)(token_line[1].value & KRON_MASK(KRON_JUMP_ADDR_WIDTH));
        address <<= KRON_OPERAND_REG_POS;

        instruction_word |= address;
    }
}

uint64_t *encode (Token *token_line)
{
    if (token_line[0].type != TOK_IDENT)
    {
        perror ("Erster Token der Zeile ist kein IDENT");
        exit (-1);
    }

    const Isa_Entry *entry = isa_lookup(token_line[0].text);

    if (entry == NULL)
    {
        fprintf(stderr, "Unbekanntes Mnemonic: %s\n", token_line[0].text);
        exit(-1);
    }

    instruction_word = 0; // Reset instruction_word for each new instruction

    // Construct the already known parts of the instruction word based on the ISA entry
    uint64_t prefix = (uint64_t)entry->Prefix & KRON_MASK(KRON_PREFIX_WIDTH);
    prefix <<= KRON_PREFIX_POS;
    uint64_t controls = (uint64_t)entry->Controls & KRON_MASK(KRON_CONTROLS_WIDTH);
    controls <<= KRON_CONTROLS_POS;
    uint64_t subopcode = (uint64_t)entry->Subopcode & KRON_MASK(KRON_SUBOPCODE_WIDTH);
    subopcode <<= KRON_SUBOPCODE_POS;

    instruction_word |= prefix;
    instruction_word |= controls;
    instruction_word |= subopcode;

    switch (entry->Prefix) {
        case 0x00: // ALU
            encode_alu(token_line, entry);
            break;

        case 0x01: // Memory
            encode_memory(token_line, entry);
            break;

        case 0x02: // Jump
            encode_jump(token_line, entry);
            break;
    }

    return &instruction_word;
}
