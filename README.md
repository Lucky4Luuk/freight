# Freight
A build system for C, inspired to cargo for Rust

## How to use
The main command is `freight` and all commands build on that.
- `new <name>` : Creates a new project inside the current directory (it makes a folder for the project)
- `build` : Builds the project found inside the current directory
- `run` : Invokes build, then runs the produced executable
- `help` : Shows information similar to this :)

### freight.toml
This is where you specify everything about your build process.
The main things that are required are part `[package]`
```toml
[package]
name = "freight"    # Project name
compiler = "clang"  # Project compiler
cflags = "-std=c99" # Any flags passed to the compiler
```
The goal is to have dependencies here too, so you can easily include dependencies, like how cargo lets you pull in dependencies for your rust projects. This still requires a lot of work, so sadly it won't be ready soon.
