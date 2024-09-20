# Varasto server C++ implementation

C++17 implementation of [Varasto server]. Work in progress.

[Varasto server]: https://www.npmjs.com/package/@varasto/server

## Compilation

```bash
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

Create directory where the data will stored into, then launch `varasto-server`
with the directory as argument, such as:

```bash
$ mkdir data
$ varasto-server ./data
```

By default port `8080` will be used. This can be overridden with `-p` switch.

## TODO

- Caching.
- SSL support.
- Basic authentication support.
