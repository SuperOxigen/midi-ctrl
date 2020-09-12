# MIDI Ctrl Documentation

[Home](../README.md) | [Docs Summary](./Index.md)

## Summary

The project is _mildly_ documented, but is regularly updated to be more
so.

## Overview

> Documentation is in working progress, not all links below work.

*   [Building](./404.md): The project is built using PlatformIO.  You will
    need to install the requirements before building.
*   [Smart Strings API](./404.md): Smart Strings is a simple C string library
    which provides a slightly safer API for handling C strings.
*   [MIDI Basics](./404.md): Covers the basics of the MIDI standard, and links
    to more information externally.
*   [Message API](./404.md): The C MIDI message struct is the most complex
    part of the library; but also the most important for using the library.
*   [Time API](./404.md): MIDI time consists of both time messages and a
    system time struct.
*   [Serializing and Deserializing](./Serialization.md): A general overview
    of serializating and deserializing MIDI messages.  Most library users
    should use the *transceiver* module instead of directly calling the
    (de)serializing functions.
*   [Transceiver](./404.md): Provides a receiver and transmitter API for
    more easily receiving/deserializing and transmitting/serializing MIDI
    messages on a MIDI interface.

## Supported Microcontrollers

*   [ATmega328P](./ATmega328P.md): 8-bit AVR-based microcontroller, 32 kB
    program memory, 2 kB SRAM, 1 kB EEPROM, 1 USART interface.
