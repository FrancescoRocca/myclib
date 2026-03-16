# myclib

A small, personal C library for learning and experimentation.  
**Not production-ready. Use at your own risk.**

## Features

Most core containers use internal locks for single-object operations.
Cross-object operations can still require caller-side synchronization.

- Hashmaps
- Strings
- Circular queues
- Vectors
- Stack
- Set

### Notes

- Hashmap keys/values are copied as raw bytes (`memcpy`) using `key_size`/`value_size`.
- For C-string keys, provide a readable key buffer of at least `key_size` bytes.
- The set module is basic and intentionally minimal.

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
