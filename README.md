# myclib

A small, personal C library for learning and experimentation.  
**Not production-ready. Use at your own risk.**

## Features

All the features listed are Thread-safe.

- Hashmaps
- Strings
- Circular queues
- Vectors
- Stack

## Installation

Clone the repo, cd into it and then install it using:

```
$ meson setup build
$ cd build
$ sudo ninja install
```

To uninstall, simply run `sudo ninja uninstall`.

## Usage

See the `test/` folder for examples.
