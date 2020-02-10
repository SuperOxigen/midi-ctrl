# Contributing to MIDI Controller Library

The library is intended to be used in micro-controller based projects
which aim to be MIDI compatible.  However, the library _should_ also
be usable on any system.

Best ways to contribute are:

1.  Report any bugs (including spelling mistakes)
2.  Fix a bug
3.  Request better code comments / documentation
4.  Request a feature
5.  Suggestions on making project's API more usable for others

## Code of Conduct

This project is a fun project, let's keep it this way.  General guidelines are:

1.  Be respectful
2.  Be constructive in your feedback
3.  Don't be aggressive / abusive towards contributers

## Reporting Bugs

Please submit all bugs and feature requests through this project's
issues page.

> TODO: Link to GitHub issue page.

### Diagnositc Details

When posting a bug report, please include the following information:

*   The commit number from `master` branch (at least the first 8 characters)
*   Summary of the bug
*   Steps to reproduce error (if applicable)
    *   If it is a run-time bug, include an example of how you call
        the API and any setup you provided
    *   If it is a compile-time bug, include the error message from your
        compiler
*   Your system information:
    *   Your target micro controller
    *   Your OS version (if building for an OS environment)
    *   Compiler version.  Output of `gcc --version`
*   Any other information that you beleive is relevant in fixing the issue(s)

## How to make a Pull Request

Simple process:

1.  Fork the repo
2.  On your fork, create a branch off of `master`
3.  Add code, fix bugs, change docs, etc.
4.  Include test code for your changes if applicable
5.  Run existing unit test to ensure nothing else breaks
6.  Issue a pull request (on your development branch)
7.  Respond to our feedback
8.  If everything is good, I'll accept your request, run some tests, then
    merge your branch with `master`.

### Coding Style

You'll probably notice that my coding style isn't _completely_ conistent, but
you can likely see the major trends.

### Project License

All contributions to this project will be published in this repo under the
[MIT License](https://opensource.org/licenses/MIT).
