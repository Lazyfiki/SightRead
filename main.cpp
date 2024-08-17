#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <cmath>
#include <csignal>
#include <getopt.h>
#include <iostream>
#include <map>
#include <string.h>
#include <thread>
#include <vector>

#include "util.h"

#define WIDTH 1600
#define HEIGHT 900

#define NOTES_PER_STAFF 12

bool running = true;
int currentNote = 0;

SDL_Color correctNoteColor = { 0, 255, 0, 255 };
SDL_Color wrongNoteColor = { 255, 0, 0, 255 };
SDL_Color idleNoteColor = { 255, 255, 255, 255 };

typedef struct {
    int correctNotes;
    int wrongNotes;
} Npm;
Npm npm = { 0, 0 };

typedef struct {
    char* glyph;
    int position;
    int pressed;
} Note;
Note notes[NOTES_PER_STAFF];

class Key {
private:
    int x, y, width, height;
    bool isWhite;
    bool isPressed;

public:
    Key(int x, int y, int width, int height, bool isWhite)
    : x(x), y(y), width(width), height(height), isWhite(isWhite), isPressed(false) {}

    void render(SDL_Renderer* renderer) {
        SDL_Rect rect = { x, 710, width, height };
        if (isPressed) {
            SDL_SetRenderDrawColor(renderer, 150, 190, 255, 255);
        } else if (isWhite) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    void press() { isPressed = true; }
    void release() { isPressed = false; }
};

std::vector<Key> keys;

float notesPerMinute(Npm npm) {
    int allNotes = npm.correctNotes + npm.wrongNotes;
    return (allNotes - npm.wrongNotes) / 60;
}

void addKeys(int numWhiteKeys) {
    int whiteKeyWidth = 50;
    int whiteKeyHeight = 190;
    for (int i = 0; i < numWhiteKeys; ++i) {
        keys.push_back(Key(i * whiteKeyWidth, 0, whiteKeyWidth, whiteKeyHeight, true));
    }

    int blackKeyWidth = 28;
    int blackKeyHeight = 120;
    for (int i = 0; i < numWhiteKeys; ++i) {
        int xPos = (i * whiteKeyWidth) + (whiteKeyWidth - blackKeyWidth / 2);
        if (i % 7 != 2 && i % 7 != 6) xPos += whiteKeyWidth;
        keys.push_back(Key(xPos, 0, blackKeyWidth, blackKeyHeight, false));
    }
}

void generateNotes() {
    for (int i = 0; i < NOTES_PER_STAFF; ++i) {
        notes[i].glyph = "𝅘𝅥";
        notes[i].position = getRandomNumber(0, 10);
        notes[i].pressed = 0;
    }

}

void renderNotes(SDL_Renderer* renderer) {
    SDL_Color color = idleNoteColor;
    for (int i = 0; i < NOTES_PER_STAFF; ++i) {
        if (notes[i].pressed == 1) {
            color = correctNoteColor;
        } else if (notes[i].pressed == -1) {
            color = wrongNoteColor;
        } else if (notes[i].pressed == 0) {
            color = idleNoteColor;
        }

        if (notes[i].position == 10) {
            int x = ((WIDTH - 400 - 100) / NOTES_PER_STAFF) * i + 300;
            int y = 350 - 74;
            SDL_RenderDrawLine(renderer, x - 5, y, x + 60, y);
        }
        renderText(renderer,
                   notes[i].glyph,
                   color,
                   ((WIDTH - 400 - 100) / NOTES_PER_STAFF) * i + 300,
                   (notes[i].position * 18) - 72
       );
    }
}

void renderStaff(SDL_Renderer* renderer, int padding, int y) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < 5; ++i) {
        SDL_RenderDrawLine(renderer, padding, y + i * 35 + 1, WIDTH - padding, y + i * 35 + 1);
    }
}

void processInput() {
    #include "config.h"

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (event.type == SDL_KEYDOWN) {
            auto it = index.find(event.key.keysym.sym);
            if (it != index.end()) {
                int currentKeyIndex = it->second;
                keys[currentKeyIndex].press();

                if (currentKeyIndex - 15 == 10 - notes[currentNote].position) {
                    npm.correctNotes++;
                    notes[currentNote].pressed = 1;
                } else {
                    npm.wrongNotes++;
                    notes[currentNote].pressed = -1;
                }

                currentNote++;
                if (currentNote == NOTES_PER_STAFF) {
                    currentNote = 0;
                    generateNotes();
                }

                // TODO: add note sounds
            }
        }

        if (event.type == SDL_KEYUP) {
            auto it = index.find(event.key.keysym.sym);
            if (it != index.end()) {
                int currentKeyIndex = it->second;
                keys[currentKeyIndex].release();
            }
        }
    }
}

void update() {
    // NOTE: i have no clue what to put here :/
    std::cout << notesPerMinute(npm) << '\n';
}

void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto& key : keys) {
        key.render(renderer);
    }

    renderStaff(renderer, 200, 100);
    renderStaff(renderer, 200, 350);

    renderText(renderer, "𝄞", idleNoteColor, 200, 54); // treble clef
    renderText(renderer, "𝄢", idleNoteColor, 200, 284);// base clef

    renderNotes(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv) {
    int opt;
    int runTime = 0;

    auto startTime = std::chrono::steady_clock::now();

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
            case 't':
                runTime = std::stoi(optarg);
                break;
            case 'h':
            case '?':
                std::cerr << "Usage: " << argv[0] << " -t <time-in-seconds>\n";
                return 1;

        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "sight reading trainer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        0
    );

    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, 0);
    if (!renderer) {
        SDL_Log("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    addKeys(32);
    generateNotes();

    SDL_Event event;
    double previous = getCurrentTime();
    double lag = 0.0;
    const double MS_PER_UPDATE = 0.016;

    while (running) {
        double current = getCurrentTime();
        double elapsed = current - previous;
        previous = current;
        lag += elapsed;

        processInput();

        while (lag >= MS_PER_UPDATE) {
            update();
            lag -= MS_PER_UPDATE;
        }

        render(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
