#include <cstdio>
#include <string>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "../../Pi-Plotter/src/Profiler.h"
#include "../../Pi-Plotter/src/ProgressBar.h"

#define HEADER_NAME  "pi-string.h"
#define FILE_NAME    "pi-string-bin"
#define OUTFILE_NAME FILE_NAME ".c"
#define BINFILE_NAME FILE_NAME ".o"

#define GCC_LIB_NAME   "libpi-data.a"
#define CLANG_LIB_NAME "pi-data.lib"
#define OUT_DIR        "../Dependencies/pi-data/"

std::string CreateContent(const std::string& data)
{
    const size_t dataSize = data.size()-1;
    std::string content;
    content.reserve(dataSize);
    content += data[0];

    ProgressBarInit();
    for (size_t i = 1; i < dataSize; ++i)
    {
        if(i % 1000000 == 0 || i == dataSize-1)
            ProgressBar((float)i, (float)dataSize-1);
        if (i % 16380 == 0)
            content += "\"\n\"";
        content += data[i];
    }
    content += "\";";
    return content;
}

std::string ConvertFile(const char* file, const std::string& identifier)
{
    std::ifstream input(file);
    if(!input.is_open())
    {
        std::cout << "Failed to open file [" << file << "]\n";
        return std::string();
    }

    std::stringstream ss;
    ss << input.rdbuf();
    const std::string data = ss.str();
    ss.clear();

    std::string content = "const char* const Pi" + identifier + "Str = \"";
    std::cout << "Converting file [" << file << "]\n";
    content += CreateContent(data);
    return content;
}

void ExecuteCmd(const char* cmd)
{
    std::cout << cmd << std::endl;
    system(cmd);
}

// 0 = clang 1 = gcc
void Compile(uint8_t compiler)
{
    std::cout << "Starting to compile: ";
    if(compiler == 0)
        ExecuteCmd("clang -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    else
        ExecuteCmd("gcc -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    std::cout << "Finished compiling\n";
}

void LinkLibrary(uint8_t compiler)
{
    std::cout << "Starting to create library: ";
    if(compiler == 0)
        ExecuteCmd("ar r pi-data.lib " BINFILE_NAME);
    else
        ExecuteCmd("ar r libpi-data.a " BINFILE_NAME);
    std::cout << "Created static library\n";
}

void CreateLibraries()
{
    Compile(0);
    LinkLibrary(0);
    Compile(1);
    LinkLibrary(1);
}

void RemoveFile(const char* file)
{
    if(std::remove(file) == 0)
        std::cout << "Successfully removed file [" << file << "]\n";
    else
        std::cout << "Failed to remove file [" << file << "] Error: " << std::strerror(errno) << std::endl;
}

void MoveFile(const char* file, const char* newLoc)
{
    if(std::rename(file, newLoc) == 0)
        std::cout << "Successfully moved file [" << file << "] to [" << newLoc << "]\n";
    else
        std::cout << "Failed to move file [" << file << "] to [" << newLoc << "] Error: " << std::strerror(errno) << std::endl;
}

void RemoveDir(const std::filesystem::path& folder)
{
    if(std::filesystem::remove_all(folder))
        std::cout << "Successfully removed folder [" << folder << "]\n";
    else
        std::cout << "Faile to removed folder [" << folder << "]\n";
}

void CreateFolder(const std::filesystem::path& folder)
{
    if (std::filesystem::exists(folder))
        RemoveDir(folder);
    if(std::filesystem::create_directory(folder))
        std::cout << "Successfully created folder [" << folder << "]\n";
    else
        std::cout << "Failed to create folder [" << folder << "]\n";
}

void MoveLibraries()
{
    std::cout << "Moving libraries...\n";
    MoveFile(CLANG_LIB_NAME, OUT_DIR CLANG_LIB_NAME);
    MoveFile(GCC_LIB_NAME, OUT_DIR GCC_LIB_NAME);
    MoveFile(HEADER_NAME, OUT_DIR HEADER_NAME);
}

void SetEnvironment(char** argv)
{
    std::filesystem::path dir = std::filesystem::weakly_canonical(argv[0]).parent_path();
    std::cout << "Changing working directory to " << dir << std::endl;
    std::filesystem::current_path(dir);
}

int main(int, char** argv)
{
    Profiler::Start();
    SetEnvironment(argv);
    std::string piMillionStr = ConvertFile("pi-million.txt", "Million");
    std::string piBillionStr = ConvertFile("pi-billion.txt", "Billion");
    if(piBillionStr.empty() && piMillionStr.empty()) return 1;
    std::string headerContent = "#ifndef PI_STRING_H\n#define PI_STRING_H\nextern \"C\" const char* const PiMillionStr;\nextern \"C\" const char* const PiBillionStr;\n#endif";

    std::ofstream header(HEADER_NAME);
    header << headerContent;
    header.close();
    std::cout << "Created header file [" << HEADER_NAME << "]\n";

    std::ofstream output(OUTFILE_NAME);
    output << piMillionStr << "\n\n" << piBillionStr;
    output.close();
    std::cout << "Finished writing to [" OUTFILE_NAME "]\n";

    CreateLibraries();
    CreateFolder(OUT_DIR);
    MoveLibraries();
    RemoveFile(OUTFILE_NAME);
    RemoveFile(BINFILE_NAME);
    Profiler::End();
    std::cout << "\nExecution time: " << Profiler::Average(Profiler::Conversion::Seconds) << " sec(s)\n";
}