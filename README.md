# MIDI Controller

A C library for sending / receiving MIDI messages and creating a
MIDI compatible device using a micro controller.

## Summary

This MIDI library provides a set of APIs for creating, serializing and
deserializing standard MIDI messages.  In addition to the messages API, there
are additional facilities for storing and handling common device data that
most MIDI compatible devices must track.

This library is intended to be used on a micro-control; as a result, most
of the API is simple and tries to use a very small memory foot print.
This library is _not_ designed to be used in a multi-threaded environment.
Some of the API function interact with global variables which might cause
unexpected behaviour if called simultaniously.

## Documentation

> The documentation is currently in working progress.

See the [docs](docs/Index.md) for latest documentation.

## Tutorials / Example Implementations

At this time, no tutorials or example implementations are available.

---

## License

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT).
Please see [LICENSE](LICENSE) for more details.

## Contributing, But Reporting and Feature Suggestions

Check out the [CONTRIBUTING.md](CONTRIBUTING.md) doc for information on how
to contribute to the project.

If you find a bug, think a MIDI feature is missing or want to suggest
additional features that you think others would enjoy, then please open an
issue on the project's GitHub page.
