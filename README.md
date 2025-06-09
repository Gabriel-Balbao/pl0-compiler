# C Lexical Analyzer Project

## Description

This project implements a lexical and syntatic analyzer (scanner) in C. It reads a source code file, processes it character by character, and groups sequences of characters into tokens.

The analyzer uses a Deterministic Finite Automaton (DFA). The DFA's transition table is generated from a JFLAP (`.jff`) file using a Python script and stored in a CSV file (`../data/var.csv`). The C code then reads this CSV to perform the lexical analysis on the input source code.

## Prerequisites

To build and run this project, make sure you have installed:

*   **Make:** A build automation tool (usually pre-installed on Linux/macOS).
*   **GCC:** The GNU Compiler Collection (or another compatible C compiler like Clang).
*   **Python 3:** Required to run the `parser.py` script which generates the DFA data.

## Setup

1.  **Clone the repository:**
    ```bash
    git clone <your-repository-url>
    cd <repository-directory>
    ```

## Building the Project

To compile and run the lexical analyzer, run `make run` with the flag FILE_PATH in the project's root directory:

```bash
make run FILE_PATH=your_file_path

```

Files ./data/ex1, ./data/ex2 are offered as examples

```bash
make run FILE_PATH=./data/ex1




