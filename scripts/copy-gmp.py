import os, shutil, pathlib, sys

def Mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        print(f"Created directory '{dir}'")

def main(dir) -> None:
    libFolder = "../Dependencies/gmp/mylibs/linux/"
    Mkdir(libFolder)
    folder = ["../Dependencies/gmp/.libs", "../Dependencies/gmp/mpf/.libs", "../Dependencies/gmp/mpn/.libs", "../Dependencies/gmp/mpq/.libs", "../Dependencies/gmp/mpz/.libs", "../Dependencies/gmp/printf/.libs", "../Dependencies/gmp/rand/.libs", "../Dependencies/gmp/scanf/.libs"]
    for i in folder:
        files = [f for f in os.listdir(i) if os.path.isfile(os.path.join(i, f))]
        for file in files:
            if pathlib.Path(file).suffix.lower() == ".a" or pathlib.Path(file).suffix.lower() == ".la":
                print(f"Copying '{i}/{file}' to " + "../Dependencies/gmp/mylibs/linux/" + dir)
                shutil.copy2(i+ "/" +file, libFolder + dir)


path = "/".join(os.path.realpath(__file__).replace('\\', '/').split('/')[0:-1])
print(f"Changing working directory to '{path}'")
os.chdir(path)
dir = "gcc"
if len(sys.argv) != 1:
    dir = sys.argv[1].lower()
    if dir != "gcc" and dir != "clang":
        dir = "gcc"
main(dir)