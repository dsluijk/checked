# checked

Checkers implementation over SSH written in C.

## Dependencies

This project has a few dependencies, make sure to install them before proceeding:

- `libssh`
- `make`

## Running

This guide is for Linux only, you're on your own for now if you are using Windows.
Run make, everything should compile and link on it's own.

```bash
make -j8
```

Now you can run the application:

```bash
./build/checked-0.0.1-x86_64
```

You can connect to the server by ssh:

```bash
ssh player1@localhost -p 4242
```
