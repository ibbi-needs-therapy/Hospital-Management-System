#pragma once
#include "raylib.h"
#include "raygui.h"
#include "UI.h"

static const float BASE_W = 1280.0f;
static const float BASE_H = 720.0f;
static const int HEADER_H = 56;
static const int BTN_H = 40;
static const int INPUT_H = 36;
static const int PAD = 20;
static const int CARD_PAD = 16;
static const int ROW_H = 30;

inline float GetUIScale() {
    float sx = (float)GetScreenWidth() / BASE_W;
    float sy = (float)GetScreenHeight() / BASE_H;
    return (sx < sy) ? sx : sy;
}

inline int S(int val) { return (int)(val * GetUIScale()); }
inline float Sf(float val) { return val * GetUIScale(); }

inline Rectangle CenteredRect(int w, int h) {
    int sw = GetScreenWidth(), sh = GetScreenHeight();
    return { (float)(sw - w) / 2, (float)(sh - h) / 2, (float)w, (float)h };
}

inline Rectangle ScaledRect(int x, int y, int w, int h) {
    return { (float)S(x), (float)S(y), (float)S(w), (float)S(h) };
}

inline Color GetBgColor() {
    return GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
}

inline Color GetPanelColor() {
    Color bg = GetBgColor();
    return Color{
        (unsigned char)(bg.r + (bg.r < 200 ? 15 : -15)),
        (unsigned char)(bg.g + (bg.g < 200 ? 15 : -15)),
        (unsigned char)(bg.b + (bg.b < 200 ? 15 : -15)),
        255
    };
}

inline Color GetTextColor() {
    return GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
}

inline Color GetAccentColor() {
    return GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
}

inline void DrawPanel(Rectangle r, Color bg) {
    DrawRectangleRounded(r, 0.015f, 8, bg);
    DrawRectangleRoundedLinesEx(r, 0.015f, 8, 1.0f, Fade(WHITE, 0.08f));
}

inline void DrawHeaderBar(const char* title, Color bg, Color textCol) {
    int sw = GetScreenWidth();
    int fs = S(22);
    DrawRectangle(0, 0, sw, S(HEADER_H), bg);
    DrawRectangleGradientV(0, S(HEADER_H), sw, S(4), Fade(BLACK, 0.2f), BLANK);
    int tw = MeasureText(title, fs);
    DrawText(title, (sw - tw) / 2, (S(HEADER_H) - fs) / 2, fs, textCol);
}

inline void DrawSectionTitle(int x, int y, const char* title, int fontSize, Color col) {
    DrawText(title, x, y, fontSize, col);
    DrawRectangle(x, y + fontSize + 4, MeasureText(title, fontSize), 2, Fade(col, 0.4f));
}

inline void DrawMedicalCross(int cx, int cy, int size, Color col) {
    int arm = size / 3;
    DrawRectangle(cx - arm / 2, cy - size / 2, arm, size, col);
    DrawRectangle(cx - size / 2, cy - arm / 2, size, arm, col);
}

inline void DrawCard(Rectangle r, Color bg) {
    DrawRectangleRounded({ r.x + 3, r.y + 3, r.width, r.height }, 0.02f, 8, Fade(BLACK, 0.15f));
    DrawRectangleRounded(r, 0.02f, 8, bg);
    DrawRectangleRoundedLinesEx(r, 0.02f, 8, 1.0f, Fade(WHITE, 0.05f));
}

inline void DrawFieldLabel(int x, int y, const char* label, int fontSize) {
    DrawText(label, x, y, fontSize, Fade(GetTextColor(), 0.7f));
}

inline void DrawDivider(int x, int y, int width) {
    DrawRectangle(x, y, width, 1, Fade(WHITE, 0.1f));
}


inline void DrawTableHeader(Rectangle bounds, const char** cols, int colCount,
    const int* colWidths, Color bg, Color text)
{
    DrawRectangleRec(bounds, bg);
    float x = bounds.x + 8;
    int fs = (int)(bounds.height * 0.5f);
    if (fs < 10) fs = 10;
    int ty = (int)(bounds.y + (bounds.height - fs) / 2);
    for (int i = 0; i < colCount; i++) {
        DrawText(cols[i], (int)x, ty, fs, text);
        x += colWidths[i];
    }
}

inline void DrawTableRow(Rectangle bounds, const char** vals, int colCount,
    const int* colWidths, Color bg, Color text, bool highlight)
{
    DrawRectangleRec(bounds, highlight ? Fade(WHITE, 0.05f) : bg);
    DrawRectangle((int)bounds.x, (int)(bounds.y + bounds.height - 1),
        (int)bounds.width, 1, Fade(WHITE, 0.03f));
    float x = bounds.x + 8;
    int fs = (int)(bounds.height * 0.45f);
    if (fs < 10) fs = 10;
    int ty = (int)(bounds.y + (bounds.height - fs) / 2);
    for (int i = 0; i < colCount; i++) {
        DrawText(vals[i], (int)x, ty, fs, text);
        x += colWidths[i];
    }
}

inline bool DrawClickableRow(Rectangle bounds, const char** vals, int colCount,
    const int* colWidths, Color bg, Color text, bool highlight)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    DrawRectangleRec(bounds, hovered ? Fade(WHITE, 0.1f) : (highlight ? Fade(WHITE, 0.05f) : bg));
    DrawRectangle((int)bounds.x, (int)(bounds.y + bounds.height - 1),
        (int)bounds.width, 1, Fade(WHITE, 0.03f));
    float x = bounds.x + 8;
    int fs = (int)(bounds.height * 0.45f);
    if (fs < 10) fs = 10;
    int ty = (int)(bounds.y + (bounds.height - fs) / 2);
    for (int i = 0; i < colCount; i++) {
        DrawText(vals[i], (int)x, ty, fs, text);
        x += colWidths[i];
    }
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
