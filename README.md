# File Fixers

A computer-themed RPG for NES, made for the [Game Off 2021](https://itch.io/jam/game-off-2021) game jam.

## Build dependencies

- cc65 for compiling
- FamiTone 5.0 (set `NSF2DATA` and `TEXT2DATA` paths on Makefile)
- FamiTracker (set `FAMITRACKER` path on Makefile).

Intermediate sfx and soundtrack files are commited, so FamiTone and FamiTracker
are only needed if their original files are changed.

## Building

To compile a release version:

```sh
$ make
```

...or to include the commit on the rom name:

```sh
$ make release
```

To compile a debug version:

```sh
$ make debug
```

To compile a debug version and open it on an
emulator (with path defined by the `EMULATOR` variable on Makefile):

```sh
$ make run
```
