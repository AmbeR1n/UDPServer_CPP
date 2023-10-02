#include <string>
#include <vector>

class DatagramParser
{
    private:
        int data_size;
        std::string data;
        std::vector<std::string> header;
        std::vector<std::string> tail;
        std::string header_b;
        std::string header_e;
        std::string tail_b;
        std::string tail_e;
        std::string sep;

    public:
        DatagramParser(int size, std::string data, std::vector<std::string> header, std::vector<std::string> tail);
        DatagramParser(int size, std::string data);
        DatagramParser(int size, std::string data, std::vector<std::string> header);
        std::vector<std::string> GetTail();
        std::vector<std::string> GetHeader();
        std::vector<std::string> GetData();
        std::vector<std::string> Separate(std::string data);
        std::string BuildDatagram(std::vector<std::string> h, std::vector<std::string> t, std::string d);
        std::string BuildHeader(std::vector<std::string> h);
        std::string BuildTail(std::vector<std::string> t);
        int DatagramSize();
};