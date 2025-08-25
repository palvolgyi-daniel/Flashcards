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

    int selectedIndex = -1; //flashcard index
    std::string newTerm;
    std::string newDefinition;
};


#endif