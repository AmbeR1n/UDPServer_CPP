class ProgressBar
{
private:
    int start;
    char reached;
    char unreached;
    int size;
    int bar_size;
    int current_size;
    double speed;
    double progress;
    char *progress_bar;
    void CreateBar();
public:
    ProgressBar();
    ProgressBar(char reached, char unreached, int size);
    void PrintLine();
    void Update(int sent_size, long nanoseconds);
};