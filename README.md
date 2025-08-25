# Flashcards

A lightweight C++ program that lets you **store and rehearse terms/phrases**.

---

## Features
- Create, edit, delete flashcards (term + definition)
- Organize flashcards into topics
- Context menus for quick actions (add to topic, rename/delete topic)
- Quiz mode per topic
  - Choose number of questions
  - Manual correct/incorrect grading
  - Result screen with score and missed terms
  - Retry with same terms or retry with new random terms from the topic
- Clean, minimal UI built with Dear ImGui

---

## Dependencies
The project uses the following libraries:

- **Dear ImGui** (core + GLFW + OpenGL3 backends)
- **GLFW** (window + input)
- **OpenGL** (rendering)
- **nlohmann/json** (single-header JSON)

> The provided `Makefile` automatically fetches Dear ImGui and the single-header `json.hpp`. You still need a system installation of GLFW and an OpenGL-capable environment.

---

## Requirements

### Windows (MSYS2 UCRT64 for best result)
- Install [MSYS2](https://www.msys2.org/), open the **UCRT64** shell, then install toolchain + GLFW:
  ```bash
  pacman -S --needed base-devel git curl
  pacman -S --needed mingw-w64-ucrt-x86_64-toolchain
  pacman -S --needed mingw-w64-ucrt-x86_64-glfw
  ```
- Ensure you are using the `ucrt64` environment so `g++`, `pkg-config`, and `libglfw3` are available.

- **If not using MSYS2**, make sure your environment/library paths allow GLFW and adjust Makefile accordingly.

### General
- C++17-capable compiler (GCC, Clang, MSVC)
- OpenGL 3.0+ runtime/driver

---

## Build & Run

```bash
# 1) Build
make

# 2) Run
./flashcards.exe

# Clean objects/binary
make clean
```

---

## Data Files
The app reads/writes simple JSON files in the `data/` directory:

- `data/flashcards.json` – array of `{"term": "...", "definition": "..."}` objects
- `data/topics.json` – array of topics: `{"name": "...", "terms": ["term1", "term2", ...]}`

You can manage them through the UI or optionally edit these files by hand (with the app closed).

The repo comes with default data files for demo purposes, but they're not required for the app to build and run.

---

## License
This project is released under the **MIT License**.

---

## Acknowledgements
- **nlohmann/json** – a JSON library for C++
- **Dear ImGui** – GUI library for C++

---

## Troubleshooting
- **`cannot find -lglfw`**: If not building via MSYS2 UCRT64, then install GLFW and adjust `LDFLAGS` according to your environment.

