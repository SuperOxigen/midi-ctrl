/*
 * MIDI Controller - System Serial - ATmega328P Platform
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_AVR
#ifndef _PLATFORM_ARDUINO

#include <avr/io.h>
#include <avr/interrupt.h>

#include "byte_buffer.h"
#include "system_serial.h"

/*
 *  ATmega328 Serial
 *    Set USART port to operate at 31250 bps.
 *    MIDI frame: 8 data bits, 1 stop bit, no parity.
 *
 * USART I/O Data Register (UDR0): Write -> Tx, Read -> Rx
 * USART Control and Status Register A (UCSR0A) = ------00
 * | RXC0| TXC0|UDRE0|  FE0| DOR0| UPE0| U2X0|MPCM0|
 *    Receive Complete (RXC0): 1 -> unread data in receive buffer (UDR0)
 *    Transmit Complete (TXC0): 1 -> frame complete
 *    Data Register Empty (UDRE0): 1 -> transmit buffer is empty (UDR0)
 *    Frame Error (FE0): 1 -> byte in receive buffer had frame error
 *    Data OverRun (DOR0): 1 -> receive buffers are full and new frame received.
 *    Parity Error (UPE0): 1 -> byte in receive buffer had parity error
 *    Double the USART Transmission Speed (U2X0) = 0 -> Disabled
 *    Multi-processor Communication Mode (MPCM0) = 0 -> Disabled
 * USART Control and Status Register B (UCSR0B) = 1011100-
 * |RXCIE0|TXCIE0|UDRIE0| RXEN0| TXEN0|UCSZ02| RXB80| TXB80|
 *    RX Complete Interrupt Enable (RXCIE0) = 1 -> Enabled
 *    TX Complete Interrupt Enable (TXCIE0) = 0 -> Disabled
 *    Data Register Empty Interrupt Enable (UDRIE0) = 1 -> Enabled
 *    Receiver Enable (RXEN0) = 1 -> Enabled, override Rx pin control
 *    Transmitter Enable (TXEN0) = 1 -> Enabled, overrides Tx pin control
 *    Character Size (UCSZ02) = 0
 *    Receive Data Bit 8 (RXB80) = 0 (Only in 9-bit mode)
 *    Transmit Data Bit 8 (TXB80) = 0 (Only in 9-bit mode)
 * USART Control and Status Register C (UCSR0C) = 00000110
 * |UMSEL01|UMSEL00|  UPM01|  UPM00|  USBS0| UCSZ01| UCSZ00| UCPOL0|
 *    USART Mode Select (UMSEL0[1:0]) = 00 -> Asynchronous
 *    Parity Mode (UPM0[1:0]) = 00 -> Disabled
 *    Stop Bit Selec (USBS0) = 0 -> Disabled
 *    Character Size (UCSZ0[1:0]) = 11 -> 8-bit
 *    Clock Polarity (UCPOL0) = 0 -> Disabled (only used in synchronous mode)
 * USART Baud Rate Registers (UBRR0 = UBRR0H|UBRR0L)
 *    UBRR0 = (F_CPU / (16 * BAUD_RATE)) - 1
 */

#if F_CPU == 16000000ul
#define HIGH_COUNT 0u
#define LOW_COUNT 31u
#elif F_CPU == 1000000ul
#define HIGH_COUNT 0u
#define LOW_COUNT 1u
#else
#error Unsupported CPU speed F_CPU
#endif

#ifndef SYSTEM_RX_SIZE
#define SYSTEM_RX_SIZE  64
#endif

#ifndef SYSTEM_TX_SIZE
#define SYSTEM_TX_SIZE  32
#endif

static uint8_t sSystemRxData[SYSTEM_RX_SIZE];
static byte_buffer_t sSystemRxBuffer;

static uint8_t sSystemTxData[SYSTEM_TX_SIZE];
static byte_buffer_t sSystemTxBuffer;

static bool_t sSystemSerialInitialized = false;

ISR(USART_RX_vect) {
  while (UCSR0A & _BV(RXC0)) {
    uint8_t const data = UDR0;
    ByteBufferEnqueueByte(&sSystemRxBuffer, data);
  }
}

ISR(USART_UDRE_vect) {
  if (ByteBufferIsEmpty(&sSystemTxBuffer)) {
    /* If no data to transmit, disable interrupt. */
    UCSR0B &= ~_BV(UDRIE0);
    return;
  }
  uint8_t data = 0x00;
  if (!ByteBufferDequeueByte(&sSystemTxBuffer, &data)) return;
  UDR0 = data;
}

void SystemSerialInitialize(void) {
  if (sSystemSerialInitialized) return;
  cli();
  ByteBufferInitialize(&sSystemRxBuffer, sSystemRxData, SYSTEM_RX_SIZE);
  ByteBufferInitialize(&sSystemTxBuffer, sSystemTxData, SYSTEM_TX_SIZE);
  /* UBRR0 */
  UBRR0H = HIGH_COUNT;
  UBRR0L = LOW_COUNT;
  /* Control Register A */
  UCSR0A = ~(_BV(U2X0) | _BV(MPCM0));
  /* Control Register C */
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
  /* Control Register B (enables the interrupts) */
  UCSR0B = _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0);

  sSystemSerialInitialized = true;
  sei();
}

size_t SystemSerialWrite(uint8_t const *data, size_t count) {
  if (data == NULL || count == 0 || !sSystemSerialInitialized) return 0;
  cli();
  size_t const queued = ByteBufferEnqueueBytes(&sSystemTxBuffer, data, count);
  UCSR0B |= _BV(UDRIE0);
  sei();
  return queued;
}

size_t SystemSerialRead(uint8_t *data, size_t data_size) {
  if (data == NULL || data_size == 0 || !sSystemSerialInitialized) return 0;
  cli();
  size_t const dequeued = ByteBufferDequeueBytes(
      &sSystemRxBuffer, data, data_size);
  sei();
  return dequeued;
}

void SystemSerialFlush(void) {
  if (!sSystemSerialInitialized) return;
  cli();
  ByteBufferClear(&sSystemRxBuffer);
  sei();
}

#endif  /* _PLATFORM_ARDUINO */
#endif  /* _PLATFORM_AVR */
