# cb
a silly little programming language

## what 
You know ML, the Meta Language? cb is a hobbyist's love letter
to that elegant functional programming language that inspired
many a student.

## the name though?
cb could mean any number of things, such as
- code beauty
- completely binary
- cool beans

Take your pick or insert your own! (pull request)

## why
Just to write a program using pure C and no libraries except
for the standard library.

Maybe it'll be the scripting language for a computer game sometime, who knows!

## how
cb is written in pure, fresh C. Compile by simply issuing `make`. A fragrant,
toasty binary will be placed under `bin/cb`.

## some examples

Evaluate expressions by using the `-e` switch:

```
$ cb -e "if 3+4 > 5 then 1 else 0"
1
```

Evaluate a file:

```
$ cb sample.cb
34
```

Executed without any arguments, `cb` will read from standard input.

```
$ echo "17 + 4" | cb
21
```

## what does it do

Evaluation of expressions

```
$ cb -e "17 + 4"
21
```

Conditionals

```
$ cb -e "if 1 > 2 then 0 else 1"
1
```

Value declarations

```
$ cb
  let x = 17
  let y = 4
  let z = x + y
21
```