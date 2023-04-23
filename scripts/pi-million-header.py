from progress.bar import IncrementalBar
from time import time

def main():
    with open('pi-million.txt', 'r') as file:
        data = file.read()[2:]
    dataSize = len(data)
    bar = IncrementalBar("Iteration", max=dataSize, suffix="%(index)d/%(max)d %(percent).2f%%")
    
    content = "#pragma once\n#include <string_view>\ninline const std::string_view PiMillionStr = \""
    content += data[0]
    for i in range(1, dataSize):
        if i % 1000000 == 0:
            bar.next(1000000)
        if i % 16380 == 0: # 16380 msvc max string size
            content += '"\n"'
        content += data[i]
    bar.finish()
    content += "\";"
    with open("../Pi-Plotter/src/pi-million.h", "w") as file:
        file.write(content)
start = time()
main()
print("Execution time: {} sec(s)".format(time()-start))