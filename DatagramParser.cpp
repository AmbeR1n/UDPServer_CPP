#include "DatagramParser.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

DatagramParser::DatagramParser(std::string sending_data)
{
    this->data = sending_data;
    this->header = std::string();
    this->tail = std::string();
    header_b = "<H>";
    header_e = "</H>";
    tail_b = "<T>";
    tail_e = "</T>";
    sep = "<S>";
}

std::vector<std::string> DatagramParser::GetHeader()
{
    if(header.empty())
        {
            if (!ExtractHeader())
            {
                return std::vector<std::string>();
        }}
    auto begin = header.find(header_b) + header_b.length();
    auto end = header.find(header_e);
    auto output = Separate(header.substr(begin, end-begin));
    return output;
}

std::vector<std::string> DatagramParser::GetTail()
{
    if(tail.empty())
        {
            if (!ExtractTail())
                return std::vector<std::string>();
        }
    auto begin = tail.find(tail_b) + tail_b.length();
    auto end = tail.find(tail_e);
    return Separate(tail.substr(begin, end-begin));
}

std::string DatagramParser::GetData()
{
    return data;
}

std::vector<std::string> DatagramParser::Separate(std::string metadata)
{
    std::vector<std::string> output;
    char* pch = strtok(const_cast<char*>(metadata.c_str()), sep.c_str());
    while (pch != NULL)
    {
        output.push_back(pch);
        pch = strtok(NULL, sep.c_str());
    }
    return output;
}

std::string DatagramParser::BuildDatagram()
{
    return header + data + tail;
}

void DatagramParser::BuildHeader(std::vector<std::string> header_params)
{
    if (header_params.empty())
        return;
    std::string str_header = header_b;
    for (auto h_data : header_params)
        str_header.append(h_data + sep);
    header = str_header + header_e;
    header.insert(header.length()-header_e.length(), std::string(128 - header.length(), '0'));
}

void DatagramParser::BuildTail(std::vector<std::string> tail_params)
{
    if (tail_params.empty())
        return;
    std::string str_tail = tail_b;
    for (auto t_data : tail_params)
        str_tail.append(t_data + sep);
    tail = str_tail.substr(0, str_tail.length()-sep.length()) + tail_e;
}

bool DatagramParser::ExtractHeader()
{
    if (data.find(header_b) < std::string::npos)
    {
        header = data.substr(data.find(header_b), data.find(header_e)-data.find(header_b));
        data = data.substr(header.length());
        return true;
    }
    return false; 
}

bool DatagramParser::ExtractTail()
{
    if (data.find(tail_b) != std::string::npos)
    {
        tail = data.substr(data.find(tail_b), data.find(tail_e)-data.find(tail_b));
        data = data.substr(tail.length());
        return true;
    }
    return false;
}

int DatagramParser::DatagramSize()
{
    return header.length()+data.length()+tail.length();
}

int DatagramParser::DataSize()
{
    ExtractHeader();
    ExtractTail();
    return data.length();
}
