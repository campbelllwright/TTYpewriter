# **TTYpewriter!**
Converting electronic typewriters to terminals


## Current Hardware in use:
  - Arduino Mega R3 + Wifi.
    - ATMega2560: handles the interfacing to the typewriter, should act as a dumb terminal in it's own right.
    - ESP8266: bridges our dumb terminal to an external webserver.

  - Canon Typestar 7 Typewriter
      - This can be any digital typewriter - I chose this because it was cheap, but there's a lot of information about it online thanks to [Alex Dueppen](https://ajd.sh/posts/canon-typestar-part-1/)


## Plan/progress
  - Stage One: Keyboard
    - [x] Read and interpret keystrokes from typewriter keyboard, print to serial
    - [ ] Implement function keys ('code', 'backspace', 'left'/'right', etc.)
  - Stage Two: Dumb Terminal
    - [x] Set up basic terminal on mega
    - [ ] Detect Keyboard scan pulses and set corresponding rows/cols high to simulate keystrokes
    - [ ] add ability to switch between typewriter (passthrough) and terminal (only print RX/echo) modes
  - Stage Three: TTY
    - [ ] Set up client for protocol of choice (TBD, possibly Telnet or SSH) on ESP8266.

  - Other Goals:
    - [ ] Expand Keyboard maps to provide compatibility for more typewriters
