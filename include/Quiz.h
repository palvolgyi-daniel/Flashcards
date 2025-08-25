#ifndef QUIZ_H
#define QUIZ_H

#include <vector>
#include <string>
#include "Flashcard.h"
#include "Topic.h"

class Quiz{
public:
    Quiz();

    void StartQuiz(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions = -1);

    // Retry options
    void RetrySameTerms();
    void RetryNewTerms(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions = -1);

    // Quiz flow
    const Flashcard& GetCurrentFlashcard() const;
    void SubmitAnswer(const std::string& answer);
    bool IsFinished() const;
    void NextQuestion();

    // Results
    int GetScore() const;
    int GetTotalQuestions() const;
    const std::vector<int>& GetMissedIndices() const;

private:
    const std::vector<Flashcard>* flashcards;
    std::vector<int> selectedIndices;
    int currentIndex;
    int correctCount;
    std::vector<int> missedIndices;

    void ShuffleIndices();
    void ResetProgress();
};

#endif
