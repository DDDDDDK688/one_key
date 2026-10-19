# One-Key Keyboard

A compact USB HID keyboard project based on the CH55x microcontroller.

This repository contains the original `1key` version and the updated `1key_v2` version. The original version is preserved for historical reference, while `1key_v2` provides improved firmware functionality and a redesigned enclosure.

## Features

### Basic Functions

- Works as a standard USB HID keyboard.
- Supports connection to a computer through USB.
- Sends keyboard key codes to the host system.
- No additional driver is required on most operating systems.
- Compact design suitable for custom keyboards, shortcut keys, and macro applications.
- Includes the keyboard PCB and enclosure design files.

### Updated Functions in `1key_v2`

- Supports configuring keyboard keys through USB serial commands.
- Supports single keys and key combinations.
- Key configurations can be saved to EEPROM.
- Saved configurations are restored automatically after power-on or reboot.

## Enclosure Improvements

The enclosure has been remodeled to improve the reliability and stability of screw installation.

- The screw mounting structure has been redesigned.
- Screw holes provide better alignment and support.

## How to Use

### 1. Connect the Keyboard

Connect the keyboard to your computer using a USB cable.

The keyboard works as a USB HID keyboard and also provides a USB CDC virtual serial port.

### 2. Open the Serial Port

Open a serial communication software.

Select the serial port assigned to the keyboard.

The serial terminal must send a line ending when you send the command.

### 3. Configure the Key

Send a command in the following format:

`SETKEY <key>`

_There must be one space between SETKEY and the key name._

**Configure a Normal Key**

Letters and numbers can be configured directly.
  
Example:

`SETKEY A`
  
`SETKEY 9`

**Configure a Special Key**

The following special keys are supported:
`ENTER, 
TAB, 
ESC, 
SPACE, 
BACKSPACE, 
DELETE, 
CTRL, 
SHIFT, 
ALT, 
GUI, `

Example:

`SETKEY ENTER`

**Configure a Key Combination**

Example:
  
`SETKEY CTRL+C`
  
`SETKEY CTRL+SHIFT+A`

_Use the + character to combine multiple keys._

**Command Response**

When a valid configuration is received, the keyboard sends a confirmation message through the USB serial port.
  
Example:

`New key set: CTRL+C`

If the key configuration is invalid, the keyboard sends:

`Invalid key`
  
**Only one key configuration is stored at a time.**
  


