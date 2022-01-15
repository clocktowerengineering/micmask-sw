# micmask
sw for the mic mask project. hardware is tracked in a separate repo.

# about
one of the really cool things about the pandemic is how nobody wants to wear good masks, including elastomerics. the classic complaints about elastomerics in general are that they're hot and that communication sucks in them. i am working on how to solve hot, but communication is a shorter putt.

the overall project aims to utilize a stock esp8266 core along with a i2s mic and i2s amp driving a speaker; this can be mated with a housing that adapts to either a 3m bayonet fitting (currently designed and in work), a housing that adapts to a 3m secureclick, a housing that adapts to a honeywell, etc. this presently uses a esp8266 board, a i2s mic board, and a i2s amp board with speaker, but we aim to make a board that has all non esp8266 components and accepts a castellated 8266. this should allow us to get cost down to the <$20 range without much effort.

# proposed architecture

this is going to run within the arduino framework for now as that should allow the maximum amount of versatility in people helping and being able to use this effectively. there's a performance penalty and technical debt to be paid there, but the advantages of leveraging existing codebase and tutorials makes this the right choice for now.

i2s mic pinout

bclk - io13
lrclk - i014
sd - io12

i2s speaker pinout

bclk - io15
lrclk - i02
sd - io3

# requirements

- arduino environment
- https://github.com/esp8266/Arduino
