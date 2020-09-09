/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <sa/ArgParser.h>
#include <absync/Partition.h>
#include <filesystem>
#include <sa/time.h>
#include <fstream>
extern "C" {
#include <abcrypto/sha1.h>
}

namespace fs = std::filesystem;

static void ShowHelp(const sa::arg_parser::cli& _cli)
{
    std::cout << sa::arg_parser::get_help("fhash", _cli, "Calculate file block hashes");
    std::cout << std::endl;
    std::cout << "If no directory is given, it uses the current directory" << std::endl;
}

static void CreateHash(const std::string& filename)
{
    std::ifstream ifs(filename, std::ifstream::binary);
    ifs.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>((long)ifs.tellg());
    if (fileSize == 0)
        return;

    unsigned char sha_hash[20] = {};
    sha1_ctx ctx;
    sha1_init(&ctx);

    ifs.seekg(0, std::ios::beg);
    std::vector<char> buffer(4096);
    while (ifs)
    {
        ifs.read(&buffer[0], 4096);
        auto readLength = ifs.gcount();
        sha1_update(&ctx, (const unsigned char*)&buffer[0], (long)readLength);
    }
    sha1_final(&ctx, sha_hash);

    std::ofstream out(filename + ".sha1", std::ios::out);
    out.flags(std::ios_base::hex);
    for (size_t i = 0; i < 20; ++i)
    {
        out << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)sha_hash[i]);
    }
}

static void ProcessEntry(const fs::directory_entry& p)
{
    if (p.is_directory())
        return;
    if (p.path().extension().string() == std::string(Sync::META_FILE_EXT) || p.path().extension().string() == ".sha1")
        return;

    const std::string filename = p.path().string();
    std::cout << "Processing file " << filename << std::endl;
    const auto hashes = Sync::PartitionFile(filename, {});
    Sync::SaveBoundaryList(filename, hashes);
    CreateHash(filename);
}

template<typename T>
static void ProcessDirectory(const T& iterator)
{
    for (const auto& p : iterator)
        ProcessEntry(p);
}

int main(int argc, char** argv)
{
    sa::arg_parser::cli _cli{ {
        { "help", { "-h", "--help", "-?" }, "Show help", false, false, sa::arg_parser::option_type::none },
        { "recursive", { "-R", "--recursive" }, "Processs also subdirectores", false, false, sa::arg_parser::option_type::none }
    } };

    sa::arg_parser::values parsedArgs;
    sa::arg_parser::result cmdres = sa::arg_parser::parse(argc, argv, _cli, parsedArgs);
    auto val = sa::arg_parser::get_value<bool>(parsedArgs, "help");
    if (val.has_value() && val.value())
    {
        ShowHelp(_cli);
        return EXIT_SUCCESS;
    }

    if (!cmdres)
        return EXIT_FAILURE;

    auto recursive = sa::arg_parser::get_value<bool>(parsedArgs, "recursive", false);

    auto dirarg = sa::arg_parser::get_value<std::string>(parsedArgs, "0");
    fs::path dir = dirarg.has_value() ? fs::path(dirarg.value()) : fs::current_path();

    std::cout << "Directory " << dir.string() << std::endl;
    sa::time::timer timer;

    if (recursive)
        ProcessDirectory(fs::recursive_directory_iterator(dir));
    else
        ProcessDirectory(fs::directory_iterator(dir));

    std::cout << "Took " << timer.elapsed_seconds() << " seconds" << std::endl;
    return EXIT_SUCCESS;
}
