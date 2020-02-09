#!/usr/bin/python3
"""MIDI Data Packet - Data Generator.

Generates the data section of a MIDI data packet given the other
packet parameters.

Copyright (c) 2020 Alex Dale
This project is licensed under the terms of the MIT license.
See LICENSE for details.
"""

# pylama:ignore=E11

import argparse
import random
import sys

from typing import List

MIDI_DATA_PACKET_DATA_LENGTH = 120
MIDI_NON_REAL_TIME_ID = 0x7E
MIDI_DATA_PACKET = 0x02


def IsDataByte(num: int) -> bool:
  """Is a MIDI Data Byte."""
  return num >= 0 and num <= 127


def uhex(num: int) -> str:
  """Uppercase Hex."""
  return "0x{:02X}".format(num)


def eprint(*args):
  """Error Print."""
  print(*args, file=sys.stderr)


def CalculateChecksum(data: List[int], did: int, pnum: int) -> int:
  """Calculate checksum, assumes all arguments are valid."""
  checksum = MIDI_NON_REAL_TIME_ID ^ MIDI_DATA_PACKET ^ did ^ pnum
  for n in data:
    checksum ^= n
  return checksum


def GetRandomData(n: int) -> List[int]:
  """Generate random bytes of data.

  This will zero pad if |n| is less than the required number of bytes
  for a MIDI data packet.  Assumes |n| is less than or equal to
  MIDI_DATA_PACKET_DATA_LENGTH.
  """
  data = [random.randint(0, 127) for _ in range(n)]
  if n < MIDI_DATA_PACKET_DATA_LENGTH:
    data.extend([0 for _ in range(n, MIDI_DATA_PACKET_DATA_LENGTH)])
  return data


def Main(args: List[str]) -> int:
  """Script Main Function."""
  parser = argparse.ArgumentParser(
    description="Generate MIDI data packet data",
    epilog="Copyright (c) 2020 Alex Dale")

  parser.add_argument(
    "-d", "--device-id",
    dest="device_id",
    type=int, default=0)
  parser.add_argument(
    "-p", "--packet-number",
    dest="number",
    type=int, default=0)
  parser.add_argument(
    "-c", "--checksum",
    dest="checksum",
    type=int, default=None)
  parser.add_argument(
    "-n", "--data-size",
    dest="size",
    help="Number of random bytes to generate",
    type=int, default=MIDI_DATA_PACKET_DATA_LENGTH)
  options = parser.parse_args(args=args[1:])

  if not IsDataByte(options.device_id):
    eprint("Invalid device ID: {:02x}".format(options.device_id))
    return 1
  if not IsDataByte(options.number):
    eprint("Invalid packet #: {:02x}".format(options.number))
    return 1
  if options.checksum is not None and not IsDataByte(options.checksum):
    eprint("Invalid checksum: {:02x}".format(options.checksum))
    return 1
  if options.size < 0 or options.size > MIDI_DATA_PACKET_DATA_LENGTH:
    eprint("Invalid data size: {}".format(options.size))
    return 1

  data = GetRandomData(options.size)
  checksum = CalculateChecksum(
    data=data, pnum=options.number, did=options.device_id)
  if options.checksum is not None:
    data[0] = data[0] ^ checksum ^ options.checksum
    checksum = CalculateChecksum(
      data=data, pnum=options.number, did=options.device_id)

  lines = []
  for i in range(len(data) // 8):
    lines.append(", ".join([uhex(n) for n in data[i*8:(i+1)*8]]))
  print("Device ID: 0x{:02X}".format(options.device_id))
  print("Packet #: 0x{:02X}".format(options.number))
  print("Data:")
  print("  " + ",\n  ".join(lines))
  print("Checksum: 0x{:02X}".format(checksum))

  return 0


if __name__ == "__main__":
  sys.exit(Main(sys.argv))
