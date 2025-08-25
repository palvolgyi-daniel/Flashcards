#ifndef UI_H
#define UI_H

#include <vector>
#include <string>
#include "Flashcard.h"
#include "Topic.h"
#include "Quiz.h"

class UI{
public:
    UI();
    void Render(std::vector<Flashcard>& flashcards, std::vector<Topic> &topics);
private:
    bool requestOpenEditFlashcardPopup = false;
    bool requestOpenRenameTopicPopup = false;
    bool requestOpenAddPopup = false;

    int selectedIndex = -1; // currently selected flashcard
    int editingIndex = -1;   // flashcard being edited
    int renameTopicIndex = -1;
    int selectedTopicIndex = 0; // selected topic in UI
    bool openAddModal = false;
    int addMode = -1; // 0 = flashcard, 1 = topic

    char newTermBuffer[256] = {0};
    char newDefinitionBuffer[512] = {0};
    char editTermBuffer[256] = {0};
    char editDefinitionBuffer[512] = {0};
    char newTopicBuffer[256] = {0};
    char renameTopicBuffer[256] = {0};

    Quiz quiz;
    bool showQuizSetup = false;
    bool inQuiz = false;
    bool showQuizResults = false;
    int quizTopicIndex = -1;
    int quizNumQuestions = 0;
    bool quizShowDefinition = false; // per-question toggle

    // Render helpers
    void RenderFlashcards(std::vector<Flashcard>& flashcards, std::vector<Topic> &topics);
    void RenderQuizSetup(std::vector<Flashcard>& flashcards, std::vector<Topic>& topics);
    void RenderQuiz(std::vector<Flashcard>& flashcards);
    void RenderQuizResults(std::vector<Flashcard>& flashcards, std::vector<Topic>& topics);
    
    bool TermExists(const std::string &term, const std::vector<Flashcard> &flashcards, int skipIndex = -1);
};

#endif