# ATmega328P Microcontroller Documentation

[Home](../README.md) | [Docs Summary](./Index.md)

## Summary

The ATMEL ATmega328P is a 28-pin AVR based microcontroller.  This model of
microcontroller is the same used in the Arduino UNO boards.

Quick Specs:
* Program memory (read-only) 32kB
* SRAM 2kB
* EEPROM 1kB
* Internal oscillator 1MHz
* External oscillator 16MHz (typical), 20MHz (max)
* Single USART
  * 5, 6, 7, 8 or 9 data bits (8 for MIDI)
  * 1 or 2 stop bits (1 for MIDI)
* SPI interface (used for programming)
* 6 ADC channels
  * 10-bits resolution
* 6 PWM channels
  * Requires timers
* Temperature range: -40C to 85C
* 2x 8-bit timers
* 1x 16-bit timers

## Pin Outs

```
             +------------+
    RST/PC6 -|    \__/    |- C5/ADC5
      RX/D0 -|            |- C4/ADC4
      TX/D1 -|            |- C3/ADC3
         D2 -|            |- C2/ADC2
         D3 -|            |- C1/ADC1
         D4 -|            |- C0/ADC0
        VCC -|   ATMEL    |- GND
        GND -| ATmega328P |- AREF
   XTAL1/B6 -|            |- AVCC
   XTAL2/B7 -|            |- B5/SCK
         D5 -|            |- B4/MISO
         D6 -|            |- B3/MOSI
         D7 -|            |- B2/SS
    CLKO/B0 -|            |- B1
             +------------+
```

| Pin     | Special  | Function                                |
|:--------|:---------|:----------------------------------------|
| `PB0`   | `CLKO`   | General IO / Clock output (see `CKOUT`) |
| `PB1`   |          | General IO                              |
| `PB2`   | `SS`     | General IO / SPI Slave Select           |
| `PB3`   | `MOSI`   | General IO / SPI Master Out-Slave In    |
| `PB4`   | `MISO`   | General IO / SPI Master In-Slave Out    |
| `PB5`   | `SCK`    | General IO / SPI Serial Clock           |
| `PB6`   | `XTAL1`  | Crystal Resonator                       |
| `PB7`   | `XTAL2`  | Crystal Resonator                       |
| `PC0-5` | `ADC0-5` | Analog to Digital Converter             |
| `PC6`   | `RST`    | Reset (active low)                      |
| `PD0`   | `RXD`    | Serial Receiver                         |
| `PD1`   | `TXD`    | Serial Transmitter                      |
| `PD2-7` |          | General IO                              |

## Fuses

Fuses are by default high (unprogrammed), and are set low (programmed).
They are active low.  Below are different fuses and the recommended
values to use for this project.

| Ext. Fuse   | Bit No | Description                    | Value/Action   |
|:------------|--------|:-------------------------------|:---------------|
| ~           | 7      | ~                              | 1              |
| ~           | 6      | ~                              | 1              |
| ~           | 5      | ~                              | 1              |
| ~           | 4      | ~                              | 1              |
| ~           | 3      | ~                              | 1              |
| `BODLEVEL2` | 2      | Brown-out Detector             | 1              |
| `BODLEVEL1` | 1      | Brown-out Detector             | 1              |
| `BODLEVEL0` | 0      | Brown-out Detector             | 1              |

| High Fuse   | Bit No | Description                    | Value/Action   |
|:------------|--------|:-------------------------------|:---------------|
| `RSTDISBL`  | 7      | External Reset Disable (`PC6`) | 1 -> enabled   |
| `DWEN`      | 6      | debugWIRE Enable               | 1              |
| `SPIEN`     | 5      | Enable Serial Downloading      | 0 -> enable    |
| `WDTON`     | 4      | Watchdog Timer Always On       | 1              |
| `EESAVE`    | 3      | Preserve EEPROM on chip erase  | 1              |
| `BOOTSZ1`   | 2      | Select Boot Size               | 0              |
| `BOOTSZ0`   | 1      | Select Boot Size               | 0              |
| `BOOTRST`   | 0      | Select Reset Vector            | 1              |

| Low Fuse    | Bit No | Description                    | Value/Action   |
|:------------|--------|:-------------------------------|:---------------|
| `CKDIV8`    | 7      | Divide clock by 8              | 1 -> no divide |
| `CKOUT`     | 6      | Clock output                   | 1 -> no output |
| `SUT1`      | 5      | Select start-up time           | 1              |
| `SUT0`      | 4      | Select start-up time           | 0              |
| `CKSEL3`    | 3      | Select clock source            | 0              |
| `CKSEL2`    | 2      | Select clock source            | 1              |
| `CKSEL1`    | 1      | Select clock source            | 1              |
| `CKSEL0`    | 0      | Select clock source            | 0              |

| Fuse    | Value | Description                            |
|:--------|-------|:---------------------------------------|
| `EFUSE` | 0xFF  |                                        |
| `HFUSE` | 0xD9  | PC6 is RST, serial programming enabled |
| `LFUSE` | 0xE6  | No divide, external oscillator         |

### Programming Fuses

The two ways to set fuses, either programmatically or with `avr-dude` from
the commandline.

```C
#include <avr/io.h>

/* Puts fuses in ".fuse" section. */
FUSES = {
  /* Possible LFUSE_DEFAULT */
  .low = (FUSE_SUT0 & FUSE_CKSEL3 & FUSE_CKSEL0),
  /* Possible HFUSE_DEFAULT */
  .high = (FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTSZ1),
  .extended = EFUSE_DEFAULT
};
```

```bash
$ avrdude -c usbasp -p m328p -u -U lfuse:w:0xE6:m  # low
$ avrdude -c usbasp -p m328p -u -U hfuse:w:0xD9:m  # high
$ avrdude -c usbasp -p m328p -u -U efuse:w:0xFF:m  # extended
```

### Reading Fuses

```bash
$ avrdude -c usbasp -p m328p -u -U lfuse:r:-:h  # low
$ avrdude -c usbasp -p m328p -u -U hfuse:r:-:h  # high
$ avrdude -c usbasp -p m328p -u -U efuse:r:-:h  # extended
```

## Universal Sync. and Async. serial Rx. and Tx. (USART)

The ATmega328P has specialized hardware for communicating using the USART
protocol.  Some AVR systems have multiple serial interfaces, the ATmega328P
only has one (`USART0`).

MIDI serial requirements:

* Baud rate: 31250 bps
* Frame size: 10 bits
  * 1 start bit
  * 8 data bits
  * No parity bit
  * 1 stop bit
* Max data rate: 3125 bytes / second
* Asynchronous (no clock signal)

### USART Registers

The following registers are used for controlling the USART interface.
When initializing, `UCSR0B` should be set last.

| Register | Name                            |
|:---------|:--------------------------------|
| `UDR0`   | USART I/O Data Reg.             |
| `UCSR0A` | USART Control and Status Reg. A |
| `UCSR0B` | USART Control and Status Reg. B |
| `UCSR0C` | USART Control and Status Reg. C |
| `UBRR0`  | USART Baud Rate Reg. (12-bits)  |

| `UCSR0A` | Bit No | Description            | Value/Action                            |
|:---------|:-------|:-----------------------|:----------------------------------------|
| `RXC0`   | 7      | Receive Complete       | 1 -> unread data in `UDR0`              |
| `TXC0`   | 6      | Transmit Complete      | 1 -> data frame sent                    |
| `UDRE0`  | 5      | Data Reg. Empty        | 1 -> transmit `UDR0` is empty           |
| `FE0`    | 4      | Frame Error            | 1 -> received data had frame error      |
| `DOR0`   | 3      | Data OverRun           | 1 -> unread data and new frame incoming |
| `UPE0`   | 2      | Parity Error           | 1 -> Bad parity *(not used in MIDI)*    |
| `U2X0`   | 1      | Double USART Rate      | Disabled (2 samples per bit) -> 0       |
| `MPCM0`  | 0      | Multi-Proc. Comm. Mode | Disabled -> 0                           |

| `UCSR0B` | Bit No | Description                      | Value/Action                       |
|:---------|:-------|:---------------------------------|:-----------------------------------|
| `RXCIE0` | 7      | Rx Complete Interrupt Enable     | Enabled -> 1                       |
| `TXCIE0` | 6      | Tx Complete Interrupt Enable     | Disabled -> 0                      |
| `UDRIE0` | 5      | Tx `UDR0` Empty Interrupt Enable | Enabled -> 1                       |
| `RXEN0`  | 4      | Receiver Enable                  | Enabled -> 1 (overrides `RX0` pin) |
| `TXEN0`  | 3      | Transmitter Enable               | Enabled -> 1 (overrides `TX0` pin) |
| `UCSZ02` | 2      | Character Size (bit 2)           | 8-Bit -> 0                         |
| `RXB80`  | 1      | Receiver Data Bit 8              | *unused*                           |
| `TXB80`  | 0      | Transmitter Data Bit 8           | *unused*                           |

| `UCSR0C`  | Bit No | Description               | Value/Action                     |
|:----------|:-------|:--------------------------|:---------------------------------|
| `UMSEL01` | 7      | USART Mode Select (bit 1) | Async Mode -> 0                  |
| `UMSEL00` | 6      | USART Mode Select (bit 0) | Async Mode -> 0                  |
| `UPM01`   | 5      | Parity Mode (bit 1)       | Disabled -> 0                    |
| `UPM00`   | 4      | Parity Mode (bit 0)       | Disabled -> 0                    |
| `USBS0`   | 3      | Stop Bit Select           | 1-Bit -> 0                       |
| `UCSZ01`  | 2      | Character Size (bit 1)    | 8-Bit -> 1                       |
| `UCSZ00`  | 1      | Character Size (bit 0)    | 8-Bit -> 1                       |
| `UCPOL0`  | 0      | Clock Polarity            | Disabled -> 0 *(sync mode only)* |

### USART Interrupts

There are 3 interrupts relevant for sending and receiving data using the USART
interface.  The vector names are available in *avr-libc* header
`<avr/interrupt.h>` and are specific to ATmega328P.

* `USART_RX_vect` - Called when a frame has been received and data is ready
  to be read from `UDR0`.
* `USART_TX_vect` - Called when the last bit of a frame has been sent.
  * Not used for library implementation of the MIDI protocol.
* `USART_UDRE_vect` - Called when mode data can be written to `UDR0` for
  transmission.

### Baud Rate Register Settings

The baud rate is determined by an internal counter/timer which counts down from
`UBRR0` to 0.  Each wrap around triggers a symbol to be sent.

Equation: `UBRR0 = (F_CPU / (16 * BAUD_RATE)) - 1`

| CPU Speed | `UBRR0` Value  |
|:----------|:---------------|
| < 1 MHz   | *not possible* |
|   1 MHz   | 1              |
|   2 MHz   | 3              |
|   4 MHz   | 7              |
|   5 MHz   | 9              |
|   6 MHz   | 11             |
|   8 MHz   | 15             |
|  10 MHz   | 19             |
|  12 MHz   | 23             |
|  16 MHz   | 31             |
|  20 MHz   | 39             |

### Sample Implementation for MIDI

The following is a simplified implementation of USART receiver and transmitted
for the ATmega328P.

```C
#include <avr/interrupt.h>
#include <avr/io.h>

ISR(USART_RX_vect) {
  if (UCSR0A & _BV(RXC0)) {
    uint8_t const data = UDR0;
    PushReceiveBuffer(data);
  }
}

ISR(USART_UDRE_vect) {
  if (HasTransmitData()) {
    UDR0 = PopTransmitBuffer();
  } else {
    /* Disable interrupt, otherwise thrashing may occur. */
    UCSR0B &= ~_BV(UDRIE0);
  }
}

void SystemSerialInitialize(void) {
  cli();
  /* Calibrate 31250 bps for 16MHz CPU speed */
  UBRR0 = 39;
  /* Regular speed, no multi-proc. */
  UCSR0A = ~(_BV(U2X0) | _BV(MPCM0));  /* Intentionally write 1 to other bits */
  /* Async, no parity, 1 stop bit, 8-bit data */
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
  /* Enabled USART interface, allow Rx interrupts.
   * Note that register B is set last. */
  UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
  /* UDRIE0 must be enabled when data ready to be sent. */
  sei();
}
```

## System Time via Timer 0

The global system time is initialized as 0 seconds and 0 nanoseconds at power
up, and then is incremented by a fixed amount (equivalent to the period) every
timer interval.

### Timer 0 Registers

| Register | Name                                |
|:---------|:------------------------------------|
| `TCCR0A` | Timer/Counter Control Register A    |
| `TCCR0B` | Timer/Counter Control Register B    |
| `TCNT0`  | Timer Control Register (unused)     |
| `OCR0A`  | Output Compare Registers A          |
| `OCR0B`  | Output Compare Registers B (unused) |
| `TIMSK0` | Timer Interrupt Mask Register       |
| `TIFR0`  | Timer Interrupt Flag Register       |

| `TCCR0A` | Bit No | Description                         | Value/Action      |
|:---------|:-------|:------------------------------------|:------------------|
| `COM0A1` | 7      | Compare Match Output A Mode (bit 1) | Disconnected -> 0 |
| `COM0A0` | 6      | Compare Match Output A Mode (bit 0) | Disconnected -> 0 |
| `COM0B1` | 5      | Compare Match Output B Mode (bit 1) | Disconnected -> 0 |
| `COM0B0` | 4      | Compare Match Output B Mode (bit 0) | Disconnected -> 0 |
|    ~     | 3,2    |                   ~                 |         ~         |
| `WGM01`  | 1      | Waveform Generation Mode (bit 1)    | CTC Mode -> 1     |
| `WGM00`  | 0      | Waveform Generation Mode (bit 0)    | CTC Mode -> 0     |

| `TCCR0B` | Bit No | Description                      | Value/Action     |
|:---------|:-------|:---------------------------------|:-----------------|
| `FOC0A`  | 7      | Force Output Compare A           | No function -> 0 |
| `FOC0B`  | 6      | Force Output Compare B           | No function -> 0 |
|    ~     | 5,4    |                 ~                |         ~        |
| `WGM02`  | 3      | Waveform Generation Mode (bit 2) | CTC Mode -> 0    |
| `CS02`   | 2      | Clock Select (bit 2)             | Div 8 -> 0       |
| `CS01`   | 1      | Clock Select (bit 1)             | Div 8 -> 1       |
| `CS00`   | 0      | Clock Select (bit 0)             | Div 8 -> 0       |

| `TIMSK0` | Bit No | Description                             | Value/Action  |
|:---------|:-------|:----------------------------------------|:--------------|
|    ~     | 7-3    |                     ~                   |       ~       |
| `OCIE0B` | 2      | Output Compare Match B Interrupt Enable | Disabled -> 0 |
| `OCIE0A` | 1      | Output Compare Match A Interrupt Enable | Enabled -> 1  |
| `TOIE0`  | 0      | Timer Overflow Interrupt Enable         | Disabled -> 0 |


| `TIFR0` | Bit No | Description                          | Value/Action            |
|:--------|:-------|:-------------------------------------|:------------------------|
|    ~    | 7-3    |                   ~                  |            ~            |
| `OCF0B` | 2      | Output Compare Match B Flag (unused) | 1 -> `TCNT0` >= `OCR0B` |
| `OCF0A` | 1      | Output Compare Match A Flag          | 1 -> `TCNT0` >= `OCR0A` |
| `TOV0`  | 0      | Timer Overflow Flag                  | 1 -> `TCNT0` overflow   |

### Timer 0 Interrupt

For the system time, the only timer interrupt that is needed is
`TIMER0_COMPA_vect`.

### Timer Settings

The timer interval is choosen to avoid thrashing.

| CPU Speed | Cycles | T0 Frequency | System Time Resolution |
|:----------|:-------|:-------------|:-----------------------|
|    1 MHz  |   400  |    2.5 kHz   |              400.0 us  |
|    2 MHz  |   400  |    5.0 kHz   |              200.0 us  |
|    4 MHz  |   400  |   10.0 kHz   |              100.0 us  |
|    5 MHz  |   400  |   12.5 kHz   |               80.0 us  |
|    6 MHz  |   600  |   10.0 kHz   |              100.0 us  |
|    8 MHz  |   400  |   20.0 kHz   |               50.0 us  |
|   10 MHz  |   400  |   25.0 kHz   |               40.0 us  |
|   12 MHz  |   600  |   20.0 kHz   |               50.0 us  |
|   16 MHz  |   400  |   40.0 kHz   |               25.0 us  |
|   20 MHz  |   400  |   50.0 kHz   |               20.0 us  |

### Sample System Time

```C
#include <avr/interrupt.h>
#include <avr/io.h>
#include "system_time.h"

static system_time_t sSystemTime = {
  .seconds = 0,
  .nanoseconds = 0
};

#define RESOLUTION_NS   25000L /* 25us -> F_T0 = 40 kHz */
ISR(TIMER0_COMPA_vect) {
  SystemTimeIncrementNanoseconds(&sSystemTime, RESOLUTION_NS);
}

void SystemTimeInitialize(void) {
  /* Calibrate 400 cycles for 16 MHz CPU */
  cli();
  TCCR0A = _BV(WGM01);     /* CTC Mode.  Disconnected output signals. */
  OCR0A = 49;              /* 400 cycles per interrupt. */
  TIFR0 = 0;               /* Clear timer interrupts */
  TIMSK0 = _BV(OCIE0A);    /* Enable TIMER0 interrupt */
  TCCR0B = (0x2 << CS00);  /* Div 8.  Set last, enables timer. */
  sei();
}

bool_t SystemTimeNow(system_time_t *time) {
  cli();
  time->seconds = sSystemTime.seconds;
  time->nanoseconds = sSystemTime.nanoseconds;
  sei();
  return true;
}
```
