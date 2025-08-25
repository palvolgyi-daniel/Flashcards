#include "UI.h"
#include "imgui.h"
#include "Storage.h"
#include <algorithm>

UI::UI(){}

void UI::Render(std::vector<Flashcard> &flashcards, std::vector<Topic> &topics){
    // Route to the appropriate screen
    if(inQuiz){
        RenderQuiz(flashcards);
        return;
    }
    if(showQuizSetup){
        RenderQuizSetup(flashcards, topics);
        return;
    }
    if(showQuizResults){
        RenderQuizResults(flashcards, topics);
        return;
    }
    RenderFlashcards(flashcards, topics);
}

//
// ---------------- Normal Flashcards UI ----------------
//
void UI::RenderFlashcards(std::vector<Flashcard> &flashcards, std::vector<Topic> &topics){
    // validate indices every frame
    if(selectedIndex >= (int)flashcards.size()) selectedIndex = -1;
    if(editingIndex >= (int)flashcards.size()) editingIndex = -1;

    ImGui::SetNextWindowSize(ImVec2(900, 600));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Flashcards", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    float tableWidth = ImGui::GetContentRegionAvail().x;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

    if(ImGui::Button("+", ImVec2(24, 24))){
        openAddModal = true;
        addMode = 0; // default to flashcard
        requestOpenAddPopup = true;
    }

    ImGui::PopStyleVar(2);

    if(ImGui::BeginTable("MainLayout", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV, ImVec2(tableWidth-100, 0))){
        ImGui::TableSetupColumn("Topics", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Terms", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Definition");
        ImGui::TableNextRow();

        // ----------- COLUMN 1: Topics -----------
        ImGui::TableSetColumnIndex(0);
    
        if(ImGui::Selectable("All", selectedTopicIndex == 0)){
            selectedTopicIndex = 0;
            selectedIndex = -1;
        }

        for(int t = 0; t < (int)topics.size(); ++t){
            if(ImGui::Selectable(topics[t].name.c_str(), selectedTopicIndex == t + 1)){
                selectedTopicIndex = t + 1;
                selectedIndex = -1;
            }

            if(ImGui::BeginPopupContextItem((std::string("topic_context_") + std::to_string(t)).c_str())){

                // Start quiz (disabled when the topic has no terms)
                bool canStart = !topics[t].terms.empty();
                if(ImGui::MenuItem("Start quiz...", nullptr, false, canStart)){
                    quizTopicIndex = t;
                    quizNumQuestions = (int)topics[t].terms.size(); // default to all terms
                    showQuizSetup = true;
                    // we switch screens via Render() routing next frame
                }

                if(ImGui::MenuItem("Rename")){
                    renameTopicIndex = t;
                    strncpy(renameTopicBuffer, topics[t].name.c_str(), sizeof(renameTopicBuffer) - 1);
                    renameTopicBuffer[sizeof(renameTopicBuffer)-1] = '\0';
                    requestOpenRenameTopicPopup = true;
                }

                if(ImGui::MenuItem("Delete")){
                    topics.erase(topics.begin() + t);
                    Storage::SaveTopics("data/topics.json", topics);
                    if(selectedTopicIndex == t + 1) selectedTopicIndex = 0; // reset if deleted current
                }
                ImGui::EndPopup();
            }
        }

        // ----------- COLUMN 2: Terms -----------
        ImGui::TableSetColumnIndex(1);
        std::vector<int> visibleIndices;
        if(selectedTopicIndex == 0){
            for(int i = 0; i < (int)flashcards.size(); ++i) visibleIndices.push_back(i);
        } else{
            Topic &topic = topics[selectedTopicIndex - 1];
            for(auto &term : topic.terms){
                auto it = std::find_if(flashcards.begin(), flashcards.end(), [&](const Flashcard &fc){
                    return fc.term == term;
                });
                if(it != flashcards.end()) visibleIndices.push_back((int)std::distance(flashcards.begin(), it));
            }
        }

        for(int idx : visibleIndices){
            if(ImGui::Selectable(flashcards[idx].term.c_str(), selectedIndex == idx)) selectedIndex = idx;

            // Right-click menu
            if(ImGui::BeginPopupContextItem()){

                if(ImGui::BeginMenu("Add to topic...")){
                    if(topics.empty()){
                        ImGui::MenuItem("(no topics)", nullptr, false, false);
                    } else{
                        const std::string &term = flashcards[idx].term;
                        for(int t = 0; t < (int)topics.size(); ++t){
                            std::vector<std::string> &vec = topics[t].terms;
                            bool alreadyInTopic = std::find(vec.begin(), vec.end(), term) != vec.end();
                            if(ImGui::MenuItem(topics[t].name.c_str(), nullptr, alreadyInTopic, !alreadyInTopic)){
                                vec.push_back(term);
                                Storage::SaveTopics("data/topics.json", topics);
                            }
                        }
                    }
                    ImGui::EndMenu();
                }

                if(ImGui::MenuItem("Edit")){
                    editingIndex = idx;
                    strncpy(editTermBuffer, flashcards[idx].term.c_str(), sizeof(editTermBuffer) - 1);
                    editTermBuffer[sizeof(editTermBuffer)-1] = '\0';
                    strncpy(editDefinitionBuffer, flashcards[idx].definition.c_str(), sizeof(editDefinitionBuffer) - 1);
                    editDefinitionBuffer[sizeof(editDefinitionBuffer)-1] = '\0';
                    requestOpenEditFlashcardPopup = true;
                }

                if(selectedTopicIndex == 0){
                    if(ImGui::MenuItem("Delete")){
                        for(auto &topic : topics){
                            topic.terms.erase(std::remove(topic.terms.begin(), topic.terms.end(), flashcards[idx].term), topic.terms.end());
                        }
                        flashcards.erase(flashcards.begin() + idx);
                        Storage::SaveFlashcards("data/flashcards.json", flashcards);
                        Storage::SaveTopics("data/topics.json", topics);
                        selectedIndex = -1;
                    }
                } else{
                    if (ImGui::MenuItem("Remove from topic")){
                        Topic &topic = topics[selectedTopicIndex - 1];
                        topic.terms.erase(std::remove(topic.terms.begin(), topic.terms.end(), flashcards[idx].term), topic.terms.end());
                        Storage::SaveTopics("data/topics.json", topics);
                        selectedIndex = -1;
                    }
                }
                ImGui::EndPopup();
            }
        }

        // ----------- COLUMN 3: Definition -----------
        ImGui::TableSetColumnIndex(2);
        if(selectedIndex >= 0 && selectedIndex < (int)flashcards.size()){
            ImGui::PushTextWrapPos(ImGui::GetColumnWidth() + ImGui::GetCursorPosX());
            ImGui::TextUnformatted(flashcards[selectedIndex].definition.c_str());
            ImGui::PopTextWrapPos();
        } else{
            ImGui::Text("Select a term to see its definition.");
        }

        ImGui::EndTable();
    }

    // pop & open popups
    if(requestOpenAddPopup){
        ImGui::OpenPopup("New...");
        requestOpenAddPopup = false;
    }
    if(requestOpenEditFlashcardPopup){
        ImGui::OpenPopup("Edit Flashcard");
        requestOpenEditFlashcardPopup = false;
    }
    if(requestOpenRenameTopicPopup){
        ImGui::OpenPopup("Rename Topic");
        requestOpenRenameTopicPopup = false;
    }

    // ---------- Rename Topic ----------
    if(ImGui::BeginPopupModal("Rename Topic", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        if(renameTopicIndex >= 0 && renameTopicIndex < (int)topics.size()){
            ImGui::InputText("New name", renameTopicBuffer, IM_ARRAYSIZE(renameTopicBuffer));

            if(ImGui::Button("Save")){
                std::string newName(renameTopicBuffer);
                if(!newName.empty()){
                    bool exists = std::any_of(topics.begin(), topics.end(), [&](const Topic &tp){
                        return tp.name == newName;
                    });
                    if(!exists){
                        topics[renameTopicIndex].name = newName;
                        Storage::SaveTopics("data/topics.json", topics);
                    }
                }
                renameTopicIndex = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if(ImGui::Button("Cancel")){
                renameTopicIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        } else{
            renameTopicIndex = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // ---------- Add modal ----------
    if(ImGui::BeginPopupModal("New...", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        // Mode selection
        if(ImGui::RadioButton("Add Flashcard", addMode == 0)) addMode = 0;
        ImGui::SameLine();
        if(ImGui::RadioButton("Create Topic", addMode == 1)) addMode = 1;

        ImGui::Separator();

        // --- Flashcard creation mode ---
        if(addMode == 0){
            ImGui::InputText("Term", newTermBuffer, IM_ARRAYSIZE(newTermBuffer));
            ImGui::InputTextMultiline("Definition", newDefinitionBuffer, IM_ARRAYSIZE(newDefinitionBuffer), ImVec2(400, 100));

            if(ImGui::Button("Add")){
                std::string term(newTermBuffer);
                std::string def(newDefinitionBuffer);

                if(!term.empty() && !def.empty()){
                    bool exists = std::any_of(flashcards.begin(), flashcards.end(), [&](const Flashcard &fc){
                        return fc.term == term;
                    });
                    if(!exists){
                        flashcards.push_back({term, def});
                        Storage::SaveFlashcards("data/flashcards.json", flashcards);
                    }
                    memset(newTermBuffer, 0, sizeof(newTermBuffer));
                    memset(newDefinitionBuffer, 0, sizeof(newDefinitionBuffer));
                    openAddModal = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")){
                openAddModal = false;
                ImGui::CloseCurrentPopup();
            }
        }

        // --- Topic creation mode
        else if(addMode == 1){
            ImGui::InputText("Topic Name", newTopicBuffer, IM_ARRAYSIZE(newTopicBuffer));

            if(ImGui::Button("Create")){
                std::string name(newTopicBuffer);

                if(!name.empty()){
                    bool exists = std::any_of(topics.begin(), topics.end(), [&](const Topic &tp){
                        return tp.name == name;
                    });
                    if(!exists){
                        topics.push_back({name, {}}); // empty terms initially
                        Storage::SaveTopics("data/topics.json", topics);
                    }
                    memset(newTopicBuffer, 0, sizeof(newTopicBuffer));
                    openAddModal = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")){
                openAddModal = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }

    // ---------- Edit flashcard popup ----------
    if(ImGui::BeginPopupModal("Edit Flashcard", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        if(editingIndex >= 0 && editingIndex < (int)flashcards.size()){
            ImGui::InputText("Term", editTermBuffer, IM_ARRAYSIZE(editTermBuffer));
            ImGui::InputTextMultiline("Definition", editDefinitionBuffer, IM_ARRAYSIZE(editDefinitionBuffer), ImVec2(400, 100));

            if(ImGui::Button("Save")){
                // Prevent duplicates (unless editing same card)
                bool duplicate = false;
                for(int i = 0; i < (int)flashcards.size(); ++i){
                    if(i != editingIndex && flashcards[i].term == std::string(editTermBuffer)){
                        duplicate = true;
                        break;
                    }
                }
                if(!duplicate){
                    std::string oldTerm = flashcards[editingIndex].term;

                    flashcards[editingIndex].term = editTermBuffer;
                    flashcards[editingIndex].definition = editDefinitionBuffer;

                    for(auto &topic : topics){
                        for(auto &t : topic.terms){
                            if(t == oldTerm){
                                t = editTermBuffer;
                            }
                        }
                    }

                    Storage::SaveFlashcards("data/flashcards.json", flashcards);
                    Storage::SaveTopics("data/topics.json", topics);
                }
                editingIndex = -1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")){
                editingIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        } else{
            editingIndex = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

//
// ---------------- Quiz Setup Screen ----------------
//
void UI::RenderQuizSetup(std::vector<Flashcard>& flashcards, std::vector<Topic>& topics){
    ImGui::SetNextWindowSize(ImVec2(900, 600));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Quiz Setup", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    if(quizTopicIndex < 0 || quizTopicIndex >= (int)topics.size()){
        ImGui::Text("Invalid topic selected.");
        if(ImGui::Button("Back")){
            showQuizSetup = false;
        }
        ImGui::End();
        return;
    }

    Topic &topic = topics[quizTopicIndex];

    ImGui::Text("Topic: %s", topic.name.c_str());
    ImGui::Separator();

    ImGui::Text("Terms in topic:");
    if(ImGui::BeginChild("TermsList", ImVec2(0, 300), true)){
        for(auto &term : topic.terms){
            ImGui::BulletText("%s", term.c_str());
        }
        ImGui::EndChild();
    }

    int total = (int)topic.terms.size();
    ImGui::InputInt("Number of questions", &quizNumQuestions);
    if(quizNumQuestions < 1) quizNumQuestions = 1;
    if(quizNumQuestions > total) quizNumQuestions = total;

    if(ImGui::Button("Start quiz")){
        quiz.StartQuiz(topic, flashcards, quizNumQuestions);
        quizShowDefinition = false;
        inQuiz = true;
        showQuizSetup = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")) showQuizSetup = false;

    ImGui::End();
}

//
// ---------------- Quiz Screen ----------------
//
void UI::RenderQuiz(std::vector<Flashcard>& flashcards){
    ImGui::SetNextWindowSize(ImVec2(900, 600));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Quiz", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    // Abort button (top-left)
    if(ImGui::Button("Abort")){
        inQuiz = false;
        quizShowDefinition = false;
        ImGui::End();
        return;
    }

    // Progress (x/y)
    int answered = quiz.GetScore() + (int)quiz.GetMissedIndices().size();
    int total = quiz.GetTotalQuestions();
    int currentDisplay = answered + 1;
    if(currentDisplay > total) currentDisplay = total;

    ImGui::SameLine();
    ImGui::Text("%d/%d", currentDisplay, total);

    if(!quiz.IsFinished()){
        const Flashcard& fc = quiz.GetCurrentFlashcard();

        ImGui::Separator();
        ImGui::Text("Term:");
        ImGui::TextWrapped("%s", fc.term.c_str());

        ImGui::Separator();
        if(!quizShowDefinition){
            if(ImGui::Button("Show definition")) quizShowDefinition = true;
        } else{
            ImGui::Text("Definition:");
            ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x-90);
            ImGui::Text("%s", fc.definition.c_str());
            ImGui::PopTextWrapPos();
        }

        ImGui::Separator();
        if(ImGui::Button("I got it right")){
            // Mark correct by submitting the actual definition (string-compare in Quiz)
            quiz.SubmitAnswer(fc.definition);
            quiz.NextQuestion();
            quizShowDefinition = false;

            if(quiz.IsFinished()){
                inQuiz = false;
                showQuizResults = true;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("I got it wrong")){
            // Mark incorrect by submitting a non-matching string
            quiz.SubmitAnswer(std::string());
            quiz.NextQuestion();
            quizShowDefinition = false;

            if(quiz.IsFinished()){
                inQuiz = false;
                showQuizResults = true;
            }
        }
    } else{
        // Safety: if already finished, go to results
        inQuiz = false;
        showQuizResults = true;
    }

    ImGui::End();
}

//
// ---------------- Quiz Results Screen ----------------
//
void UI::RenderQuizResults(std::vector<Flashcard>& flashcards, std::vector<Topic>& topics){
    ImGui::SetNextWindowSize(ImVec2(900, 600));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Quiz Results", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::Text("Score: %d/%d", quiz.GetScore(), quiz.GetTotalQuestions());
    ImGui::Separator();

    const auto& missed = quiz.GetMissedIndices();
    if(!missed.empty()){
        ImGui::Text("Missed terms:");
        if(ImGui::BeginChild("MissedList", ImVec2(0, 250), true)){
            for(int idx : missed){
                if(idx >= 0 && idx < (int)flashcards.size()) ImGui::BulletText("%s", flashcards[idx].term.c_str());
            }
            ImGui::EndChild();
        }
    } else{
        ImGui::Text("No missed terms, nice!");
    }

    if(ImGui::Button("Home")) showQuizResults = false; // leave everything else as-is
    ImGui::SameLine();
    if(ImGui::Button("Try again with same terms")){
        quiz.RetrySameTerms();
        quizShowDefinition = false;
        inQuiz = true;
        showQuizResults = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Try again with new terms")){
        if(quizTopicIndex >= 0 && quizTopicIndex < (int)topics.size()){
            quiz.RetryNewTerms(topics[quizTopicIndex], flashcards, quizNumQuestions);
            quizShowDefinition = false;
            inQuiz = true;
            showQuizResults = false;
        }
    }

    ImGui::End();
}
