# QuickCalc
A quick to use calculator which is also a rudimentary programming language.

# Structure
This program is split into a few simple components

* lexer: Lexical analyzer, converts string input into tokens
* parser: Converts a sequence of tokens to an abstract syntax tree
* ast: Abstract syntax tree, stores all the operations to perform in a tree structure
* executor: Evaluates abstract syntax trees
* concepts: A library of some useful functions written in C++ exposed in the calculator

# Usage
Calculations can either be passed as a command line argument or through an interactive shell.

Statements are seperated with semicolons, which must be present when used as a shell.

Functions can be defined using a `let` statement, e.g.

```
let my_number = 10;
let add_one(x) = x + 1;
```
