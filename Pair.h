#ifndef PAIR_H
#define PAIR_H
#include <string>

class Pair
{
    public:
        Pair(std::string sen, int wordCount = 0);
        ~Pair();

        int wordCount;
        std::string sentence;

};

#endif // PAIR_H
