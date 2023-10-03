#include <string>
#include <vector>

class DatagramParser
{
    private:
        std::string data;
        std::string header;
        std::string tail;
        std::string header_b;
        std::string header_e;
        std::string tail_b;
        std::string tail_e;
        std::string sep;
        std::vector<std::string> Separate(std::string metadata);

    public:
        DatagramParser(std::string sending_data);

        std::vector<std::string> GetHeader();
        std::vector<std::string> GetTail();
        std::string GetData();

        void BuildHeader(std::vector<std::string> header_params);
        void BuildTail(std::vector<std::string> tail_params);
        std::string BuildDatagram();
        
        bool ExtractHeader();
        bool ExtractTail();

        int DatagramSize();
        int DataSize();
};