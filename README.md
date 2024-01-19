# AmbiMod
Ambimod Modular Mouse Software Repository

Project Brief:

Mouse inputs (Left, Right, Middle + Optical sensor movements) will be sent via HID (Mouse.h).
M4-M12 will be sent via HID (Keyboard.h). Companion app will not handle either of these, as HID interfaces directly with OS. 

When first being plugged into the computer, mouse will briefly establish a Serial (COM Port) connection to the PC. This may require the Arduino to disable HID connection momentarily (unsure). 

Over this COM port, the keybinds for M4-M12 will be loaded into the companion app for the user to see. After companion app sends back a "confirm" signal over Serial, Arduino will close COM connection, and HID should automatically reestablish to send keystrokes. 

Keybinds changes from within the companion application will remain local to the app until "save button" is clicked. 

When save button is clicked, companion will first locally save binds to a JSON file. Intercommunication between Arduino and Companion should switch to serial connection. Once serial is established, companion app will encode the JSON config file and send a serialized version of the keybinds to the Arduino, where it should be decoded and mapped to the buttons. Once this is done, send a confirm signal, and have the Arduino terminate the serial connection and reestablish HID connection. 

Comprehensive To-Do:

Standard Operation (Arduino Config Only)
1. Ensure HID inputs are read at a system level (mouse clicks, scroll, optical, etc). This should only require configuration on Arduino sketch.
2. Ensure M4-M12 are assigned as arbitrary keystrokes, and ensure that they are registered (i.e. test by pressing each button while clicked into notepad).
3. When mouse plugs in, look to establish serial link on COM port. Companion will send "IDENTIFY" to serial, and when received, have arduino send a "handshake signal" to identify itself (pls specify in this document what the signal being sent is).

Loading Keybinds from Arduino
1. Once bilateral connection is established, send M4-M12 keybinds to the port.
2. Wait to receive "CONFIRM" over serial, then close COM connection from arduino side, resume HID

Companion Heavy Tasks (C# Config, Some Arduino Config Reqd)

Saving New Keybinds to Arduino
1. User will enter keyboard strokes which they wish to register to M4-M12 into the UI (MOBA module only). These changes should not be written to JSON until Save is clicked
2. Upon Save, save keymap into json file. Following this, reestablish serial connection with mouse. Once Serial established, tokenize json into serial signal, process on arduino and save new keybinds.
3. Once keybinds are saved terminate COM connection and reestablish HID to return to using mouse as a realtime pointer device.

KeyTest
1. When keytest page is open in companion, check if received keystrokes match any binds on mouse's active profile, and reflect this in mouse keytest image

Macros
1. These may no longer need a separate tab. "Macros" are limited to keyboard shortcuts up to 4 keystrokes long, and may not include pointer movement or autoclick functionality.
2. Macros may be configured as standard keybinds within the profile pages. 

   
