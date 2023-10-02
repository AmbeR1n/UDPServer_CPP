#include <string>
#include <vector>

class DataGramParser
{
    private:
        int dgram_size;
        std::string data;
        std::string header_b;
        std::string header_e;
        std::string tail_b;
        std::string tail_e;
        std::string sep;

    public:
        DataGramParser(int size, char *data);
        std::vector<std::string> GetTail();
        std::vector<std::string> GetHeader();
        std::vector<std::string> GetData();
        std::vector<std::string> Separate(std::string data);
};