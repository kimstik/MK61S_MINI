#include  "library_pmk.hpp"
#include  "rust_types.h"
#include  "mk61emu_core.h"
#include  "lcd_gui.hpp"
#include  "keyboard.hpp"
#include  "cross_hal.h"

extern  class_mk61_core    mk61s;

static  i8          nProgram;

static const u32 Factorial_size       = 1 + 8 + 1;
static const u32 Fox_hunting_size     = 1 + 105 + 1;
static const u32 Naval_battle_size    = 1 + 105 + 7*2 + 1;
static const u32 Bumblebee_fly_size   = 1 + 105 + 3*7 + 2*4 + 6 + 2*4 + 7 + 4 + 1;
static const u32 Double_interpol_size = 1 + 105 + 10*4 + 1;
static const u32 Infinity_story_size  = 1 + 105 + 11*4 + 6 + 1;
static const u32 Simple_num_size      = 1 +  27 + 1;
static const u32 e_num_size           = 1 + (6 + 6 * 16) + 1;

static  TPunct program_list[CountPrograms] = {
//          0123456789ABCDEF
  {.text = "Factorial      ", .offset = 0},
  {.text = "Fox hunting    ", .offset = Factorial_size},
  {.text = "Naval battle   ", .offset = Factorial_size + Fox_hunting_size},
  {.text = "Bumblebee fly  ", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size},
  {.text = "Double interpol", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size + Bumblebee_fly_size},
  {.text = "Infinity store ", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size + Bumblebee_fly_size + Double_interpol_size},
  {.text = "Simple number  ", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size + Bumblebee_fly_size + Double_interpol_size + Infinity_story_size},
  {.text = "Compute e-num  ", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size + Bumblebee_fly_size + Double_interpol_size + Infinity_story_size + Simple_num_size},
  {.text = "Lunolet-1      ", .offset = Factorial_size + Fox_hunting_size + Naval_battle_size + Bumblebee_fly_size + Double_interpol_size + Infinity_story_size + Simple_num_size + e_num_size},
};

static  u8          mk61_library[] = {
// Factorial
  8,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x34, 0x40, 0x01, 0x60, 0x12, 0x5D, 0x03, 0x50,
  //0x0C, 0x40, 0x01, 0x60, 0x12, 0x5D, 0x03, 0x50, 0x80,
  0xFF,
// Fox hunting  
  9 + 6 * 16,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x40, 0x09, 0x42, 0x43, 0x00, 0x45, 0x46, 0xD5, 0x65, 0x41, 0x02, 0x15, 0x20, 0x60, 0x07, 0x12, 
  0x10, 0x35, 0x40, 0x12, 0x34, 0x22, 0x14, 0x13, 0x22, 0x05, 0x44, 0xD4, 0x11, 0x57, 0x08, 0x3E, 
  0x5B, 0x27, 0xB4, 0x58, 0x07, 0x11, 0x44, 0x63, 0x0B, 0xB4, 0x61, 0x14, 0x50, 0x62, 0x15, 0x45, 
  0x12, 0x34, 0x65, 0x13, 0x41, 0x59, 0x93, 0x65, 0x11, 0x5C, 0x93, 0xD6, 0x63, 0x42, 0x06, 0x44, 
  0x00, 0x45, 0x64, 0x61, 0xD4, 0x59, 0x66, 0x21, 0x21, 0x11, 0x57, 0x99, 0x0F, 0x35, 0x61, 0x35, 
  0x11, 0x57, 0x93, 0x10, 0x57, 0x93, 0x0F, 0x13, 0x31, 0x17, 0x17, 0x5E, 0x94, 0xD5, 0x58, 0x66, 
  0x65, 0x51, 0x42, 0x5A, 0x37, 0x66, 0x03, 0x23, 0x50,
  0xFF,
// Naval battle
  9 + 6 * 16,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x51, 0x50, 0x5C, 0x89, 0x5D, 0x68, 0x6A, 0x52, 0x04, 0x53, 0x76, 0x6D, 0x10, 0x4E, 0x14, 0x07, 
  0x53, 0x76, 0x15, 0x14, 0x08, 0x13, 0x0B, 0x0C, 0x23, 0x34, 0x11, 0xDE, 0x37, 0x35, 0x52, 0x6C, 
  0x07, 0x12, 0x20, 0x10, 0x35, 0x4C, 0x53, 0x71, 0xAB, 0xEA, 0x25, 0xDE, 0x39, 0xBE, 0x10, 0x32, 
  0x4D, 0x52, 0x34, 0x40, 0x17, 0x09, 0x4D, 0xBD, 0x01, 0x11, 0x5E, 0x54, 0x60, 0x41, 0x15, 0xAA, 
  0x5B, 0x63, 0x60, 0x41, 0x05, 0x4D, 0xAA, 0x6C, 0x0C, 0x02, 0x34, 0x52, 0x3E, 0x0F, 0x14, 0x0F, 
  0x13, 0x34, 0x0E, 0x25, 0x12, 0x01, 0x11, 0x11, 0x52, 0x15, 0x0F, 0x34, 0xAB, 0x57, 0x68, 0x53, 
  0x42, 0x61, 0x5B, 0x87, 0x6B, 0x52, 0x00, 0x00, 0x00,
// registers {nReg, sign_num|sign_pow|len, abs(pow), mantissa... }
  0x0A, 0x04, 0x07, 0x33, 0x33, 0x33, 0x31,
  0x0B, 0xC4, 0x07, 0x8C, 0xEC, 0x6A, 0xBA,
  0xFF,
// Bumble fly
  9 + 6 * 16,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x0D, 0x40, 0x4E, 0x46, 0xD5, 0x64, 0x48, 0x6A, 0x10, 0x53, 0x5F, 0x59, 0x16, 0x57, 0x20, 0xAC,
  0x59, 0x2E, 0x0B, 0x40, 0x51, 0x2F, 0xAC, 0x59, 0x12, 0x57, 0x28, 0xD8, 0x3A, 0xB8, 0x67, 0x50,
  0xAC, 0x57, 0x2D, 0x68, 0x10, 0x48, 0x51, 0x2F, 0x6B, 0x3A, 0x4B, 0x67, 0x50, 0x01, 0x4E, 0x6A,
  0x4D, 0xD6, 0x6D, 0x69, 0x21, 0x12, 0x4D, 0x34, 0x57, 0x31, 0x6E, 0x10, 0x4E, 0x60, 0x66, 0x10,
  0x46, 0x15, 0x13, 0x6D, 0x68, 0x10, 0xD8, 0x37, 0x35, 0x23, 0x07, 0x6E, 0x11, 0x59, 0x59, 0x66,
  0x6E, 0x15, 0x13, 0x68, 0x10, 0x6B, 0x37, 0x34, 0x23, 0x68, 0x44, 0x6D, 0x4A, 0x89, 0x65, 0x0E,
  0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x69, 0x1D, 0x32, 0x52,  
// registers {nReg, sign_num|sign_pow|len, abs(pow), mantissa... }
  0x01, 0x04, 0x00, 0x8B, 0x72, 0x73, 0xE6,
  0x02, 0x04, 0x00, 0x83, 0x7E, 0x73, 0x36,
  0x03, 0x04, 0x00, 0x83, 0x62, 0x73, 0xE6,
  0x04, 0x01, 0x00, 0x10,
  0x05, 0x01, 0x00, 0x00,
  0x07, 0x03, 0x05, 0xDE, 0xCE, 0x55,
  0x09, 0x01, 0x02, 0x10,
  0x0A, 0x41, 0x98, 0x10,
  0x0B, 0x04, 0x00, 0x8A, 0xBB, 0x9B, 0xFB,
  0x0C, 0x01, 0x01, 0x94,
  0xFF,
// Double interpolate  
  9 + 6 * 16,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x00, 0x47, 0x61, 0x0E, 0x64, 0x11, 0x0E, 0x69, 0x0E, 0x68, 0x11, 0x0E, 0x25, 0x25, 0x25, 0x12, 
  0x40, 0x69, 0x0E, 0x6A, 0x11, 0x0E, 0x61, 0x0E, 0x6B, 0x11, 0x0E, 0x25, 0x25, 0x25, 0x12, 0x0E, 
  0x65, 0x12, 0x0E, 0x67, 0x10, 0x47, 0x6A, 0x0E, 0x68, 0x11, 0x0E, 0x61, 0x0E, 0x6B, 0x11, 0x0E,
  0x25, 0x25, 0x25, 0x12, 0x0E, 0x66, 0x12, 0x0E, 0x67, 0x10, 0x47, 0x69, 0x0E, 0x6A, 0x11, 0x0E, 
  0x6B, 0x0E, 0x64, 0x11, 0x0E, 0x25, 0x25, 0x25, 0x12, 0x0E, 0x62, 0x12, 0x0E, 0x67, 0x10, 0x47, 
  0x6A, 0x0E, 0x68, 0x11, 0x0E, 0x6B, 0x0E, 0x64, 0x11, 0x0E, 0x25, 0x25, 0x25, 0x12, 0x0E, 0x63, 
  0x12, 0x0E, 0x67, 0x10, 0x47, 0x0E, 0x60, 0x13, 0x50,
  0x01, 0x01, 0x01, 0x40,  // 40
  0x02, 0x01, 0x02, 0x80,  // 800
  0x03, 0x01, 0x03, 0x10,  // 1000
  0x04, 0x01, 0x01, 0x20,  // 20
  0x05, 0x01, 0x02, 0x50,  // 500
  0x06, 0x01, 0x02, 0x60,  // 600
  0x08, 0x01, 0x02, 0x30,  // 300
  0x09, 0x01, 0x03, 0x10,  // 1000
  0x0A, 0x01, 0x02, 0x80,  // 800
  0x0B, 0x01, 0x01, 0x22,  // 22
  0xFF,
// Infinity story  
  9 + 6 * 16,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x52, 0x6A, 0x1D, 0x5C, 0x18, 0x63, 0x5E, 0x44, 0x62, 0x0B, 0x43, 0x00, 0x42, 0x87, 0x42, 0x00, 
  0x43, 0x87, 0x77, 0x63, 0x5E, 0x27, 0x62, 0x43, 0x00, 0x42, 0x87, 0x0B, 0x42, 0x00, 0x43, 0x5D, 
  0x42, 0x5B, 0x37, 0x6D, 0x50, 0x61, 0x69, 0x12, 0x4C, 0x40, 0x64, 0x62, 0x10, 0x44, 0x65, 0x63, 
  0x10, 0x45, 0x10, 0x31, 0x6C, 0x11, 0x59, 0x64, 0x65, 0x0F, 0x11, 0x59, 0x63, 0x6D, 0x50, 0x27, 
  0x65, 0x0F, 0x01, 0x11, 0x11, 0x5E, 0x78, 0x66, 0x64, 0x11, 0x59, 0x78, 0x6B, 0x45, 0x65, 0x6E, 
  0x11, 0x5E, 0x85, 0x6D, 0x50, 0x6B, 0x65, 0x11, 0x5E, 0x96, 0x05, 0x64, 0x11, 0x5E, 0x96, 0x50, 
  0x64, 0xA8, 0x65, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E,
  0x00, 0x01, 0x02, 0x10, // R0 = 100
  0x01, 0x01, 0x01, 0x10, // R1 = 10
  0x03, 0x01, 0x00, 0x01, // R3 = 1
  0x06, 0x81, 0x01, 0x07, // R6 = -7
  0x07, 0x01, 0x01, 0x37, // R7 = 37
  0x08, 0x01, 0x01, 0x99, // R8 = 99
  0x09, 0x01, 0x01, 0x10, // R9 = 10
  0x0A, 0x01, 0x02, 0x10, // RA = 100
  0x0B, 0x81, 0x01, 0x40, // RB = -40
  0x0C, 0x01, 0x02, 0x10, // RC = 100
  0x0E, 0x81, 0x01, 0x39, // RE = -39
  0x0D, 0x03, 0x00, 0xDD, 0xD0, 0xDF, // RD = Г.ГГОГ
  0xFF,
// Simple numbers
  27,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x44, 0xD4, 0x64, 0x21, 0x01, 0x10, 0x40, 0x47, 0xD7, 0x14, 0x67, 0x11, 0x5D, 0x18, 0x64, 0x50,
  0x51, 0x00, 0x5E, 0x22, 0x51, 0x01, 0x64, 0x60, 0x13, 0x51, 0x07,
  0xFF,
// e num
  6 * 16 + 6,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x0D, 0x06, 0x05, 0x0E, 0x07, 0x15, 0x0E, 0x60, 0x53, 0x81, 0x40, 0x61, 0x53, 0x81, 0x41, 0x62, 
  0x53, 0x81, 0x42, 0x63, 0x53, 0x81, 0x43, 0x64, 0x53, 0x81, 0x44, 0x65, 0x53, 0x81, 0x45, 0x66, 
  0x53, 0x81, 0x46, 0x67, 0x53, 0x81, 0x47, 0x68, 0x53, 0x81, 0x48, 0x69, 0x53, 0x81, 0x49, 0x6A, 
  0x53, 0x81, 0x4A, 0x6B, 0x53, 0x81, 0x4B, 0x6C, 0x53, 0x81, 0x4C, 0x6D, 0x53, 0x81, 0x4D, 0x6E, 
  0x53, 0x81, 0x4E, 0x25, 0x25, 0x01, 0x11, 0x5E, 0x04, 0x60, 0x07, 0x15, 0x13, 0x01, 0x10, 0x40, 
  0x50, 0x14, 0x25, 0x10, 0x14, 0x0E, 0x0F, 0x14, 0x13, 0x34, 0x0E, 0x25, 0x14, 0x12, 0x0F, 0x25, 
  0x11, 0x06, 0x15, 0x12, 0x14, 0x52,
  0xFF,
// Lunalet-1
  6 * 16 + 2,
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
  0x6D, 0x5C, 0x09, 0x0E, 0x68, 0x13, 0x14, 0x53, 0x90, 0x6A, 0x57, 0x43, 0x5C, 0x33, 0x02, 0x12,
  0x0E, 0x64, 0x63, 0x11, 0x12, 0x6B, 0x22, 0x10, 0x21, 0x6B, 0x11, 0x13, 0x0E, 0x68, 0x12, 0x51,
  0x90, 0x6D, 0x57, 0x86, 0x63, 0x22, 0x21, 0x67, 0x11, 0x5C, 0x87, 0x6B, 0x6A, 0x50, 0x41, 0x42,
  0x57, 0x43, 0x13, 0x48, 0x65, 0x6D, 0x10, 0x13, 0x66, 0x12, 0x43, 0x64, 0x11, 0x62, 0x12, 0x6B,
  0x10, 0x4B, 0x0F, 0x10, 0x02, 0x13, 0x62, 0x12, 0x6A, 0x10, 0x4A, 0x6C, 0x62, 0x60, 0x12, 0x11,
  0x4C, 0x6D, 0x61, 0x11, 0x4D, 0x52, 0x66, 0x69, 0x50, 0x0D, 0x41, 0x14, 0x42, 0x5C, 0x50, 0x63,
  0x51, 0x59,
// registers {nReg, sign_num(8-bit)|sign_pow(7-bit)|len(bytes for mantisa), abs(pow), mantissa... }
  0x00, 0x01, 0x00, 0x01,       // R0 = 1
  0x04, 0x02, 0x00, 0x16, 0x20, // R4 = 1.62
  0x05, 0x02, 0x03, 0x22, 0x50, // R5 = 2250
  0x06, 0x02, 0x03, 0x36, 0x60, // R6 = 3660
  0x07, 0x02, 0x01, 0x29, 0x43, // R7 = 29.43 3G
  0x09, 0x01, 0x00, 0xD0,       // R9 = Г
  0x0D, 0x01, 0x02, 0x40,       // RD = 2250
  0x0C, 0x01, 0x03, 0x36,       // RC = 3600
  0xFF
};

const u8 pack_clear_register[6] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00};  // 0.0

void  init_library(void) {
  nProgram = 0;
}

int   select_program(class_keyboard keyboard) {

  do {
    const int delta = (nProgram + 1) - 2;
    const int up = (delta <= 0)? 0 : delta;

    for(int i=0; i < 2; i++) {
      lcd.setCursor(0,i); 
      const int real_index = i + up;
      if(nProgram == real_index) { 
        lcd.print('>');
      } else {
        lcd.print(' ');
      }
      lcd.print(program_list[real_index].text);
    }

    const i32 last_key_code = keyboard.get_key_wait();
    switch(last_key_code) {
      case KEY_RIGHT_PRESS:
          if(nProgram < (CountPrograms-1)) nProgram++;
        break;
      case KEY_LEFT_PRESS:
          if(nProgram > 0) nProgram--;
        break;
      case KEY_ESC_PRESS:
        return -1; // отмена
      case KEY_OK_PRESS:
        #ifdef SERIAL_OUTPUT
          Serial.print("load code '");
          Serial.print(program_list[nProgram].text);
          Serial.print("' offset: ");
          Serial.println(program_list[nProgram].offset);
        #endif 
        return nProgram;
    }

  } while(true);
}

void  load_program(int nProg_for_load) {
  int offs = program_list[nProg_for_load].offset;
  for(u8 nReg=0; nReg < 0x0F; nReg++) MK61Emu_UnpackRegster(nReg, (u8*) &pack_clear_register);  // clear registers

  const u32 code_len = mk61_library[offs++];
  for(u32 addr=0; addr < code_len; addr++) {
    MK61Emu_SetCode(mk61s.get_ring_address(addr), mk61_library[offs++]);
  }

  u8* pPack_number = &mk61_library[offs];
  while(*pPack_number != 0xFF) {
    #ifdef DEBUG_LOAD
      Serial.print("unpack reg: "); Serial.println(*pPack_number, HEX);
    #endif
    const u8 RegisterN = *pPack_number++;
    pPack_number = MK61Emu_UnpackRegster(RegisterN, pPack_number);
  }
}
