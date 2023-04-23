from progress.bar import IncrementalBar
from time import time

def main(iters):
    bar = IncrementalBar("Iteration", max=iters, suffix="%(index)d/%(max)d %(percent).2f%%")
    content = "#pragma once\n#include <array>\n#include <string_view>\ninline constexpr std::array<std::string_view, {}> FactorialTableData = \n{{\n".format(iters)

    prev = 1
    for i in range(1, iters+1):
        bar.next()
        prevStr = str(prev)
        if len(prevStr) >= 16380:
            content += '\t'
            blocks = int(len(prevStr) / 16380.0)
            for k in range(blocks):
                content += '"{}"'.format(prevStr[k*16830:k*16830+16380]) # 16379
            if blocks != len(prevStr) / 16380.0:
                content += '"{}"'.format(prevStr[blocks*16380:])
            content += ',\n'
        else:
            content += '\t"{}",\n'.format(prevStr)
        prev *= i

    content += "};"
    bar.finish()
    print(str(iters) + " decimal: " + str(len(str(prev))))
    with open("../Pi-Plotter/src/FactorialTableData.h", "w") as file:
        file.write(content)

iters = 10000
start = time()
main(iters)
print("Execution time to calculate {} factorials: {} sec(s)".format(iters, time()-start))