#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../Pi-Plotter/src/Profiler.h"
#include "../Pi-Plotter/src/ProgressBar.h"

int main()
{
    Profiler::Start();
    std::ifstream input("pi-billion.txt");
    std::stringstream ss;
    ss << input.rdbuf();
    const std::string data = ss.str();
    ss.clear();

    const size_t dataSize = data.size()-1;
    std::string content = "#pragma once\n#include <string_view>\ninline const std::string_view PiBillionStr = \"";
    content.reserve(dataSize);
    content += data[0];

    ProgressBarInit();
    for (size_t i = 1; i < dataSize; ++i)
    {
        if(i % 1000000 == 0)
            ProgressBar((float)i, (float)dataSize-1);
        if (i % 16380 == 0)
            content += "\"\n\"";
        content += data[i];
    }
    content += "\";";

    std::ofstream output("../Pi-Plotter/src/pi-billion.h");
    output << content;
    Profiler::End();
    std::cout << "\nExecution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)\n";
}