#include "Storage.h"
#include "Flashcard.h"
#include <json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool Storage::LoadFlashcards(const std::string& filename,std::vector<Flashcard>& outCards){
    std::ifstream file(filename);
    if(!file.good()){
        outCards.clear();
        return false;
    }

    json j;
    file >> j;
    file.close();

    if(!j.is_array()){
        std::cerr << "Invalid JSON format in " << filename << "\n";
        return false;
    }

    outCards.clear();
    for(auto& item:j){
        Flashcard fc;
        fc.term = item.value("term","");
        fc.definition = item.value("definition","");
        outCards.push_back(fc);
    }

    return true;
}

bool Storage::SaveFlashcards(const std::string& filename,const std::vector<Flashcard>& cards){
    json j = json::array();
    for(const auto& fc:cards){
        j.push_back({{"term",fc.term},{"definition",fc.definition}});
    }

    std::ofstream file(filename);
    if(!file.is_open()){
        std::cerr << "Could not open " << filename << " for writing.\n";
        return false;
    }

    file << j.dump(4);
    file.close();
    return true;
}

void Storage::SaveTopics(const std::string &filename, const std::vector<Topic> &topics){
    json j = json::array();
    for(auto &t : topics){
        j.push_back({{"name", t.name}, {"terms", t.terms}});
    }
    std::ofstream out(filename);
    out << j.dump(4);
}

void Storage::LoadTopics(const std::string &filename, std::vector<Topic> &topics){
    std::ifstream in(filename);
    if(!in.is_open()) return;

    json j;
    in >> j;

    topics.clear();
    for(auto &t : j){
        Topic topic;
        topic.name = t["name"];
        topic.terms = t["terms"].get<std::vector<std::string>>();
        topics.push_back(topic);
    }
}