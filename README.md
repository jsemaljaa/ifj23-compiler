# IFJ23 Imperative Language Compiler
### Note: It's essentially the same project as [this one.](https://github.com/jsemaljaa/ifj22-compiler) Different from last year, this implementation is written for the IFJ23 language. Otherwise, project structure and modules are pretty much the same.

### You can look at the official documentation of this project [here](https://github.com/jsemaljaa/ifj23-compiler/blob/main/docs/dokumentace.pdf)

## Usage

Compile the entire application:
```
$ make
```
Run:
```
$ ./compiler <input
```
where input is a file containing IFJ23 code (see `examples` folder)

## Important info
* Due to certain communication difficulties, I am the sole author of the entire implementation (except for the files [utils.h](https://github.com/jsemaljaa/ifj23-compiler/blob/main/src/utils.h) and [log.h](https://github.com/jsemaljaa/ifj23-compiler/blob/main/src/log.h))
* Because of this, **there is no generator in my implementation.**

## Tests
* Lexical analysis (error detection): **65%**
* Syntactic analysis (error detection): **83 %**
* Semantic analysis (error detection): **35 %**
