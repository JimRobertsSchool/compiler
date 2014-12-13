from os import listdir
from subprocess import call
def main():
    todo = map(lambda a: a[:-4], filter(lambda a: a[-3:] == "out", listdir("./tocompare")))
    for each in todo:
        call(["./run", "./tocompare/"+each+".decaf", "|", "diff", "-", "./tocompare"+each+".out"])


if __name__ == "__main__":
    main()
