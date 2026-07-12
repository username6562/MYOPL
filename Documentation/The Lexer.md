
The Lexer is the first part of the interpreter. It reads the source code , splits all the words , symbols , etc  , into tokens for [[The Parser]]

## What are Tokens?

Tokens are meaningful units of data with values and a type.
```c
typedef struct {
    char value[MAXTOKENLEN];
    TokenType type;
} Token;

```

# How It Works

When our compiler reads a code like "int x = 5;". The lexer moves through 3 phases
```text
[Text bag]-> "int x = 5;"

[Gets Each Word]-> ["int"] , ["x"] , ["="] , ["5"]

[Turns Them Into Tokens]->("KeyWordToken , "int") , ("IdentifierToken","x")...
```

# Lexer Pipeline

In the background the `GetToken(FILE * file)` function does a continuous rules:
**1. Clears Blank Spaces : The code uses the C's standard library function `isspace()`** to check  for spaces ,empty lines and tabs. It ignores them so file can focus on more important characters

**2. Looks for End of File : It checks if the files has reached its end and  returns and `EOFToken` to tell the Parser that the file has reached its end

**3. Looks for Symbols : It runs quick character checks to look grab individual operators:
- `;` -> `SemiColonToken`
- `(`-> `OpenParenthesisToken`
- `)`-> `CloseParenthesisToken` ....

**4. Checks For Letters `isalpha` : Checks for a a letter then runs a loop to check for all the characters till the word ends . Then it goes to check:
- If the word matches `"int"` `"bool"` `"string"` -> `KeyWordToken`
- If the word matches `"if"` -> `IfToken`
- If the word matches `"else"` -  `ElseToken`
- If the word matches `"true` or `false` -> `BoolToken`
- Anything else -> `IdentifierToken` (A variable name)

**5. Gathers Digits `isdigit` : It looks for a digit then runs a mini loop to check for all possible numbers and returns an `NumberToken`**

**6. Special Check For `=` : A special check is made for `=` ,  `==`  and other Comparision Operator because without it Lexer will treat `==` as two `EqualsToken` instead of `ComparisonOpToken` 

# The Character Pipeline & `ungetc`

In our Lexer `ungetc` is used for the purpose of not skipping characters in our buffer

## The Core Problem

In C reading  a file stream using `fgetc`  behaves like a **one way conveyor belt**
- Once a character is read there is no way it can be gotten back without using `ungetc`

# Example

If a user types a mathematical string `80+5;` and there is no `ungetc` at the end this is what the pipeline would look like:
- The loop starts get `8` . 8 is a digit so it begins the inner loop and checks and stores it in the `lexerBuffer`.
- The inner loop starts gets the next character using `fgetc` and the next character is `0` . it then stores 0 in the `lexerBuffer` , now the `lexerBuffer` looks like this `80`
- The Lexer doesn't know whether the next character is a so it goes to the next char and picks the next one and finds that is is `+` and closes the inner loop and return `(IntToken , 80`)

**The Problem** : Because `+` has already been taken out from the loop begins again going to the next character without running a lexical analysis on `+` . 
The Parser will look for the operator and not find the `BinaryOpToken`


