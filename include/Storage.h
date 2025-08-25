#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include "Flashcard.h"
#include "Topic.h"

class Storage{
public:
    static bool LoadFlashcards(const std::string& filename, std::vector<Flashcard>& outCards);
    static bool SaveFlashcards(const std::string& filename, const std::vector<Flashcard>& cards);

    static void SaveTopics(const std::string &filename, const std::vector<Topic> &topics);
    static void LoadTopics(const std::string &filename, std::vector<Topic> &topics);
};

#endif