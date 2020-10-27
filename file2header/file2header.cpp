#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "color_utils.h"

#define BYTES_PER_LINE  15

bool read_file_binary(const std::string& path, std::vector<uint8_t>& out_buffer)
{
    std::ifstream file_stream(path, std::fstream::binary);
    if (!file_stream)
        return false;

    file_stream.seekg(0, file_stream.end);
    int length = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);

    out_buffer.resize(length);
    file_stream.read((char*)out_buffer.data(), length);

    file_stream.close();
    return true;
}

bool write_file_text(const std::string& path, std::string& in_buffer)
{
    std::ofstream file_stream(path, std::ofstream::out);
    if (!file_stream)
        return false;

    file_stream << in_buffer;
    file_stream.close();
    return true;
}

int main(int argc, char** argv)
{
    /* Ensure parameters are correctly entered */
    if (argc != 3)
    {
        std::cout << ERROR << "Incorrect usage!" << std::endl;
        std::cout << INFO << "Example: " << argv[0] << " image.exe image.h" << std::endl;
        return -1;
    }

    /* Read first parameter */
    char input_path[MAX_PATH];
    strcpy_s(input_path, argv[1]);

    /* Read second parameter */
    char output_path[MAX_PATH];
    strcpy_s(output_path, argv[2]);

    /* Print paths */
    std::cout << INFO << "Input file: " << input_path << std::endl;;
    std::cout << INFO << "Output file: " << output_path << std::endl;;

    /* Read input file */
    std::vector<uint8_t> buffer;
    if (!read_file_binary(input_path, buffer))
    {
        std::cout << ERROR << "Couldn't read input file" << std::endl;
        return -1;
    }

    std::cout << INFO << "Input file size: 0x" << std::hex << std::uppercase << buffer.size() << std::endl;
    std::cout << INFO << "Generating file, please wait..." << std::endl;
    
    /* Generate header file */
    std::string header;
    header.append("#include <cstdint>\n\n");
    header.append("const uint8_t image[] = \n");
    header.append("{\n");

    int counter = 0;
    for (uint8_t current_char : buffer)
    {
        if (counter == 0)
        {
            header.append("    ");
            counter++;
        }
        else if (counter == BYTES_PER_LINE)
        {
            header.append("\n");
            counter = 0;
        }
        else 
        {
            char hex_string[10];
            sprintf_s(hex_string, "0x%02X", current_char);
            header.append(hex_string);
            header.append(", ");
            counter++;
        }
    }

    header.append("}\n");

    /* Write file */
    if (!write_file_text(output_path, header))
    {
        std::cout << ERROR << "Couldn't write output file" << std::endl;
        return -1;
    }

    std::cout << SUCCESS << "Success!" << std::endl;
    return 0;
}