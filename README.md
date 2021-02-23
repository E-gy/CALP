![Build](https://github.com/E-gy/CALP/workflows/CMake/badge.svg)
[![Coverage Status](https://coveralls.io/repos/github/E-gy/CALP/badge.svg?branch=master&t=4MeVqY)](https://coveralls.io/github/E-gy/CALP?branch=master)
# CALP
_C ~~Abusive~~ Abstract Language Parser_

A wanna-be-LL/LR parser in C, featuring:
- runtime language construction & parser compilation
- macros for compile-time grammar-like declaration.
- with both left and right recursive rules support, thanks to
- abusive error resolution to try to parse at whatever cost

The latter basically means _this isn't a fast parser_, but _an incredibly permissive one_ instead.
Lemme show what that means in practice
```
Z: expr $
expr: ifelse
    | cmdinc

word: /\w+/

cmdinc: word /\s+/ cmdinc
      | word

ifelse: "if" word "then" cmdinc "else" cmdinc "fi"
```
Then `if potato then say hi` parses, but not to what you think - `Z(expr(cmdinc(if potato then say hi)))`.

What happened? Well, word can accept both `if` and `then`, and after trying the `Z->expr->ifelse` rule (which failed), the parser backtracked and went onto it with `Z->expr->cmdinc`.

In fact, it just so happens that in the above ruleset, as it is, the `ifelse` will _never_ be produced - eager `cmdinc` parsing will consume the `then` and/or else, because they are valid for `word`.

To mitigate all the issues akin above, CALP provides 2 special features:
- priority specification at symbol, rule, and group levels   (higher priority branches are likelier to be tried first)
- programmatic symbol-specific lexing/tokenization   (each terminal symbol specifies its own lexer given an input decides how much input to consume, _and which portion of the consumed input corresponds to the symbol_)

CALP still certainly has some 🐛 around!
