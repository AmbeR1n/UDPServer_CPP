#include <string>
class ProgressBar
{
private:
    int duration;
    int count;
    int start;
    char reached;
    char unreached;
    uint64_t size;
    double speed;
    double progress;
    double sum_speed;
    std::string progress_bar;
public:
    ProgressBar(uint64_t size);
    void PrintLine();
    void PrintFinal();
    void Update(uint64_t sent_size);
};