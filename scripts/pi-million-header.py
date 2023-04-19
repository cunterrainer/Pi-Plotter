def main():
    with open('pi-million.txt', 'r') as file:
        data = file.read()[2:]
    content = "#pragma once\n#include <string_view>\ninline const std::string_view PiMillionStr = \""
    for i in range(len(data)):
        if i % 16380 == 0 and i != 0 and i != len(data)-1: # 16380 msvc max string size
            content += '"\n"'
        content += data[i]
    if content[-1] != '"':
        content += '"'
    content += ";"
    with open("../Pi-Plotter/src/pi-million.h", "w") as file:
        file.write(content)
main()