#include "DatagramParser.h"
#include <stdio.h>
#include <string.h>

DatagramParser::DatagramParser(int size, std::string data, std::vector<std::string> header, std::vector<std::string> tail)
{
    data_size = size;
    this->data = data;
    this->header = header;
    this->tail = tail;
    header_b = "<H>";
    header_e = "</H>";
    tail_b = "<T>";
    tail_e = "</T>";
    sep = "<S>";
}

DatagramParser::DatagramParser(int size, std::string data)
{
    data_size = size;
    this->data = data;
    this->header = std::vector<std::string>();
    this->tail = std::vector<std::string>();
    header_b = "<H>";
    header_e = "</H>";
    tail_b = "<T>";
    tail_e = "</T>";
    sep = "<S>";
}

DatagramParser::DatagramParser(int size, std::string data, std::vector<std::string> header)
{
    data_size = size;
    this->data = data;
    this->header = header;
    this->tail = std::vector<std::string>();
    header_b = "<H>";
    header_e = "</H>";
    tail_b = "<T>";
    tail_e = "</T>";
    sep = "<S>";
}

std::vector<std::string> DatagramParser::GetHeader()
{
    auto start = data.find(header_b) + header_b.length();
    auto end = data.find(header_e);
    return Separate(data.substr(start, end-start));
}



std::vector<std::string> DatagramParser::GetTail()
{
    auto start = data.find(tail_b) + tail_b.length();
    auto end = data.find(tail_e);
    return Separate(data.substr(start, end-start));
}

std::vector<std::string> DatagramParser::GetData()
{
    auto start = data.find(header_e) + header_e.length();
    auto end = data.find(tail_b);
    return Separate(data.substr(start, end-start));
}

std::vector<std::string> DatagramParser::Separate(std::string data)
{
    std::vector<std::string> output;
    char* pch = strtok(const_cast<char*>(data.c_str()), sep.c_str());
        while (pch != NULL)
        {
            output.push_back(pch);
            pch = strtok(NULL, sep.c_str());
        }
    return output;
}

std::string DatagramParser::BuildDatagram(std::vector<std::string> h, std::vector<std::string> t, std::string d)
{
    return header_b + BuildHeader(h) + header_e + d + tail_b + BuildTail(t) + tail_e;
}

std::string DatagramParser::BuildHeader(std::vector<std::string> h)
{
    std::string head;
    for (auto h_data : h)
        head.append(h_data);
    return head;
}

std::string DatagramParser::BuildTail(std::vector<std::string> t)
{
    std::string tail;
    for (auto t_data : t)
        tail.append(t_data);
    return tail;
}

int DatagramParser::DatagramSize()
{
    return 0;
}
