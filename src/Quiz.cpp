#include "Quiz.h"
#include <algorithm>
#include <random>
#include <ctime>

Quiz::Quiz() : flashcards(nullptr), currentIndex(0), correctCount(0){}

void Quiz::StartQuiz(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions){
    flashcards = &allFlashcards;
    selectedIndices.clear();

    // Collect flashcards for the topic
    for(const auto& term : topic.terms){
        auto it = std::find_if(allFlashcards.begin(), allFlashcards.end(),
            [&](const Flashcard& fc){ return fc.term == term; });
        if(it != allFlashcards.end()){
            selectedIndices.push_back((int)std::distance(allFlashcards.begin(), it));
        }
    }

    // Limit to numQuestions if specified
    if(numQuestions > 0 && numQuestions < (int)selectedIndices.size()){
        std::shuffle(selectedIndices.begin(), selectedIndices.end(), std::default_random_engine((unsigned)time(nullptr)));
        selectedIndices.resize(numQuestions);
    }

    ShuffleIndices();
    ResetProgress();
}

void Quiz::RetrySameTerms(){
    ShuffleIndices();
    ResetProgress();
}

void Quiz::RetryNewTerms(const Topic& topic, const std::vector<Flashcard>& allFlashcards, int numQuestions){
    StartQuiz(topic, allFlashcards, numQuestions);
}

void Quiz::ShuffleIndices(){
    std::shuffle(selectedIndices.begin(), selectedIndices.end(), std::default_random_engine((unsigned)time(nullptr)));
}

void Quiz::ResetProgress(){
    currentIndex = 0;
    correctCount = 0;
    missedIndices.clear();
}

const Flashcard& Quiz::GetCurrentFlashcard() const{
    return (*flashcards)[selectedIndices[currentIndex]];
}

void Quiz::SubmitAnswer(const std::string& answer){
    if(IsFinished()) return;

    const Flashcard& fc = GetCurrentFlashcard();
    if(answer == fc.definition){
        correctCount++;
    } else{
        missedIndices.push_back(selectedIndices[currentIndex]);
    }
}

bool Quiz::IsFinished() const{
    return currentIndex >= (int)selectedIndices.size();
}

void Quiz::NextQuestion(){
    if (!IsFinished()) {
        currentIndex++;
    }
}

int Quiz::GetScore() const{
    return correctCount;
}

int Quiz::GetTotalQuestions() const{
    return (int)selectedIndices.size();
}

const std::vector<int>& Quiz::GetMissedIndices() const{
    return missedIndices;
}
