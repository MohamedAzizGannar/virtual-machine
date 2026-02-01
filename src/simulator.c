#include "../include/simulator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int simulate(char *filename) {

  // initialisation :
  uint8_t mem[65536];
  FILE *file_ptr = fopen(filename, "r");
  if (!file_ptr) {
    fprintf(stderr, "Failure Opening Source File for Simulation, Please Check "
                    "Name : Exiting\n");
    return -1;
  }
  int address = 0;
  char line[16];
  while (fgets(line, sizeof(line), file_ptr)) {
    uint32_t instruction;
    if (sscanf(line, "%X", &instruction) == 1) {
      mem[address++] = (instruction >> 24) & 0xFF;
      mem[address++] = (instruction >> 16) & 0xFF;
      mem[address++] = (instruction >> 8) & 0xFF;
      mem[address++] = (instruction) & 0xFF;
    }
  }
  fclose(file_ptr);

  int32_t r[32];
  for (int i = 0; i < 32; i++)
    r[i] = 0;

  uint32_t PC;
  PC = 0;

  int Z = 0;
  int C = 0;
  int N = 0;
  int opcode;
  int rd; // register destination, pr savoir oû mettre le resultat
  int rn; // premier operande
  int S;  // 0 ou 1 pour savoir si on prends un registre ou un nombre pr le 2eme
          // operande
  int src2;    // numero de registre, on prend lui si S == 0
  int16_t imm; // nombre, on prend lui si S == 1

  // boucle infini arrêté par hlt :
  while (1) {

    // fetch :
    uint32_t instruction =
        0; // on recupere et construit l'instruction stoqué en mémoire
    instruction |= (uint32_t)mem[PC] << 24;
    instruction |= (uint32_t)mem[PC + 1] << 16;
    instruction |= (uint32_t)mem[PC + 2] << 8;
    instruction |= (uint32_t)mem[PC + 3];
    PC = PC + 4;

    // decode
    opcode = (instruction >> 27) &
             0x1F; // on recupere l'opcode(5 bits de poids fort)
    rd = (instruction >> 22) & 0x1F;
    rn = (instruction >> 17) & 0x1F;
    S = (instruction >> 16) & 0x1;
    imm = (int16_t)(instruction & 0xFFFF);
    src2 = instruction & 0x1F;

    // execute
    switch (opcode) {
    case 0: { // or
      if (S == 0) {
        r[rd] = r[rn] | r[src2];
      } else {
        r[rd] = r[rn] | imm;
      }
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 1: { // and
      if (S == 0) {
        r[rd] = r[rn] & r[src2];
      } else {
        r[rd] = r[rn] & imm;
      }
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 2: { // xor
      if (S == 0) {
        r[rd] = r[rn] ^ r[src2];
      } else {
        r[rd] = r[rn] ^ imm;
      }
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 3: { // add
      int64_t result;
      if (S == 0) {
        result = (int64_t)r[rn] + (int64_t)r[src2];
      } else {
        result = (int64_t)r[rn] + (int64_t)imm;
      }
      r[rd] = (int32_t)result;
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      C = ((uint64_t)result > 0xFFFFFFFF);
      break;
    }
    case 4: { // sub
      int64_t result;
      if (S == 0) {
        result = (int64_t)r[rn] - (int64_t)r[src2];
      } else {
        result = (int64_t)r[rn] - (int64_t)imm;
      }
      r[rd] = (int32_t)result;
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      C = (result < 0 || result > 0xFFFFFFFF);
      break;
    }
    case 5: { // mul
      int32_t op1 = r[rn] & 0xFFFF;
      int32_t op2;
      if (S == 0) {
        op2 = r[src2] & 0xFFFF;
      } else {
        op2 = imm & 0xFFFF;
      }
      int64_t full_res = (int64_t)op1 * (int64_t)op2;
      r[rd] = (int32_t)full_res;
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      C = (full_res > 0xFFFFFFFF || full_res < 0);
      break;
    }
    case 6: { // div
      if (S == 0) {
        if (r[src2] == 0) {
          fprintf(stderr,
                  "Failure During Division : Dividing By Zero : Exiting\n");
          return -1;
        }
        r[rd] = r[rn] / r[src2];
      } else {
        if (imm == 0) {
          fprintf(stderr,
                  "Failure During Division : Dividing By Zero : Exiting\n");
          return -1;
        }
        r[rd] = r[rn] / imm;
      }
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      C = 0;
      break;
    }
    case 7: { // shr
      int32_t shift_amount;
      uint32_t last_bit_fall = 0;
      if (S == 0) {
        shift_amount = r[src2];
      } else {
        shift_amount = imm;
      }
      if (shift_amount > 0) {
        if (shift_amount >= 32) {
          last_bit_fall = ((uint32_t)r[rn] >> (shift_amount - 1)) & 1;
        }
        r[rd] = (uint32_t)r[rn] >> shift_amount;
      } else if (shift_amount < 0) {
        if (-shift_amount >= 32) {
          last_bit_fall = ((uint32_t)r[rn] << (shift_amount + 32)) & 1;
        }
        r[rd] = (uint32_t)r[rn] << (-shift_amount);
      } else {
        r[rd] = r[rn];
      }
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      C = last_bit_fall;
      break;
    }
    case 12: { // ldb
      uint32_t address;
      if (S == 0) {
        address = r[rn] + r[src2];
      } else {
        address = r[rn] + imm;
      }
      int8_t byte_val = (int8_t)mem[address];
      r[rd] = (int32_t)byte_val;
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 13: { // ldh
      uint32_t address;
      if (S == 0) {
        address = r[rn] + r[src2];
      } else {
        address = r[rn] + imm;
      }
      int16_t half_val = (int16_t)(mem[address] << 8) | (mem[address + 1]);
      r[rd] = (int32_t)(half_val);
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 14: { // ldw
      uint32_t address;
      if (S == 0) {
        address = r[rn] + r[src2];
      } else {
        address = r[rn] + imm;
      }
      r[rd] = 0;
      r[rd] |= (uint32_t)mem[address] << 24;
      r[rd] |= (uint32_t)mem[address + 1] << 16;
      r[rd] |= (uint32_t)mem[address + 2] << 8;
      r[rd] |= (uint32_t)mem[address + 3];
      Z = (r[rd] == 0);
      N = (r[rd] >> 31) & 1;
      break;
    }
    case 15: { // stb
      int32_t address;
      if (S == 0) {
        address = r[rd] + r[src2];
      } else {
        address = r[rd] + imm;
      }
      mem[address] = r[rn] & 0xFF;
      Z = (r[rn] == 0);
      N = (r[rn] >> 31) & 1;
      break;
    }
    case 16: { // sth
      int32_t address;
      if (S == 0) {
        address = r[rd] + r[src2];
      } else {
        address = r[rd] + imm;
      }
      mem[address] = (r[rn] >> 8) & 0xFF;
      mem[address + 1] = (r[rn]) & 0xFF;
      Z = (r[rn] == 0);
      N = (r[rn] >> 31) & 1;
      break;
    }
    case 17: { // stw
      int32_t address;
      if (S == 0) {
        address = r[rd] + r[src2];
      } else {
        address = r[rd] + imm;
      }
      mem[address] = (r[rn] >> 24) & 0xFF;
      mem[address + 1] = (r[rn] >> 16) & 0xFF;
      mem[address + 2] = (r[rn] >> 8) & 0xFF;
      mem[address + 3] = (r[rn]) & 0xFF;
      Z = (r[rn] == 0);
      N = (r[rn] >> 31) & 1;
    } break;
    case 21: { // jmp
      int32_t address;
      if (S == 0) {
        address = r[src2];
      } else {
        address = imm;
      }
      PC = address;
      break;
    }
    case 22: { // jzs
      if (Z) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 23: { // jzc
      if (!Z) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 24: { // jcs
      if (C) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 25: { // jcc
      if (!C) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 26: { // jns
      if (N) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 27: { // jnc
      if (!N) {
        int32_t address;
        if (S == 0) {
          address = r[src2];
        } else {
          address = imm;
        }
        PC = address;
      }
      break;
    }
    case 28: { // in
      printf("Please Enter A Value : ");
      scanf("%d", &r[rd]);
      break;
    }
    case 29: { // out
      printf("Value: %d\n", r[rd]);
      break;
    }
    case 30: { // rnd
      uint32_t max_val = 0;
      if (S == 0) {
        max_val = r[src2];
      } else {
        max_val = imm;
      }
      r[rd] = rand() % (max_val - r[rn]) + r[rn];
      break;
    }

    case 31: { // hlt
      return 1;
    }
    default:
      fprintf(stderr, "Unknown Opcode : Exiting\n");
      return -1;
    }
    r[0] = 0;
  }
  return 1;
}
