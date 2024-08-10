#include "rust_types.h"
#include "lcd_gui.hpp"
#include "tools.hpp"
#include "basic.hpp"
#include "keyboard.hpp"
#include "cross_hal.h"

/*
   Cx,  Bx, MUL, DIV, NON,
  POW,  XY, ADD, SUB, NON,
  NEG, _3_, _6_, _9_, NON,
  DOT, _2_, _5_, _8_, NON,
  _0_, _1_, _4_, _7_, NON,
  JSR, JMP,  xP,  Px, NON,
  RUN, RET,  SF,  SB, NON,
  NON, NON,   K,   F, NON
*/  
static const char BASIC_key[40] = {
   '%', '%', '*', '/', '%',
  '?', '%', '+', '-', '%',
   '%', '3', '6', '9', '%',
  '.', '2', '5', '8', '%',
  '0', '1', '4', '7', '%',
  ' ', '%', '%', '%', '%',
  LCD_NOT_EQU_CHAR, 0xA6, '=', '<', '%',
  '%', '%', '%', '%', '%'
};

static const char BASIC_alpha_key[40] = {
  'D', 'E', 'F', 'G', '%',
  'C', '%', '%', 'X', 'H',
  'B', '%', '%', '%', 'I',
  'A', 'Z', 'W', '%', 'J',
  'T', 'Y', '%', '%', 'K',
  'S', '$', 'V', 'U', 'L',
  'R', '%', '%', '%', 'M',
  'Q', 'P', 'O', '%', 'N'
};

static const char BASIC_Kshift_key[40] = {
  '!', ':', '%', '%', '%',
  '^', '"', '%', '%', '%',
  '&', '%', '%', '%', '%',
  '|', '%', '%', '%', '%',
  '%', '%', '%', '%', '%',
  '%', '%', '%', '%', '%',
  '%', '%', '%', '%', '%',
  '%', '%', '%', '%', '%'
};

static constexpr u32 CURSOR_BLINK_MS    = 850;
static constexpr u8  SHIFT_CURSOR       = 0;
static constexpr u8  CURSOR_ASCII       = 0xFF;

static int   IPpi;
static u8    picode[100];
static u16   BasicList[40];
static int   NextBasic;

bool  BasicIsReady(void) {return (bool) (NextBasic >= 0);}

void  InitBasic(void) {
  NextBasic = -1;
  IPpi = 0;
  //memset(&BasicList, 0xFFFF, sizeof(BasicList));
}

void RunBasic(int BasicN) {
  int offset = BasicList[BasicN];
  lcd.setCursor(0, 0); lcd.print("                ");
  do {
    const BASIC_WORD word = (BASIC_WORD) picode[offset++];
    switch(word) {
      case BASIC_WORD::_PRINT:
          lcd.setCursor(0, 0);
          lcd.print((char*) &picode[offset]);
        break;
      case BASIC_WORD::_INPUT:
      case BASIC_WORD::_IF:
      case BASIC_WORD::_ELSE:
        break;
      case BASIC_WORD::_STOP:
        return;
    }
  } while(true);
}

int  AssignBasic(void) {
  int active = 0;

  lcd.clear();
  if(NextBasic < 0) {
    lcd.setCursor(0, 0); lcd.print("BASIC is empty!");
    lcd.setCursor(0, 1); lcd.print("Press any key...");
    keyboard.get_key_wait();
  } else {

    lcd.setCursor(0, 0); lcd.print("Assign BASIC:");
    do{
      lcd.setCursor(0, 1);
      for(int i = 0; i <= NextBasic; i++) {
        lcd.print(i); 
        if(i == active) lcd.write(LCD_LT_ARROW_CHAR); else lcd.write(' ');
      }
      const i32 key = keyboard.get_key_wait();
      switch(key) {
        case  KEY_LEFT: 
            if(active > 0) active--;
          break;
        case  KEY_RIGHT:
            if(active < NextBasic) active++;
          break;
        case  KEY_OK:
          return active;
        case  KEY_ESC:
          return -1;
        case  KEY_RUN:
          RunBasic(active);
      }
    } while(true);
  }
  return -1;
}

char* skip_space(char* line) {
  do {
      const char symbol = *line;
      if(symbol == ' ' || symbol == 0) break;
      line++;
  } while(true);
  return line;
}

bool ErrorBasic(const char* text) {
  lcd.setCursor(0,0); lcd.print("Error BASIC!");
  lcd.setCursor(0,1); lcd.print(text);
  keyboard.get_key_wait();
  return false;
}

/*
   - трансляция по текущему адресу IPpi
   - первым при компиляции транслируется оператор начало области BEGIN
   - конструкция HLT без следующего за ним имени рассматривается как оператор остановки исполнения - STOP 
   - конструкция HLT name рассматривается как оператор остановки, завершающий трансляцию с поименованием области операторов как программа "name" - END
    + имя области следует за оператором END
    + IPip устанавливается за строкой имемнующей область
*/

bool  CompileBasic(char* program) {
  const int start_program = IPpi;
  picode[IPpi++] = (u8) BASIC_WORD::_BEGIN;

  while(*program != 0) { // не конец строки продолжим
    const char* token = program;
    program = skip_space(program);
    *program = 0;

    if(strcmp(token, "?") == 0) {
      #ifdef DEBUG_BASIC
        Serial.print("Basic compiler token:"); Serial.println((char*) token);
      #endif
      picode[IPpi++] = (u8) BASIC_WORD::_PRINT;
      // передаем строку 
      program++;
      if(*program++ != '"') return ErrorBasic("begin quote?");
      int len_string = BASIC_MAXIMUM_STRING;
      do {
        picode[IPpi++] = *program++;
        if(--len_string < 0) return ErrorBasic("end quote?");
      } while(*program != '"');
      picode[IPpi++] = 0;
      program++;
    } else if(strcmp(token, "HLT")) {
      #ifdef DEBUG_BASIC
        Serial.print("Basic parser - HLT");
      #endif
      program = skip_space(program);
      if(*program >= 'A' && *program <= 'Z') { // END - конец трансляции файла
        const char *name = program;
        picode[IPpi++] = (u8) BASIC_WORD::_END;
        while(*program != 0 && *program != ' ') picode[IPpi++] = *program++;
        *program = 0;
        #ifdef DEBUG_BASIC
          Serial.print("Basic compile END - area set as "); Serial.println(name);
        #endif
        break;
      } else { // STOP - остановка программы
        picode[IPpi++] = (u8) BASIC_WORD::_STOP;
        #ifdef DEBUG_BASIC
          Serial.print("Basic compile STOP");
        #endif
      }
    }
    
  }
  
  BasicList[++NextBasic] = start_program;

  #ifdef DEBUG_BASIC
    Serial.println("BASIC picode:");
    for(int i=start_program; i < IPpi; i++) {
      Serial_write_hex(picode[i]);
      Serial.write(' ');
    }
    Serial.println();
  #endif

  return true;
}

void  EditBasic(void) { // Редактирования строки BASIC
  char  program[2][17];
  i8    idx = 0;
  i8    line = 0;
  u32   blink_cursor_time;

  u8    cursor              = CURSOR_ASCII;
  u8    cursor_blinked_char = CURSOR_ASCII;
  u32   cursor_blink_on     = 0;
  char* pTABLE_key  = (char*) &BASIC_key;

  //clear program BASIC space
    memset(&program, ' ', sizeof(program));
    program[0][16] = 0; program[1][16] = 0;

  lcd.clear(); 
  lcd.setCursor(0,0); lcd.print((char*) &program[0][0]);
  lcd.setCursor(0,1); lcd.print((char*) &program[1][0]);
  lcd.setCursor(0,0); lcd.write(cursor_blinked_char); lcd.setCursor(0,0);

  keyboard.process_init();
  blink_cursor_time = millis() + CURSOR_BLINK_MS;

  while(true) {
    // Цикл ожиадния нажатия клавиши 
      const u32 now = millis();
      keyboard.process(now);
      i32 key_code = keyboard.get_key(key_state::PRESSED);

      if(now >= blink_cursor_time) {
          cursor_blinked_char = (cursor_blink_on)? cursor : program[line][idx];
          cursor_blink_on = ~cursor_blink_on;

          lcd.setCursor(idx, line); lcd.write(cursor_blinked_char); lcd.setCursor(idx, line);
          blink_cursor_time = millis() + CURSOR_BLINK_MS;
      }

      if(key_code < 0) continue;

    lcd.setCursor(idx, line); lcd.write(program[line][idx]); lcd.setCursor(idx, line);

    const char input_char = pTABLE_key[key_code]; 
    if(key_code != KEY_DEGREE && cursor == SHIFT_CURSOR) { //Если нажат шифт и кнопка не ЗАБОЙ то вводим символ
      key_code = 0;
    }

    switch (key_code) {
      case KEY_K:
      case KEY_ALPHA:
          if(key_code == KEY_K) {
            lcd.createChar(0, (uint8_t*) &Symbol[0]);
            pTABLE_key = (char*) &BASIC_Kshift_key;
          } else {
            lcd.createChar(0, (uint8_t*) &Alpha[0]);
            pTABLE_key = (char*) &BASIC_alpha_key;
          }
          cursor = SHIFT_CURSOR;
          cursor_blink_on = -1;
          blink_cursor_time = now;
        continue;
      case KEY_ESC:
          #ifdef DEBUG_BASIC
            Serial.print("1."); Serial.println((char*) &program[0]);
            Serial.print("2."); Serial.println((char*) &program[1]);
          #endif
          CompileBasic((char*) &program);
          #ifdef DEBUG_BASIC
            Serial.println("compiled program.");
          #endif
          return;
      case KEY_DEGREE:  /* ЗАБОЙ (Г) */
        if(idx > 0) { // если курсор не в начале строки
          #ifdef DEBUG_BASIC
            Serial.print("BS: "); Serial.print((char*) &program[line][0]);
          #endif
          idx--;
          // 0123456789
          // abcd*fgrte
          memcpy(&program[line][idx],&program[line][idx+1],(15 - idx));
          #ifdef DEBUG_BASIC
            Serial.print(" -> "); Serial.println((char*) &program[line][0]);
          #endif
          program[line][15] = ' ';
          lcd.setCursor(0, line);
          lcd.print((char*) &program[line][0]);
        }
        break;
      case KEY_OK: /* ENTER */
          if(line < 1){
            line++;
            idx = 0;
          }
        break;
      case KEY_PUSH_B: /* UP (B^) */
          if(line > 0) {
            line--;
            idx = 0;
          }
        break;
      case KEY_LEFT:
          if(idx > 0) idx--;
        break;
      case KEY_RIGHT:
          if(idx < 15) idx++;
        break;
      default:
        if(idx < 16) {
          program[line][idx] = input_char;
          lcd.setCursor(0, line);
          lcd.print((char*) &program[line][0]);
          idx++;
        }
    }

// очистить состояние шифтов после любой нажатой кнопки
    pTABLE_key  = (char*) &BASIC_key;
    cursor      = CURSOR_ASCII;

    lcd.setCursor(idx, line); lcd.write(CURSOR_ASCII); lcd.setCursor(idx, line);
  }
}
