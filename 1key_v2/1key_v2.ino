/*
  CDCinUserCode

  A simple example echoes back every line of data it receives.
  Also it prints how many lines it has echoed.

  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user266

*/
/*
  CdcHidCombo - 支持单键和最多三个组合键
  CH55xduino 示例

  功能：
  - 串口命令修改按键
  - 支持单键或组合键（最多三个键）
  - 保存按键到 EEPROM
*/
#ifndef USER_USB_RAM
#error "This example needs USER USB setting"
#endif

#include "src/CdcHidCombo/USBCDC.h"
#include "src/CdcHidCombo/USBHIDKeyboard.h"
#include <string.h>
#include <stdbool.h>

// ================= EEPROM =================
#define EEPROM_FLAG_ADDR 0
#define EEPROM_KEY_ADDR  1
#define EEPROM_MAGIC     0xAA

// ================= 按键 =================
#define BUTTON1_PIN 15
bool buttonPrev = false;

// ================= 键存储 =================
uint8_t keys[3] = {0};
uint8_t keyCount = 0;
uint8_t modifier = 0;

// ================= 串口缓存 =================
char recvStr[64];
uint8_t recvPtr = 0;
bool stringComplete = false;

// ================= 功能键映射 =================
uint8_t stringToHidKey(char* str) {

    if(strcmp(str,"CTRL")==0) return KEY_LEFT_CTRL;
    if(strcmp(str,"SHIFT")==0) return KEY_LEFT_SHIFT;
    if(strcmp(str,"ALT")==0) return KEY_LEFT_ALT;
    if(strcmp(str,"GUI")==0) return KEY_LEFT_GUI;

    if(strcmp(str,"ENTER")==0) return KEY_RETURN;
    if(strcmp(str,"TAB")==0) return KEY_TAB;
    if(strcmp(str,"ESC")==0) return KEY_ESC;
    if(strcmp(str,"SPACE")==0) return KEY_SPACE;
    if(strcmp(str,"BACKSPACE")==0) return KEY_BACKSPACE;
    if(strcmp(str,"DELETE")==0) return KEY_DELETE;

    if(strcmp(str,"UP")==0) return KEY_UP_ARROW;
    if(strcmp(str,"DOWN")==0) return KEY_DOWN_ARROW;
    if(strcmp(str,"LEFT")==0) return KEY_LEFT_ARROW;
    if(strcmp(str,"RIGHT")==0) return KEY_RIGHT_ARROW;

    if(strcmp(str,"F1")==0) return KEY_F1;
    if(strcmp(str,"F2")==0) return KEY_F2;
    if(strcmp(str,"F3")==0) return KEY_F3;
    if(strcmp(str,"F4")==0) return KEY_F4;
    if(strcmp(str,"F5")==0) return KEY_F5;
    if(strcmp(str,"F6")==0) return KEY_F6;
    if(strcmp(str,"F7")==0) return KEY_F7;
    if(strcmp(str,"F8")==0) return KEY_F8;
    if(strcmp(str,"F9")==0) return KEY_F9;
    if(strcmp(str,"F10")==0) return KEY_F10;
    if(strcmp(str,"F11")==0) return KEY_F11;
    if(strcmp(str,"F12")==0) return KEY_F12;

    return 0;
}

// ================= 普通字符 =================
uint8_t charToHid(char c) {
    if((c >= 65 && c <= 93) ||
       (c >= 97 && c <= 122) ||
       (c >= 42 && c <= 57))
        return c;
    return 0;
}

// ================= 手动解析组合键（核心） =================
void parseComboKey(char* str) {

    modifier = 0;
    keyCount = 0;

    char buffer[16];
    uint8_t bufIndex = 0;

    for(uint8_t i = 0; ; i++) {

        char c = str[i];

        if(c == '+' || c == '\0') {

            buffer[bufIndex] = '\0';

            if(bufIndex > 0) {

                uint8_t k = stringToHidKey(buffer);

                // 修饰键
                if(k == KEY_LEFT_CTRL) modifier |= 0x01;
                else if(k == KEY_LEFT_SHIFT) modifier |= 0x02;
                else if(k == KEY_LEFT_ALT) modifier |= 0x04;
                else if(k == KEY_LEFT_GUI) modifier |= 0x08;
                else {
                    uint8_t ck = charToHid(buffer[0]);
                    if(k != 0) ck = k;

                    if(ck != 0 && keyCount < 3) {
                        keys[keyCount++] = ck;
                    }
                }
            }

            bufIndex = 0;

            if(c == '\0') break;

        } else {
            if(bufIndex < sizeof(buffer)-1) {
                buffer[bufIndex++] = c;
            }
        }
    }
}

// ================= EEPROM =================
void loadEEPROM() {
    if(eeprom_read_byte(EEPROM_FLAG_ADDR) == EEPROM_MAGIC) {
        modifier = eeprom_read_byte(EEPROM_KEY_ADDR);

        for(uint8_t i=0;i<3;i++) {
            keys[i] = eeprom_read_byte(EEPROM_KEY_ADDR + 1 + i);
            if(keys[i] != 0) keyCount++;
        }
    }
}

void saveEEPROM() {
    eeprom_write_byte(EEPROM_FLAG_ADDR, EEPROM_MAGIC);
    eeprom_write_byte(EEPROM_KEY_ADDR, modifier);

    for(uint8_t i=0;i<3;i++)
        eeprom_write_byte(EEPROM_KEY_ADDR + 1 + i, keys[i]);
}

// ================= 初始化 =================
void setup() {
    USBInit();
    pinMode(BUTTON1_PIN, INPUT_PULLUP);

    loadEEPROM();
}

// ================= 主循环 =================
void loop() {

    // ===== 串口接收 =====
    while(USBSerial_available()) {
        char c = USBSerial_read();

        if(c == '\n' || c == '\r') {
            recvStr[recvPtr] = '\0';
            if(recvPtr > 0) stringComplete = true;
        } else {
            recvStr[recvPtr++] = c;
            if(recvPtr >= 63) {
                recvStr[recvPtr] = '\0';
                stringComplete = true;
            }
        }
    }

    // ===== 命令处理 =====
    if(stringComplete) {

        if(strncmp(recvStr,"SETKEY ",7)==0) {

          __xdata char* keyStr = recvStr + 7;

            // 清空
            for(uint8_t i=0;i<3;i++) keys[i]=0;
            keyCount = 0;
            modifier = 0;

            parseComboKey(keyStr);

            if(keyCount > 0 || modifier != 0) {
                saveEEPROM();
                USBSerial_print("New key set: ");
                USBSerial_println(keyStr);
            } else {
                USBSerial_println("Invalid key");
            }

            USBSerial_flush();
        }

        recvPtr = 0;
        stringComplete = false;
    }

    // ===== 按键检测 =====
    bool pressed = !digitalRead(BUTTON1_PIN);

    if(pressed != buttonPrev) {
        buttonPrev = pressed;
        delay(10);

        if(pressed) {

            // 修饰键
            if(modifier & 0x01) Keyboard_press(KEY_LEFT_CTRL);
            if(modifier & 0x02) Keyboard_press(KEY_LEFT_SHIFT);
            if(modifier & 0x04) Keyboard_press(KEY_LEFT_ALT);
            if(modifier & 0x08) Keyboard_press(KEY_LEFT_GUI);

            // 普通键
            for(uint8_t i=0;i<keyCount;i++) {
                Keyboard_press(keys[i]);
            }

        } else {
            Keyboard_releaseAll();
        }
    }
}
