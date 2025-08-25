#ifndef APP_H
#define APP_H
#include <vector>
#include "Flashcard.h"
#include "Topic.h"

class App{
public:
    App();
    ~App();
    void Run();
    void OnExit();

private:
    std::vector<Flashcard> flashcards;
    std::vector<Topic> topics;
};


#endif