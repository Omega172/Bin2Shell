#include <stdio.h>
#include <sstream>
#include <fstream>
#include <filesystem>

long long GetFileSize(std::ifstream& file)
{
	std::streampos fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;

	file.seekg(0, std::ios::beg);

	return fsize;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Usage: %s input_file output_file", argv[0]);
		return EXIT_FAILURE;
	}

	if (!std::filesystem::exists(argv[1]))
	{
		printf("Error: File \"%s\" does not exist.", argv[1]);
		return EXIT_FAILURE;
	}

	std::ifstream file(argv[1], std::ios::in | std::ios::binary);
	if (!file.is_open() || !file.good())
	{
		printf("Error: Failed to open file \"%s\"", argv[1]);
		return EXIT_FAILURE;
	}

	auto size = GetFileSize(file);
	char* buffer = new char[size];

	file.read(buffer, size);

	file.close();

	std::string sBuf = std::string(buffer, size);

	auto GetHex = [](std::string& s, bool bUpperCase)
	{
		std::vector<std::string> bytes;

		for (std::string::size_type i = 0; i < s.length(); i++)
		{
			int x = s[i] & 0xFF;

			std::stringstream ss;
			ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << ((bUpperCase) ? std::uppercase : std::nouppercase) << x << ", ";

			bytes.push_back(ss.str());
		}

		return bytes;	
	};

	std::vector<std::string> hexBytes = GetHex(sBuf, true);
	
	hexBytes[hexBytes.size()-1].erase(hexBytes[hexBytes.size()-1].find_last_of(", ") - 1);

	std::ofstream outFile(argv[2]);
	if (!outFile.is_open() || !outFile.good())
	{
		printf("Error: Failed to open file \"%s\"", argv[1]);
		return EXIT_FAILURE;
	}

	outFile << "const size_t rawDataSize = " << size << ";\n";
	outFile << "const unsigned char rawData[rawDataSize] = { ";

	for (int i = 0; i < hexBytes.size(); i++)
	{
		if ((i % 30) == 0)
		{
			outFile.seekp(-1, std::ios_base::cur);
			outFile << "\n\t";
		}

		outFile << hexBytes[i];
	}

	outFile << "\n};";

	outFile.close();

	return 0;
}