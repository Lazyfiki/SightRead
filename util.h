#ifndef UTIL_H
#define UTIL_H

double getCurrentTime();
int getRandomNumber(int min, int max);
void renderText(SDL_Renderer* renderer, const char* text, SDL_Color color, int x, int y);

#endif // UTIL_H
