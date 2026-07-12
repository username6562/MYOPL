An **Abstract Syntax Tree** (ast) is like a bridge between the compilers front end (lexer) with the backend (the interpreter)


It is called abstract cause it removes all the extra punctuation and pieces of the code
- When an AST is made it ignores things like:
  -  Semicolons
  - Parenthesis
  - Brackets

# Visualization of an AST

If the user types something like `2 + 3 * 2 + 3`
The AST turns it into a tree like structure
```text
				+
			/      \
			+       3
		/      \
		2       *
			/        \
			3         2
```



