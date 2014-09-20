import time
import os
import random

kwds = [
        " void ",
        " int ",
        " double ",
        " bool ",
        " string ",
        " class ",
        " interface ",
        " null ",
        " this ",
        " extends ",
        " implements ",
        " for ",
        " while ",
        " if ",
        " else ",
        " return ",
        " break ",
        " New ",
        " NewArray ",
        " true ",
        " false ",
        " Print ",
        " ReadInteger ",
        " ReadLine ",
    ]

ops = [
        "+",
        "-",
        "*",
        "/",
        "%",
        "<",
        "<=",
        ">",
        ">=",
        "=",
        "==",
        "!=",
        "&&",
        "||",
        "!",
        ";",
        ",",
        ".",
        "[",
        "]",
        "(",
        ")",
        "{",
        "}",
        ]

whitespace = [
        " ",
        "\n",
        "\t",
        ]

digits = "0123456789"

ids = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

def get_whitespaces():
    return whitespace[random.randrange(len(whitespace))]

def get_kwds():
    return kwds[random.randrange(len(kwds))]

def get_ids():
    length = random.randrange(10, 40)
    return "".join([str(ids[random.randrange(len(ids))]) for i in xrange(length)])

def get_ops():
    return ops[random.randrange(len(ops))]

def get_int():
    return str(random.randrange(0, 400)
)
def get_float():
    to_return = ""

    if random.randrange(0, 2) == 0:
        to_return += str(get_int())

    to_return += "."

    if random.randrange(0, 2) == 0:
        to_return += str(get_int())

    if random.randrange(0, 2) == 0:
        to_return += random.choice(("e", "E", ""))
        to_return += random.choice(("+", "-", ""))
        to_return += str(get_int())

    return to_return

def get_string():
    length = random.randrange(0, 100)
    to_return = "\""

    for i in xrange(length):
        c = str(unichr(random.randrange(32, 127)))
	to_return += c

    to_return += "\""
    return to_return


funs = [
        get_whitespaces,
        get_kwds,
        get_ids,
        get_ops,
        get_int,
        get_float,
        get_string
        ]

def main():
    os.system("make -s")

    os.system("mkdir pytest")
    
    for j in xrange(1000):
	    length = random.randrange(10, 40)
	    to_test = range(length)
	    for i in xrange(length):
		num = random.randrange(1, 10)
		fun = random.choice(funs)
		to_put = ""
		for each in xrange(num):
		    to_put += fun()
		    if random.randrange(0, 50) > 5:
			    to_put += get_whitespaces()
		to_test[i] = to_put

	    to_pipe = " ".join(to_test)

	    
	    with open("pytest/temp.txt", "w") as f:
		f.write(to_pipe)
	    
	    os.system("./dcc < pytest/temp.txt > pytest/mine.out 2>&1")
	    os.system("./solution/dcc < pytest/temp.txt > pytest/theirs.out 2>&1")

	    os.system("diff -w pytest/mine.out pytest/theirs.out")

    #os.system("rm -rf pytest")

    os.system("make -s clean")






if __name__ == "__main__":
    main()
