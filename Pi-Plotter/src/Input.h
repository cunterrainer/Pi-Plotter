#pragma once
#include <limits>
#include <vector>
#include <string>
#include <istream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "Pi.h"
#include "Log.h"
#include "FactorialTable.h"

class InputHandler
{
private:
    template <typename T> using RT = std::remove_reference_t<std::remove_cv_t<T>>;
private:
	int m_Argc;
	bool m_Lowered = false;
	bool m_Uppered = false;
	std::vector<std::string> m_Args;
private:
	template <typename T>
	inline std::optional<RT<T>> ConvertNumber(const char* data) const noexcept
	{
		RT<T> n;
		char* endptr = nullptr;
		if constexpr (std::is_floating_point_v<RT<T>>)
		{
			if constexpr (std::is_same_v<float, RT<T>>)
				n = std::strtof(data, &endptr);
			if constexpr (std::is_same_v<double, RT<T>>)
				n = std::strtod(data, &endptr);
		}
		if constexpr (std::numeric_limits<RT<T>>::is_integer && std::numeric_limits<RT<T>>::is_signed)
		{
			n = static_cast<RT<T>>(std::strtoll(data, &endptr, 10));
		}
		if constexpr (std::numeric_limits<RT<T>>::is_integer && !std::numeric_limits<RT<T>>::is_signed)
		{
			n = static_cast<RT<T>>(std::strtoull(data, &endptr, 10));
		}

		if (*endptr != 0)
			return std::nullopt;
		return { n };
	}
public:
	explicit InputHandler(int argc, const char** argv) : m_Argc(argc)
	{
		m_Args.reserve(argc);
		for (int i = 0; i < m_Argc; ++i)
			m_Args.emplace_back(argv[i]);
	}


	bool Contains(const char* element) const noexcept
	{
		for (int i = 0; i < m_Argc; ++i)
			if (strcmp(m_Args[i].c_str(), element) == 0)
				return true;

		return false;
	}

	bool Contains_l(const std::string& element) const noexcept
	{
		std::string element_l = Lower(element);

		if (m_Lowered)
		{
			for (int i = 0; i < m_Argc; ++i)
				if (m_Args[i] == element_l)
					return true;

			return false;
		}

		for (int i = 0; i < m_Argc; ++i)
			if (Lower(m_Args[i]) == element_l)
				return true;

		return false;
	}

	bool Contains_u(const std::string& element) const noexcept
	{
		std::string element_u = Upper(element);

		if (m_Uppered)
		{
			for (int i = 0; i < m_Argc; ++i)
				if (m_Args[i] == element_u)
					return true;

			return false;
		}

		for (int i = 0; i < m_Argc; ++i)
			if (Upper(m_Args[i]) == element_u)
				return true;

		return false;
	}

	int Location(const char* data) const noexcept
	{
		for (int i = 0; i < m_Argc; ++i)
			if (strcmp(m_Args[i].c_str(), data) == 0)
				return i;
		return -1;
	}

	int Location_l(const char* data) const noexcept
	{
		std::string data_l = Lower(data);
		if (m_Lowered)
		{
			for (int i = 0; i < m_Argc; ++i)
				if (data_l == m_Args[i])
					return i;
			return -1;
		}

		for (int i = 0; i < m_Argc; ++i)
			if (data_l == Lower(m_Args[i]))
				return i;
		return -1;
	}

	int Location_u(const char* data) const noexcept
	{
		std::string data_u = Upper(data);
		if (m_Uppered)
		{
			for (int i = 0; i < m_Argc; ++i)
				if (data_u == m_Args[i])
					return i;
			return -1;
		}

		for (int i = 0; i < m_Argc; ++i)
			if (data_u == Upper(m_Args[i]))
				return i;
		return -1;
	}

	std::string Lower(const std::string& data) const noexcept
	{
		std::string result;

		for (size_t i = 0; i < data.size(); ++i)
			result += (char)std::tolower(data[i]);

		return result;
	}

	std::string Upper(const std::string& data) const noexcept
	{
		std::string result;

		for (size_t i = 0; i < data.size(); ++i)
			result += (char)std::toupper(data[i]);

		return result;
	}

	void UpperAll() noexcept
	{
		if (m_Uppered)
			return;

		for (size_t i = 0; i < m_Args.size(); ++i)
			std::transform(m_Args[i].begin(), m_Args[i].end(), m_Args[i].begin(), [](unsigned char c) { return std::toupper(c); });

		m_Uppered = true;
		m_Lowered = false;
	}

	void LowerAll() noexcept
	{
		if (m_Lowered)
			return;

		for (size_t i = 0; i < m_Args.size(); ++i)
			std::transform(m_Args[i].begin(), m_Args[i].end(), m_Args[i].begin(), [](unsigned char c) { return std::tolower(c); });

		m_Uppered = false;
		m_Lowered = true;
	}

	std::vector<std::string> Split(const std::string& str, char c, size_t minSize = 2) const
	{
		std::stringstream ss(str);
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(ss, segment, c))
			seglist.push_back(segment);

		while(seglist.size() < minSize)
			seglist.emplace_back("");
		return seglist;
	}


	inline const std::string& operator[](const int& index) const { return m_Args[index]; }

	template <typename T> inline bool IsNumber(const char* data) const noexcept { return ConvertNumber<T>(data).has_value(); }
	template <typename T> inline bool IsNumber(const std::string& data) const noexcept { return IsNumber<T>(data.c_str()); }
	template <typename T> inline RT<T> Number(const std::string& data) const noexcept { return ConvertNumber<T>(data.c_str()).value(); }
	template <typename T> inline RT<T> Number(const char* data) const noexcept { return ConvertNumber<T>(data).value(); }
	inline int Size() const { return m_Argc; }
	inline const std::vector<std::string>& elements() const noexcept { return m_Args; }
	inline const char* at(const int& index) const { return m_Args[index].c_str(); }

	void Print() const noexcept
	{
		std::cout << '[';
		for (int i = 0; i < m_Argc; ++i)
		{
			std::cout << m_Args[i] << (i == m_Argc-1 ? "" : ", ");
		}
		std::cout << "]\n";
	}
};


struct CmdSettings
{
	size_t factorial = 100000;
	bool piBillion = true;
	bool piMillion = false;
	bool fromFile = false;
	bool error = false;

	uint32_t archimedesArgs[3] = {    100, 1000000000, 100 };
	uint32_t chudnovskyArgs[3] = { 150000,     100000, 100 };
	uint32_t newtonArgs[3] =     { 100000,         20,   1 };
};


inline void PrintHelp(const std::string& program, const CmdSettings& s)
{
	const auto& o = ThreadSafe::Stdout;
	o.Printfln("Usage: %s [options]", program.c_str());
	o.Println("Options:");
	o.Println("  --help                     | -h  Display this information");
	o.Println("  --million                  | -m  Use one million digits of pi to compare against");
	o.Println("  --billion                  | -b  Use one billion digits of pi to compare against");
	o.Println("  --readfile                 | -r  Read pi digits from file");
	o.Println("  --factorial=[value=100000] | -f  Set the limit for the internal factorial table");
	o.Println("  --archimedes=[p,i,m]       | -a  Set properties for the archimedes algorithm");
	o.Println("  --chudnovsky=[p,i,m]       | -c  Set properties for the chudnovsky algorithm");
	o.Println("  --newton=[p,i,m]           | -n  Set properties for the newton algorithm");
	o.Println("Properties: p = precision, i = iterations, m = render modulus, All of values must be greater than 0");
	o.Println("Default properties:");
	o.Printfln("--archimedes=%u,%u,%u", s.archimedesArgs[0], s.archimedesArgs[1], s.archimedesArgs[2]);
	o.Printfln("--chudnovsky=%u,%u,%u", s.chudnovskyArgs[0], s.chudnovskyArgs[1], s.chudnovskyArgs[2]);
	o.Printfln("--newton=%u,%u,%u", s.newtonArgs[0], s.newtonArgs[1], s.newtonArgs[2]);
}


inline void SetAlgoArgs(uint32_t* s, const InputHandler& ih, const std::vector<std::string>& split)
{
	const std::vector<std::string> algoArgs = ih.Split(split[1], ',', 3);
	const auto setNum = [&](size_t idx) {
		if (ih.IsNumber<uint32_t>(algoArgs[idx]))
		{
			uint32_t t = ih.Number<uint32_t>(algoArgs[idx]);
			if (t != 0)
				s[idx] = t;
		}
	};
	setNum(0);
	setNum(1);
	setNum(2);
}


inline CmdSettings ParseCmd(int argc, const char** argv)
{
	const std::string program = argv[0];
	CmdSettings s;
	InputHandler ih(argc, argv);
	ih.LowerAll();

	for (int i = 1; i < ih.Size(); ++i)
	{
		const std::string& c = ih[i];
		const std::vector<std::string> split = ih.Split(c, '=', 2);
		if ((split[0] == "--factorial" || split[0] == "-f") && ih.IsNumber<size_t>(split[1]))
		{
			s.factorial = ih.Number<size_t>(split[1]);
		}
		else if (split[0] == "--archimedes" || split[0] == "-a")
			SetAlgoArgs(s.archimedesArgs, ih, split);
		else if (split[0] == "--chudnovsky" || split[0] == "-c")
			SetAlgoArgs(s.chudnovskyArgs, ih, split);
		else if (split[0] == "--newton" || split[0] == "-n")
			SetAlgoArgs(s.newtonArgs, ih, split);
		else if (c == "--billion" || c == "-b")
		{
			s.piBillion = true;
			s.piMillion = false;
		}
		else if (c == "--million" || c == "-m")
		{
			s.piBillion = false;
			s.piMillion = true;
		}
		else if (c == "--readfile" || c == "-r")
			s.fromFile = true;
		else if (c == "--help" || c == "-h")
		{
			PrintHelp(program, CmdSettings{});
			s.error = true;
			return s;
		}
		else
		{
			std::cout << "Unknown argument: " << c << "\nTry --help for detailed information" << std::endl;
			s.error = true;
			return s;
		}
	}
	return s;
}


inline bool ApplySettings(const CmdSettings& cs)
{
	if (cs.error) return false;

	bool res = true;
	auto readFile = [&res](const char* file) -> std::string 
	{
		std::ifstream t(file);
		if (!t.is_open())
		{
			Err.Printfln("Failed to open file [%s]", file);
			res = false;
			return "";
		}
		Log.Printfln("Starting to read file [%s]", file);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	};

	const char* pistr = [&]() { return cs.piBillion ? "Billion" : "Million"; }();
	const char* filestr = [&]() { return cs.fromFile ? "True" : "False"; }();
	if (cs.piBillion)
	{
		if (cs.fromFile)
		{
			static std::string p = readFile("pi-billion");
			if (!res) return res;
			p = p.substr(2);
			Pi::PiStr = p;
			filestr = "true";
		}
		else
			Pi::PiStr = PiBillionStr;
	}
	else
	{
		if (cs.fromFile)
		{
			static std::string p = readFile("pi-million");
			if (!res) return res;
			p = p.substr(2);
			Pi::PiStr = p;
			filestr = "true";
		}
		else
			Pi::PiStr = PiMillionStr;
		pistr = "Million";
	}

	Pi::Algorithm::FloatA::Precision = (size_t)(3.5 * (double)cs.archimedesArgs[0]);
	Pi::Algorithm::FloatA::Format = "%." + std::to_string(cs.archimedesArgs[0]) + "RDf";
	Pi::Algorithm::FloatA::StrBuf = std::string(cs.archimedesArgs[0] + 2, 0);

	Pi::Algorithm::FloatC::Precision = (size_t)(3.5 * (double)cs.chudnovskyArgs[0]);
	Pi::Algorithm::FloatC::Format = "%." + std::to_string(cs.chudnovskyArgs[0]) + "RDf";
	Pi::Algorithm::FloatC::StrBuf = std::string(cs.chudnovskyArgs[0] + 2, 0);

	Pi::Algorithm::FloatN::Precision = (size_t)(3.5 * (double)cs.newtonArgs[0]);
	Pi::Algorithm::FloatN::Format = "%." + std::to_string(cs.newtonArgs[0]) + "RDf";
	Pi::Algorithm::FloatN::StrBuf = std::string(cs.newtonArgs[0] + 2, 0);
	Pi::Algorithm::Reset();

	ThreadSafe::Stdout.Println("-------------------------------------------------------------------------------------------");
	ThreadSafe::Stdout.Printfln("Global settings [Factorial limit=%zu] [Pi digits=%s] [Pi from file=%s]", cs.factorial, pistr, filestr);
	ThreadSafe::Stdout.Printfln("Algorithm settings [Archimedes=p:%u, i:%u, m:%u] [Chudnovsky=p:%u, i:%u, m:%u] [Newton=p:%u, i:%u, m:%u]", cs.archimedesArgs[0], cs.archimedesArgs[1], cs.archimedesArgs[2], cs.chudnovskyArgs[0], cs.chudnovskyArgs[1], cs.chudnovskyArgs[2], cs.newtonArgs[0], cs.newtonArgs[1], cs.newtonArgs[2]);
	FactTable.Create(cs.factorial);
	ThreadSafe::Stdout.Println("-------------------------------------------------------------------------------------------\n");
	return res;
}