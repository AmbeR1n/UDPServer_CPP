#include "DataGramParser.h"

DataGramParser::DataGramParser(int size, char* data)
{
    dgram_size = size;
    this->data = std::string(data);
    header_b = "<HEADER>";
    header_e = "</HEADER>";
    tail_b = "<TAIL>";
    tail_e = "</TAIL>";
    sep = "<SEP>";
}

std::vector<std::string> DataGramParser::GetHeader()
{
    auto start = data.find(header_b) + header_b.length();
    auto end = data.find(header_e);
    return Separate(data.substr(start, end-start));
}


std::vector<std::string> DataGramParser::GetTail()
{
    auto start = data.find(tail_b) + tail_b.length();
    auto end = data.find(tail_e);
    return Separate(data.substr(start, end-start));
}

std::vector<std::string> DataGramParser::GetData()
{
    auto start = data.find(header_e) + header_e.length();
    auto end = data.find(tail_b);
    return Separate(data.substr(start, end-start));
}

std::vector<std::string> DataGramParser::Separate(std::string data)
{
    std::vector<std::string> output;
    int start = 0;
    while (auto pos_b = data.find(sep, start) + sep.length() != data.length())
    {
        auto pos_e = data.find(sep, pos_b);
        output.push_back(data.substr(pos_b, pos_e-pos_b));
    }
    return output;
}