import requests, subprocess, sys, os, platform
try:
    from clint.textui import progress
except ImportError:
    print("Failed to import 'clint' trying to install it")
    subprocess.check_call([sys.executable, "-m", "pip", "install", 'clint'])
finally:
    from clint.textui import progress

def ExecutableName():
    plt = platform.system().lower()
    if plt == "windows":
        return "pi-lib.exe"
    elif plt == "linux":
        return "./pi-lib"
    return ""

def Execute(cmd):
    popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True)
    for stdoutLine in iter(popen.stdout.readline, ""):
        yield stdoutLine 
    popen.stdout.close()
    popen.wait()

def main():
    url = "https://stuff.mit.edu/afs/sipb/contrib/pi/pi-billion.txt"
    fileName = "pi-billion.txt"
    r = requests.get(url, stream=True)
    f = open(fileName, "wb")
    first = True

    print("Downloading pi-billion.txt from: '{}'".format(url))
    totalLength = int(r.headers.get('content-length'))
    for chunk in progress.bar(r.iter_content(chunk_size=1024), expected_size=(totalLength/1024) + 1): 
        if chunk:
            if first:
                f.write(chunk[2:])
                first = False
            else:
                f.write(chunk)
    f.flush()
    f.close()
    print("Finished downloading\nCompiling and creating data binaries...")
    for i in Execute(ExecutableName()):
        print(f"[{ExecutableName()}] {i}", end='')

if __name__ == "__main__":
    try:
        path = "/".join(os.path.realpath(__file__).replace('\\', '/').split('/')[0:-1])
        print(f"Changing working directory to '{path}'")
        os.chdir(path)
        main()
    except KeyboardInterrupt as ex:
        print("\nStopped due to keyboard interrupt")
    except BaseException as ex:
        print("\nStopped due to an unhandled exception: {}".format(ex))
