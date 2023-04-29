#include <cstdio>
#include <string>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "../../Pi-Plotter/src/Log.h"
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
        Err << "Failed to open file [" << file << ']' << Endl;
        return std::string();
    }

    std::stringstream ss;
    ss << input.rdbuf();
    const std::string data = ss.str();
    ss.clear();

    std::string content = "const char* const Pi" + identifier + "Str = \"";
    Log << "Converting file [" << file << ']' << Endl;
    content += CreateContent(data);
    return content;
}

void ExecuteCmd(const char* cmd)
{
    Log << '[' << cmd << ']' << Endl;
    std::system(cmd);
}

// 0 = clang 1 = gcc
void Compile(uint8_t compiler)
{
    Log << "Starting to compile: ";
    if(compiler == 0)
        ExecuteCmd("clang -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    else
        ExecuteCmd("gcc -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    Log << "Finished compiling" << Endl;
}

void LinkLibrary(uint8_t compiler)
{
    Log << "Starting to create library: ";
    if(compiler == 0)
        ExecuteCmd("ar r pi-data.lib " BINFILE_NAME);
    else
        ExecuteCmd("ar r libpi-data.a " BINFILE_NAME);
    Log << "Created static library" << Endl;
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
        Log << "Successfully removed file [" << file << ']' << Endl;
    else
        Err << "Failed to remove file [" << file << "] Error: " << std::strerror(errno) << Endl;
}

void MvFile(const char* file, const char* newLoc)
{
    if(std::rename(file, newLoc) == 0)
        Log << "Successfully moved file [" << file << "] to [" << newLoc << ']' << Endl;
    else
        Err << "Failed to move file [" << file << "] to [" << newLoc << "] Error: " << std::strerror(errno) << Endl;
}

void RemoveDir(const std::filesystem::path& folder)
{
    if(std::filesystem::remove_all(folder))
        Log << "Successfully removed folder [" << folder << ']' << Endl;
    else
        Err << "Faile to removed folder [" << folder << ']' << Endl;
}

void CreateFolder(const std::filesystem::path& folder)
{
    if (std::filesystem::exists(folder))
        RemoveDir(folder);
    if(std::filesystem::create_directory(folder))
        Log << "Successfully created folder [" << folder << ']' << Endl;
    else
        Err << "Failed to create folder [" << folder << ']' << Endl;
}

void MoveLibraries()
{
    Log << "Moving libraries..." << Endl;
    MvFile(CLANG_LIB_NAME, OUT_DIR CLANG_LIB_NAME);
    MvFile(GCC_LIB_NAME, OUT_DIR GCC_LIB_NAME);
    MvFile(HEADER_NAME, OUT_DIR HEADER_NAME);
}

void SetEnvironment(char** argv)
{
    std::filesystem::path dir = std::filesystem::weakly_canonical(argv[0]).parent_path();
    Log << "Changing working directory to " << dir << Endl;
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
    Log << "Created header file [" << HEADER_NAME << ']' << Endl;

    std::ofstream output(OUTFILE_NAME);
    output << piMillionStr << "\n\n" << piBillionStr;
    output.close();
    Log << "Finished writing to [" << OUTFILE_NAME << ']' << Endl;

    CreateLibraries();
    CreateFolder(OUT_DIR);
    MoveLibraries();
    RemoveFile(OUTFILE_NAME);
    RemoveFile(BINFILE_NAME);
    Profiler::End();
    Profiler::Log(Profiler::Conversion::Seconds);
}