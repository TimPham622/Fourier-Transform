#include "raylib.h"
#include "Complex.h"
#include "DFT.h"
#include <vector>
#include <algorithm> // For std::sort
#include <cmath>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const float PI = 3.14159265358979323846f;

bool compareAmp(const FourierCoef& a, const FourierCoef& b) {
    return a.amp > b.amp;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fourier Epicycles - Draw with Mouse!");
    SetTargetFPS(60);

    enum State { USER_DRAWING, CALCULATING, ANIMATING };
    State currentState = USER_DRAWING;

    std::vector<Complex> drawing;           
    std::vector<FourierCoef> fourierX;   
    std::vector<Vector2> path;              
    float time = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == USER_DRAWING) {
            DrawText("Draw a continuous line. Right Click to Process.", 10, 10, 20, RAYWHITE);
            
            // Input Logic
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                if (drawing.empty() || 
                    (drawing.back().re != mouse.x || drawing.back().im != mouse.y)) {
                    drawing.push_back(Complex(mouse.x, mouse.y));
                }
            }
            for (size_t i = 0; i < drawing.size(); i++) {
                DrawPixel(drawing[i].re, drawing[i].im, YELLOW);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && drawing.size() > 0) {
                currentState = CALCULATING;
            }
        } 
        else if (currentState == CALCULATING) {
            DrawText("Calculating DFT...", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2, 20, RAYWHITE);
            
            fourierX = dft(drawing);
            
            std::sort(fourierX.begin(), fourierX.end(), compareAmp);
            
            currentState = ANIMATING;
            time = 0;
            path.clear();
        } 
        else if (currentState == ANIMATING) {
            DrawText("Press SPACE to reset", 10, 10, 20, DARKGRAY);

            Vector2 v = {0, 0}; 
            
            for (size_t i = 0; i < fourierX.size(); i++) {
                Vector2 prevObj = v;
                
                float freq = fourierX[i].freq;
                float radius = fourierX[i].amp;
                float phase = fourierX[i].phase;

                float theta = freq * time + phase + (PI / 2); 
                
                float x = radius * std::cos(theta);
                float y = radius * std::sin(theta);
                
                v.x += x;
                v.y += y;

                if (radius > 1.0f) {
                    DrawCircleLines(prevObj.x, prevObj.y, radius, Fade(WHITE, 0.3f));
                    DrawLineV(prevObj, v, Fade(WHITE, 0.5f));
                }
            }

            path.insert(path.begin(), v); 
            
            if(path.size() > 5000) path.pop_back();

            for (size_t i = 0; i < path.size() - 1; i++) {
                DrawLineV(path[i], path[i+1], YELLOW);
            }

            float dt = (2 * PI) / fourierX.size();
            time += dt;

            if (time > 2 * PI) {
                time = 0;
                path.clear();
            }
            
            if (IsKeyPressed(KEY_SPACE)) {
                currentState = USER_DRAWING;
                drawing.clear();
                path.clear();
                fourierX.clear();
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}