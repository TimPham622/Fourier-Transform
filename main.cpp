#include "raylib.h"
#include "raymath.h"
#include "Complex.h"
#include "DFT.h"   
#include "FFT.h"    
#include <vector>
#include <algorithm>
#include <cmath>
#include <complex>   

const int SCREEN_WIDTH = 1512;
const int SCREEN_HEIGHT = 800;

const int FFT_SIZE = 1024; 
std::vector<std::complex<float>> audioBuffer(FFT_SIZE);
int audioIndex = 0;

void ProcessAudio(void *bufferData, unsigned int frames) {
    float *samples = (float *)bufferData;

    for (unsigned int i = 0; i < frames; i++) {
        if (audioIndex < FFT_SIZE) {
            float leftChannel = samples[i];
            audioBuffer[audioIndex] = std::complex<float>(leftChannel, 0.0f);
            audioIndex++;
        }
    }
}

bool compareAmp(const FourierCoef& a, const FourierCoef& b) {
    return a.amp > b.amp;
}

Color Checkered(int i) {
    return (i % 2 == 0) ? ORANGE : GOLD;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fourier Playground");
    InitAudioDevice(); 
    SetTargetFPS(60);
    Texture2D bgTexture = LoadTexture("background.jpg");

    enum AppState { MENU, EPICYCLES, VISUALIZER };
    AppState currentState = MENU;

    std::vector<Complex> drawing;
    std::vector<FourierCoef> fourierX;
    std::vector<Vector2> path;
    float time = 0;
    bool isEpicycleCalculating = false;
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.target = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };

    Music music = LoadMusicStream("song.mp3");
    music.looping = true;
    AttachAudioStreamProcessor(music.stream, ProcessAudio);
    float smoothBass = 0.0f; 

    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            currentState = MENU;
            StopMusicStream(music); 
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentState) {
            
            case MENU: {
                DrawText("THE FOURIER PLAYGROUND", 350, 200, 50, WHITE);
                DrawText("Epicycle Drawing Machine", 400, 350, 30, YELLOW);
                DrawText("Audio Visualiser", 400, 400, 30, GREEN);
                DrawText("[Select with Number Keys]", 450, 600, 20, DARKGRAY);

                if (IsKeyPressed(KEY_ONE)) {
                    currentState = EPICYCLES;
                    camera.zoom = 1.0f;
                    camera.target = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
                }
                if (IsKeyPressed(KEY_TWO)) {
                    currentState = VISUALIZER;
                    PlayMusicStream(music);
                    audioIndex = 0;
                }
                break;
            }


            case EPICYCLES: {
                float targetZoom = 1.0f;
                bool rightDragPanning = false;
                Vector2 rightDragAccum = {0,0};
                float wheel = GetMouseWheelMove();

                if (IsKeyPressed(KEY_R)) {
                    camera.zoom = 1.0f;
                    targetZoom = 1.0f;
                    camera.target = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
                }


                if (wheel != 0) camera.zoom += wheel * 0.1f;
                if (wheel != 0.0f) {

                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorldBefore = GetScreenToWorld2D(mouseScreen, camera);

                float zoomFactor = 1.0f + wheel * 0.15f; 
                targetZoom = Clamp(targetZoom * zoomFactor, 0.1f, 10.0f);

                camera.zoom = targetZoom;

                Vector2 mouseWorldAfter = GetScreenToWorld2D(mouseScreen, camera);

                Vector2 delta = Vector2Subtract(mouseWorldBefore, mouseWorldAfter);
                camera.target = Vector2Add(camera.target, delta);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    rightDragPanning = false;
                    rightDragAccum = (Vector2){0,0};
                }

                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    Vector2 d = GetMouseDelta();
                    rightDragAccum = Vector2Add(rightDragAccum, d);

                    if (Vector2Length(rightDragAccum) > 6.0f) {
                        rightDragPanning = true;
                    }

                    if (rightDragPanning) {
                        camera.target.x -= d.x / camera.zoom;
                        camera.target.y -= d.y / camera.zoom;
                    }
                }

                if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
                    Vector2 delta = GetMouseDelta();
                    camera.target.x -= delta.x / camera.zoom;
                    camera.target.y -= delta.y / camera.zoom;
                }


                BeginMode2D(camera);
                
                if (!isEpicycleCalculating && fourierX.empty()) {

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        Vector2 mouseRaw = GetMousePosition();
                        Vector2 mouse = GetScreenToWorld2D(mouseRaw, camera);
                        if (drawing.empty()) drawing.push_back(Complex(mouse.x, mouse.y));
                        else {
                            float dx = mouse.x - drawing.back().re;
                            float dy = mouse.y - drawing.back().im;
                            if (dx*dx + dy*dy > 25) drawing.push_back(Complex(mouse.x, mouse.y));
                        }
                    }
                    for (auto& p : drawing) DrawPixel(p.re, p.im, YELLOW);
                    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && !rightDragPanning && !drawing.empty()) {
                    isEpicycleCalculating = true;
                    }

                }
                else if (isEpicycleCalculating) {
                    DrawText("Calculating...", camera.target.x, camera.target.y, 20, WHITE);
                    fourierX = dft(drawing); 
                    std::sort(fourierX.begin(), fourierX.end(), compareAmp);
                    isEpicycleCalculating = false;
                    time = 0;
                    path.clear();
                }
                else {
                    Vector2 v = {0,0};
                    for (size_t i = 1; i < fourierX.size(); i++) {
                        Vector2 prev = v;
                        float theta = fourierX[i].freq * time + fourierX[i].phase;
                        v.x += fourierX[i].amp * std::cos(theta);
                        v.y += fourierX[i].amp * std::sin(theta);
                        if(fourierX[i].amp > 1.0f) {
                            DrawCircleLines(prev.x, prev.y, fourierX[i].amp, Fade(WHITE, 0.2f));
                            DrawLineV(prev, v, Fade(WHITE, 0.2f));
                        }
                    }
                    path.insert(path.begin(), v);
                    if (path.size() > 2000) path.pop_back();
                    for (size_t i = 0; i < path.size() - 1; i++) DrawLineV(path[i], path[i+1], YELLOW);
                    
                    time += (2 * PI) / fourierX.size();
                    if (time > 2 * PI) { time = 0; path.clear(); }
                    
                    if (IsKeyPressed(KEY_SPACE)) {
                        drawing.clear(); fourierX.clear(); path.clear();
                    }
                }
                EndMode2D();
                
                DrawText("Backspace: Return to Menu", 10, 10, 20, GRAY);
                break;
            }

            case VISUALIZER: {
                UpdateMusicStream(music);

                static std::vector<float> smoothedBars(FFT_SIZE / 2, 0.0f);
                static float targetBass = 0.0f;
                
                Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTexture.width, (float)bgTexture.height };
                Rectangle destRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
                DrawTexturePro(bgTexture, sourceRec, destRec, {0,0}, 0.0f, WHITE);
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));

                if (audioIndex >= FFT_SIZE) {
                    std::vector<std::complex<float>> processingBuffer = audioBuffer;
                    fft(processingBuffer); 

                    float currentBass = 0;
                    for(int i=0; i<15; i++) currentBass += std::abs(processingBuffer[i]);
                    targetBass = currentBass; 

                    for (int i = 0; i < FFT_SIZE / 2; i++) {
                        float amp = std::abs(processingBuffer[i]);
                        float targetHeight = std::log(amp + 1.0f) * 80.0f; 
                        
                        smoothedBars[i] = Lerp(smoothedBars[i], targetHeight, 0.2f);
                    }
                    
                    audioIndex = 0;
                }

                smoothBass = Lerp(smoothBass, targetBass, 0.1f);

                float bassVis = Clamp(smoothBass*0.5f, 0.0f, 100.0f);
                Color c = ColorFromHSV(20.0f + bassVis*2.0f, 0.9f, 0.7f);

                Vector2 center = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                
                float circleRadius = 50 + smoothBass;

                DrawCircleV(center, circleRadius, Fade(c, 0.65f));
                DrawCircleLines(center.x, center.y, circleRadius, c);

                int maxFreq = FFT_SIZE / 2;
                float angleStep = 360.0f / maxFreq;

                for (int i = 0; i < maxFreq; i++) {
                    float height = smoothedBars[i]; 

                    if (height < 2.0f) continue;

                    float angle = i * angleStep;
                    float rad = (angle * PI) / 180.0f;
                    
                    float startRadius = circleRadius + 10.0f; 
                    
                    Vector2 start = {
                        center.x + std::cos(rad) * startRadius,
                        center.y + std::sin(rad) * startRadius
                    };
                    Vector2 end = {
                        center.x + std::cos(rad) * (startRadius + height),
                        center.y + std::sin(rad) * (startRadius + height)
                    };

                    DrawLineEx(start, end, 2.0f, Checkered(i));
                }
                
                DrawText("Playing: song.mp3", 10, SCREEN_HEIGHT - 30, 20, GREEN);
                DrawText("Backspace: Return to Menu", 10, 10, 20, GRAY);
                break;
            }
        }

        EndDrawing();
    }
    UnloadTexture(bgTexture);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

