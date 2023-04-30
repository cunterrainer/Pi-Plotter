#include <ios>
#include <tuple>
#include <cstdio>
#include <vector>
#include <string>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <string_view>

#include "../../Pi-Plotter/src/Log.h"
#include "../../Pi-Plotter/src/Profiler.h"
#include "../../Pi-Plotter/src/ProgressBar.h"

#include "libdownload.h"

#define LENGTH(ntstring) ((sizeof(ntstring) / sizeof(ntstring[0]))-1) // null terminated const char* only
#define URL          "https://stuff.mit.edu/afs/sipb/contrib/pi/pi-billion.txt"
#define BILLION_FILE "pi-billion"
#define MILLION_FILE "pi-million"

#define HEADER_NAME  "pi-string.h"
#define FILE_NAME    "pi-string-bin"
#define OUTFILE_NAME FILE_NAME ".c"
#define BINFILE_NAME FILE_NAME ".o"

#define GCC_LIB_NAME   "libpi-data.a"
#define CLANG_LIB_NAME "pi-data.lib"
#define OUT_DIR        "../Dependencies/pi-data/"

enum class Compiler { Clang, GCC };

std::string CreateContent(const std::string& data)
{
    const size_t dataSize = data.size()-2;
    std::string content;
    content.reserve(dataSize);

    ProgressBarInit();
    for (size_t i = 2; i <= dataSize; ++i)
    {
        if(i % 1000000 == 0 || i == dataSize)
            ProgressBar((float)i, (float)dataSize);
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

void Compile(Compiler compiler)
{
    Log << "Starting to compile: ";
    if(compiler == Compiler::Clang)
        ExecuteCmd("clang -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    else
        ExecuteCmd("gcc -O2 -c " OUTFILE_NAME " -o " BINFILE_NAME);
    Log << "Finished compiling" << Endl;
}

void LinkLibrary(Compiler compiler)
{
    Log << "Starting to create library: ";
    if(compiler == Compiler::Clang)
        ExecuteCmd("ar r pi-data.lib " BINFILE_NAME);
    else
        ExecuteCmd("ar r libpi-data.a " BINFILE_NAME);
    Log << "Created static library" << Endl;
}

void CreateLibraries()
{
    Compile(Compiler::Clang);
    LinkLibrary(Compiler::Clang);
    Compile(Compiler::GCC);
    LinkLibrary(Compiler::GCC);
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

void SetEnvironment(const char** argv)
{
    std::filesystem::path dir = std::filesystem::weakly_canonical(argv[0]).parent_path();
    Log << "Changing working directory to " << dir << Endl;
    std::filesystem::current_path(dir);
}

template <typename... Args>
void Print(Args&&... args)
{
    ((std::cout << std::forward<Args>(args)), ...) << '\n';
}

void PrintHelp(std::string_view program)
{
    Print("Usage: ", program, " [options]");
    Print("Options:");
    Print("  --help     | -h  Display this information");
    Print("  --build    | -b  Build '" MILLION_FILE "' and '" BILLION_FILE "' into static C libraries");
    Print("  --compile  | -c  Compile '" MILLION_FILE "' and '" BILLION_FILE "' into C obj files");
    Print("  --download | -d  Download '" BILLION_FILE "'");
    Print("\n'" MILLION_FILE "' and '" BILLION_FILE "' will be saved in the directory of the program");
    Print("[" CLANG_LIB_NAME "|" GCC_LIB_NAME "] will be saved in " OUT_DIR "[" CLANG_LIB_NAME "|" GCC_LIB_NAME "] respectively");
    Print("No [options] provided is equal to [--build --download]");
    Print("Hint: --compile and --build can not be selected at the same time");
}

std::tuple<bool, bool, bool> ParseCmd(int argc, const char** argv)
{
    std::vector<std::string> args(argv + 1, argv + argc);
    std::for_each(args.begin(), args.end(), [](std::string& s){std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){return std::tolower(c);});});
    const auto returnError = [&argv](){PrintHelp(argv[0]); return std::make_tuple(false, false, false);};
    bool build = false, compile = false, download = false;

    if (argc == 1)
        return std::make_tuple(true, false, true);

    for(const std::string& arg : args)
    {
        if(arg == "--help" || arg == "-h")
            return returnError();
        else if (arg == "--build" || arg == "-b")
        {
            if(compile)
            {
                Print("Error: --compile and --build can not be selected at the same time");
                return returnError();
            }
            build = true;
        }
        else if (arg == "--download" || arg == "-d")
            download = true;
        else if (arg == "--compile" || arg == "-c")
        {
            if(build)
            {
                Print("Error: --compile and --build can not be selected at the same time");
                return returnError();
            }
            compile = true;
        }
        else
        {
            Print("Unknown argument '", arg , '\'');
            return returnError();
        }
    }
    return std::make_tuple(build, compile, download);
}

int main(int argc, const char** argv)
{
    Profiler::Start();
    const auto[build, compile, download] = ParseCmd(argc, argv);
    if (!build && !compile && !download) return 0;
    Print(std::boolalpha, "Selected configuration: [compile=", compile, "] [build=", build, "] [download=", download, ']');
    SetEnvironment(argv);

    if (download)
    {
        GoString response = Download({URL, LENGTH(URL)}, {BILLION_FILE, LENGTH(BILLION_FILE)});
        if (response.n != 0) { Print(response.p); return 1; }
    }

    if (build || compile)
    {
        std::string piMillionStr = ConvertFile(MILLION_FILE, "Million");
        std::string piBillionStr = ConvertFile(BILLION_FILE, "Billion");
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
        
        if (compile)
        {
            Compile(Compiler::Clang);
            Compile(Compiler::GCC);
        }
        else if (build)
        {
            CreateLibraries();
            CreateFolder(OUT_DIR);
            MoveLibraries();
            RemoveFile(OUTFILE_NAME);
            RemoveFile(BINFILE_NAME);
        }
    }
    Profiler::End();
    Profiler::Log(Profiler::Conversion::Seconds);
}
