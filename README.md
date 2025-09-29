# TCP UART Bridge

* [wt32-eth01 board info](https://github.com/egnor/wt32-eth01)
* ESP IDF `v5.4.2`

Simple replacement for the default firmware (the one with AT commands) that focuses on
providing TCP to UART bridge functionality, written by Gemini.
Can be useful to hook up devices that already have UART IO to a network.

## Programming

Use usb-to-serial adapter on TX0, RX0.

* IO0 to GND to program
* remove IO0 jumper and touch EN to ground to reset

## Config

Run `idf.py monitor` and then `help` for config commands.
