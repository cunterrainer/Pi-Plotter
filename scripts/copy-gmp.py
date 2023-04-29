import os, shutil, pathlib, sys, subprocess

def Mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        print(f"Created directory '{dir}'")

def SetEnvironment(path):
    print(f"Changing working directory to '{path}'")
    os.chdir(path)

def Execute(cmd):
    popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True)
    for stdoutLine in iter(popen.stdout.readline, ""):
        yield stdoutLine 
    popen.stdout.close()
    popen.wait()

def main(dir) -> None:
    libFolder = "../Dependencies/gmp/libs/linux/" + dir
    Mkdir(libFolder)
    folder = ["../Dependencies/gmp/.libs", "../Dependencies/gmp/mpf/.libs", "../Dependencies/gmp/mpn/.libs", "../Dependencies/gmp/mpq/.libs", "../Dependencies/gmp/mpz/.libs", "../Dependencies/gmp/printf/.libs", "../Dependencies/gmp/rand/.libs", "../Dependencies/gmp/scanf/.libs"]
    for i in folder:
        files = [f for f in os.listdir(i) if os.path.isfile(os.path.join(i, f))]
        for file in files:
            if pathlib.Path(file).suffix.lower() == ".a" or pathlib.Path(file).suffix.lower() == ".la":
                print(f"Copying '{i}/{file}' to " + libFolder)
                shutil.copy2(i+ "/" +file, libFolder)
    if dir == "clang":
        currentDir = os.getcwd()
        SetEnvironment(libFolder)
        for i in Execute(["python", currentDir + "/rename-libs.py"]):
            print(f"[rename-libs.py] {i}", end='')
        


SetEnvironment("/".join(os.path.realpath(__file__).replace('\\', '/').split('/')[0:-1]))
dir = "gcc"
if len(sys.argv) != 1:
    dir = sys.argv[1].lower()
    if dir != "gcc" and dir != "clang":
        dir = "gcc"
main(dir)