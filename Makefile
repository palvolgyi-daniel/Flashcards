CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/json
LDFLAGS = -static -lglfw3 -lopengl32 -lgdi32

SRC_DIR = src

SRC = $(wildcard $(SRC_DIR)/*.cpp) \
      vendor/imgui/imgui.cpp \
      vendor/imgui/imgui_draw.cpp \
      vendor/imgui/imgui_tables.cpp \
      vendor/imgui/imgui_widgets.cpp \
      vendor/imgui/backends/imgui_impl_glfw.cpp \
      vendor/imgui/backends/imgui_impl_opengl3.cpp

OBJ = $(SRC:.cpp=.o)
EXE = flashcards.exe

all: deps $(EXE)

$(EXE): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

deps:
	@if [ ! -d vendor/imgui ]; then \
		echo "Fetching ImGui..."; \
		git clone --filter=blob:none --no-checkout https://github.com/ocornut/imgui.git vendor/imgui; \
		cd vendor/imgui; \
		git sparse-checkout init --cone; \
		git sparse-checkout set imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp imconfig.h imstb_rectpack.h imstb_textedit.h imstb_truetype.h backends/ ; \
		git checkout master; \
	fi
	@if [ ! -f vendor/json/json.hpp ]; then \
		echo "Fetching nlohmann/json..."; \
		mkdir -p vendor/json; \
		curl -L https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o vendor/json/json.hpp; \
	fi

clean:
	rm -f $(OBJ) $(EXE)