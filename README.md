# Markov Text Generator

Text generator based on Markov chain. Consists of 2 stages: learn stage to create a Markov chain of order N, and use stage to generate new text using this Markov chain. English ASCII and UTF-8 as well as Russian UTF-8 texts are supported.


# Build

Linux only. Just run `make` in the root directory of the project. `bin` and `obj` subdirs will be created. All executables will be saved into `bin` subdir.

## Prequisites

    C++ compiler with C++11 support.
    curl util.


# Testing
Run `test` executable for autotests.


# Usage

`stage_learn` supports following command line options:

    -n, --order <chain order>
Order of markov chain to build. Must be positive.

    -o, --output
File to output Markov chain to, std::cout will be used if not provided.

    -h, --help
Show help message and exit.

All other options will be treated as URLs for text files. Example:

    stage_learn -n 3 -o chain.txt "https://dl.pushbulletusercontent.com/qLE2ofZ55IVCUsKatIam9QRO6X7CynGf/Alice_rus.txt" "https://dl.pushbulletusercontent.com/P5JVQzsG7U3SKUXYvy1Nfy4VeR12REfD/Margarita_rus.txt"


`stage_use` supports following command line options:

    -w, --words
Number of new words to generate. Must be positive.

    -i, --input
File to load Markov chain from, std::cin will be used if not provided.

    -h, --help
Show help message and exit.

Example:

    stage_use -w 2000 -i chain.txt в белом плаще



