The Parser is the second stage in making the interpreter it is the bridge between [[The Lexer]] and the [[The Interpreter]]   . It is what turns tokens gotten from [[The Lexer]] int an [[ast]].



# `ParsePrimary()` Function

**`ParsePrimary`**  is the bedrock of our Pratt parsing . While `ParseExpression` is used to handle operator precedence and find the end point of out function

Every time `ParsePrimary` is called it Peeks ahead in the file stream of the code and handles tokens using these conditions:

**1. Numeric values (`IntToken`) -> Consumes token and creates a `NumberNode`
2.** Variable Identifiers (`IdentifierToken`) ->Consumes token and creates a `IdentifierNode`
**3. String values (`StringToken`) -> Consumes token and creates a `StringNode`
4.** Open Parenthesis (`OpenParenthesisToken`) -> 
- Consumes `(`
- Calls `ParseExpression(file , 0)` to solve everything inside it 
- Checks for `CloseParenthesisToken` then Consumes it
- returns the node created by `ParseExpression(file , 0)`

**After finding every token  and creating its node . `ConsumeToken()` must be called so to not affect other functions outside this one

# `ParseExpression` Function

`ParseExpression`  function is made to handle operator precedence . It is one of the main functions used to get our ASTNodes

To handle operator precedence we use something called binding values. Binding values are basically numbers giving to operators to tell them which opoerators are stronger. Binding Powers are gotten with the function `GetBindingPower(Token token)` in  `parser.c`
# `ParseExpression` Pipeline

It starts by getting the left hand side of the operator with the help of `ParsePrimary` function
Then looks at the next token in order to know operator type

*`NOTE`: Looking at next token before getting the left hand side will result in an error 
because token has not been token has has not been consumed therefore `hasLookAhead` variable is still false. And in the `ParsePrimary` function a `PeekToken` is called , and because the `hasLookAhead` variable is still false it just stays stuck  then returning a Stack Overflow error 

An loop is made to  look forward for upcoming tokens. 
It get the `nextBindingPower` with the help of  `GetBindingPower(Token token)` 

```c if (currentBindingPower >= nextBindingPower) { break; } ```

It then get Create Node   depending on the operator that was found
The left side of the operator node is  the left side gotten by the `ParsePrimary` function  

**The Recursive Leap:** It launches a brand-new copy of `ParseExpression` to solve the right side of the equation, passing the operator's own score as the new baseline: 
```c opNode->right = ParseExpression(file, nextBindingPower); ```


---
# Example
 = 
Lets use  `2 * 3 + 6 * 5` as an example:
# step 1: Grab its left side
- Current Baseline  = 0
-  `ParsePrimary` peeks at the front of file stream and get the number 2 consumes two and creates a (`NumberNode , 2`)
-    The left variable now holds number 2

# step 2: Peeks at the first Operator
Copy 1 enters its `while(true)` loop
- It peeks forward and sees `"*"` sign
- It checks its binding powers = 30
- it runs the check
```c if (currentBindingPower >= nextBindingPower) { break; } ```
- 0 is not greater than 30 so we skip
- It then checks which operator it is . Operator is `"*"`  so it creates an `MultiplyNode`

# step 3 : Copy 1 spawns copy 2

```c opNode->right = ParseExpression(file, 30); ```

# step 4: Copy 2 grabs it left side
- Copy 2 gets it's left by calling `ParsePrimary` . `ParsePrimary` looks into file stream and see the number 3 
- It consumes 3  and builds the `NumberNode - 3`

# step 5 : Copy 2 finds the next operator

Copy 2 enters it's own `while(true)` loop
- It peeks forward and sees the `+`  sign 
- It checks the next binding strength which is 20
- it runs the check
```c if (currentBindingPower >= nextBindingPower) { break; } ```
- but 30 > 20 so it breaks and copy 2 only returns 3 
# step 6: copy 1 receives 3 and inserts it into its right
- Copy 1 receives 3 from copy 2 and plugs it into the right
- and makes left the opNode
- Currently node is looks like
```text
			*
		/       \
		2       3        
```

*`NOTE` : Copy 1's loop doesn't break it was copy 2 comparing `*` binding power and `+` binding power*  
# step 7 : Copy 1 gets the next token
- Copy 1 gets the next token which is `+` therefore the next binding power is 20
- Then makes the check
```c

        if (currentBindingPower >= nextBindingPower) {
            break;
        }
```
- It checks between 0 and 20 and 0 is not greater than 20 so loop doesn't break
- Then it makes an `AdditionNode` 
- Copy 1 then get its right node by recursively calling `ParseExpression`
```c
 opNode->right = ParseExpression(file, nextBindingPower);
```

# step 8 : another copy 2 made
- Current binding power is then = 20
- It grabs the left which is 6 consumes the token and creates `NumberNode :6
- It then makes its own `while(true)` loop
- It gets the next token the which is `*` therefore the nextbindingpower is 30
- It makes the check if 20 is greater than 30 which is false so code doesn't break
- It consumes `*` token and makes aa `MultiplyNode`
- Then it gets it right side with the `ParseExpression` and then return 5 and the right
