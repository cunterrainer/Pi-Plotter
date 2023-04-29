import os
files = [f for f in os.listdir('.') if os.path.isfile(f)]
for f in files:
    if f[-1] == "a" and f[-2] == ".":
        os.rename(f, f[3:-2] + ".lib")
        print("Renamed: '" + f + "' to '" + f[3:-2] + ".lib'")