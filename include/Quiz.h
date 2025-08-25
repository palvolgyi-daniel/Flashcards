#ifndef QUIZ_H
#define QUIZ_H

#include <vector>
#include <string>
#include "Flashcard.h"
#include "Topic.h"

class Quiz{
public:
    Quiz();

    // Start a new quiz from a topic
    void StartQuiz(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions = -1);

    // Retry options
    void RetrySameTerms();  // reshuffle the same terms
    void RetryNewTerms(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions = -1);

    // Quiz flow
    const Flashcard& GetCurrentFlashcard() const;
    void SubmitAnswer(const std::string& answer);
    bool IsFinished() const;
    void NextQuestion();

    // Results
    int GetScore() const;
    int GetTotalQuestions() const;
    const std::vector<int>& GetMissedIndices() const; // indices into selectedIndices

private:
    const std::vector<Flashcard>* flashcards; // reference (not owning)
    std::vector<int> selectedIndices;         // indices into flashcards used in this quiz
    int currentIndex;
    int correctCount;
    std::vector<int> missedIndices;           // stores indices from selectedIndices that were wrong

    void ShuffleIndices();
    void ResetProgress();
};

#endif
