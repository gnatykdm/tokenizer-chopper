# Tokenizer Chopper

Tokenizer Chopper is a simple C++ implementation of a text tokenizer and detokenizer using Byte Pair Encoding (BPE). This project utilizes Clang and Visual Studio Code to process input text, tokenize it into subword units, and then detokenize it back to its original form.

## Features

- **Tokenization**: Splits the input text into token units based on BPE.
- **Detokenization**: Reconstructs the original text from the tokenized form.
- **Customizable Tokenization**: The number of merges for the BPE algorithm can be adjusted for finer control over tokenization.

## Usage

1. Clone the repository:
    ```bash
    git clone https://github.com/gnatykdm/tokenizer-chopper.git
    cd tokenizer-chopper/src
    ```

2. Build the project using Clang:
    ```bash
    clang++ main.cpp bpe.cpp -o tokenizer
    ```

3. Run the tokenizer:
    ```bash
    ./tokenizer
    ```
