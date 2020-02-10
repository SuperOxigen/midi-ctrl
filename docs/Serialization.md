# MIDI Message Serialization and Deserialization

[Home](../README.md) | [Docs Summary](./Index.md) || [Messages](./404.md)

## Summary

All of the MIDI messages that are available in this library are based standard
messages defined in the 1.0 specification.

All (de)serialization _should_ be done using `midi_message_t`, and not directly
through the specialized (de)serialization function.

Regardless of which serialization or deserialization function you use, they
all follow the same conventions.  Serialization and deserialization are
slightly different from each other.

Use the transceiver module to make serializing and deserializing easier.

## Serialization

From **message** struct to **byte array**.

Serialization is the process of taking a _valid_ MIDI message struct and
converting it into it's standard binary representation.

The general serialization function prototype is:

```C
size_t MidiSerializeSomeType(
  midi_some_type_t const *some_type, /* Additional input parameters */
  uint8_t *data, size_t data_size);
```

Parameters:
*   `some_type`: The message / sub-message component being serialized
*   Some serialization functions have additional arguments when data from
    a higher level required.
*   `data`: Output buffer to store serialized data.
*   `data_size`: The size of the `data` buffer.  See note below.

The `data` and `data_size` _may_ be set to `NULL` and 0 (respectively), along
with a _valid_ message/sub-message, and the required size to serialize to
message will be returned.

Return values:
*   If the inputs are valid, then function return the number of bytes required
    for successful serialization.
    *   Only if the value return is less than or equal to `data_size`, has the
        message actually been serialized.
    *   **IMPORTANT**: if the returned value is larger than `data_size`, this
        implies that serialization was **NOT** completed fully, and `data`
        _should_ be treated as garbage.
*   Returns _zero_ if the provided message/sub-message is invalid in anyway,
    or if `data` is `NULL` and `data_size` is not 0.

### Example of Serialization

The following is an example of serializing a note on message.

```C
uint8_t buffer[8];
size_t const res = MidiSerializeMessage(message, false, buffer, sizeof(buffer));
/* Handle invalid message */
if (res == 0) {
  ...
}
/* Handle too small of buffer */
if (res > 8) {
  ...
}
/* Successful serialization. */
if (res > 0 && res <= 8) {
  ...
}
```

If programming with C++, one can do the following to ensure that valid messages
are alway serialized correctly.

```C++
bool_t SerializeMessageToVector(
    midi_message_t const *message, std::vector<uint8_t> *data) {
  if (message == nullptr || data == nullptr) return false;
  size_t const message_size = MidiSerializeMessage(message, false, NULL, 0);
  if (message_size == 0) {
    LOG_ERROR("Invalid message");
    return false;
  }
  data->resize(message_size);
  return MidiSerializeMessage(message, false, data->data(), data->size()) > 0;
}
```

## Deserialization

Deserialization: From **byte array** to **message**.

Deserialization is the process of converting a binary MIDI message and
converting it into a message/sub-message struct.  For each message /
sub-message type, it is upto the caller to know which deserialization
function to call.  In most cases, you will only need to call
`MidiDeserializeMessage()`; this function will call the appropriate
deserialization function of the message's sub-messages.

The general serialization function prototype is:

```C
size_t MidiDeserializeSomeType(
  uint8_t const *data, size_t data_size, /* Additional input parameters */
  midi_some_type_t *some_type)
```

Parameters:
*   `data`: Input buffer containing binary MIDI message / sub message.
    This value cannot be `NULL`.
*   `data_size`: The number of bytes available in `data` buffer.
*   Some deserialization functions have additional arguments which can
    effect how the data is deserialized.  Read the doc string for the
    deserialization function to understand how it works.
*   `some_type`: The message / sub-message struct to be populated.

Return values:
*   On successful deserialization, the total number of bytes that were used
    to deserialize the message are returned.  If successful, this value will
    be less than or equal to `data_size`.
*   If the message is _incomplete_ (ie, not enough bytes are available for
    the given message type) then the value returned will be greater than
    `data_size`.
    *   **Note**: Some message / sub-message types are variable in length.
        To address this, the value returned _may_ be the minimum number of
        bytes required before the actual size is determined.  It is
        _recommended_ that the caller treat all returned values larger than
        `data_size` as simply "not enough data", and not a specific
        number of bytes required for deserialization.
*   Returns _zero_ if the MIDI data is not valid for the expected type of
    of message, or if `some_type` is `NULL`.

Typically, device / program using this library does not have full control
over the source of the MIDI message data, which _could_ be invalid.
The deserialization functions will fail if the data provided is invalid;
however, the information returned to the caller is limited.  In the event
that the MIDI data is invalid, the caller must discard all bytes current
MIDI message (message type byte and all immediately trailing data bytes)
until the next message type byte.  A special case is in the event that a
_SysEx_ message was being deserialized, in which case the _EndOfSysEx_
at the end of the data bytes must also be discarded.

### Example of Deserialization

The following example shows how to deserialize a MIDI message.  This
example assumes that MIDI bytes are receive single bytes at a time.

```C
midi_message_t message;
uint8_t buffer[16];
size_t count = 0;
size_t res;
do {
  buffer[count++] = GetMidiByte();
  res = MidiDeserializeMessage(buffer, count, MIDI_NONE, &message);
} while (res != 0 && res > count);
/* Handle bad data */
if (res == 0) {
  ...
}
/* Handle good case */
if (res <= count) {
  ...
}
```

## Simplify Using the Receiver / Transmitter Module

Serializing and deserializing MIDI messages is further complicated by mild
data dependencies between messages.  To make serializing and deserializing
easier for the programmer a transceiver module is available to simplify the
process.
