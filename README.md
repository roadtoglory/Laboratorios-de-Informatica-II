# Solitaire Card Game Engine

A terminal-based patience (solitaire) card game engine written entirely in **C**. The program parses custom `.paciencia` configuration files to define the rules, initialize the game, and recreate different card games directly in the terminal.

## Overview

The program reads a `.paciencia` file, describing the game setup, card layouts, valid moves, and winning conditions.

This approach allows different card games to be represented and executed using the same underlying engine, depending on the rules defined in the input text file.

The project was developed as part of an academic assignment.

## Features

- **Custom game configurations** — Load `.paciencia` files to define different card games.
- **Text parser** — Parse game definitions and interpret their configuration rules.
- **Terminal interface** — Recreate and interact with card games directly in the terminal.
- **Move validation** — Apply the movement rules specified in the configuration file.
- **Load and save** — Save a game state and load it again.
- **Undo** — Undo previous moves during gameplay.
- **Makefile support** — Compile the project and clean generated files using predefined commands.

## Requirements

- A C compiler, such as GCC.
- GNU Make.
- A terminal compatible with the program's input and output.

## Build

Clone or download this repository and navigate to the project directory.

Compile the project using the provided Makefile:

```bash
make etapa3
```

This runs the `etapa3` target defined in the Makefile and generates the executable according to the project's build configuration.

To remove generated build files, run:

```bash
make clean
```


## Academic Context

This project was developed as part of a school assignment, focusing on C programming, text parsing, game-state management, and the implementation of a configurable card game engine.

