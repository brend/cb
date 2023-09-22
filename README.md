# NL
a silly little programming language

## what 
You know ML, the Meta Language? NL is a hobbyist's love letter
to that elegant functional programming language that inspired
many a student.

## the name though?
Consequently, NL could mean any number of things, such as
- Nutty Language
- Neta Language (+1 compared to Meta Language 🫤)
- Not Likely (to succeed?)

Take your pick or insert your own! (pull request)

## why
Just to write a program using pure C and no libraries except
for the standard library.

Maybe it'll be the scripting language for a computer game sometime, who knows!

## how
NL is written in pure, fresh C. Compile by simply issuing `make`. A fragrant,
toasty binary will be placed under `bin/nl`.

## some examples

Evaluate expressions by using the `-e` switch:

```
$ nl -e "if 3+4 > 5 then 1 else 0
```

Evaluate a file:

```
$ nl sample.nl
```

Executed without any arguments, `nl` will read from standard input.

