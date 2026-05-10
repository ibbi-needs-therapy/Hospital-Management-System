#pragma once
#include "raylib.h"
#include "raygui.h"
#include <cstring>

struct StatusToast {
    char message[256];
    float timer;
    int type;
};

inline StatusToast gToast = { "", 0.0f, 0 };

inline void ShowToast(const char* msg, int type, float duration = 3.0f) {
    strncpy(gToast.message, msg, 255);
    gToast.message[255] = '\0';
    gToast.type = type;
    gToast.timer = duration;
}

inline void UpdateToast() {
    if (gToast.timer > 0.0f) gToast.timer -= GetFrameTime();
}

inline void DrawToast() {
    if (gToast.timer <= 0.0f) return;
    float alpha = (gToast.timer < 0.5f) ? gToast.timer / 0.5f : 1.0f;
    Color bg;
    switch (gToast.type) {
        case 1:  bg = Fade(Color{34, 139, 34, 255}, alpha * 0.9f); break;
        case 2:  bg = Fade(Color{200, 50, 50, 255}, alpha * 0.9f); break;
        default: bg = Fade(Color{60, 120, 200, 255}, alpha * 0.9f); break;
    }
    int sw = GetScreenWidth();
    int fs = 18;
    int tw = MeasureText(gToast.message, fs);
    int pw = tw + 40;
    int ph = 40;
    int px = (sw - pw) / 2;
    int py = 10;
    DrawRectangleRounded({ (float)px, (float)py, (float)pw, (float)ph }, 0.3f, 8, bg);
    DrawText(gToast.message, px + 20, py + (ph - fs) / 2, fs, Fade(WHITE, alpha));
}

struct ConfirmDialog {
    bool active;
    char title[128];
    char message[256];
    int result;
};

inline ConfirmDialog gConfirm = { false, "", "", 0 };

inline void ShowConfirm(const char* title, const char* message) {
    gConfirm.active = true;
    strncpy(gConfirm.title, title, 127); gConfirm.title[127] = '\0';
    strncpy(gConfirm.message, message, 255); gConfirm.message[255] = '\0';
    gConfirm.result = 0;
}

inline bool IsConfirmActive() { return gConfirm.active; }

inline int DrawConfirmDialog() {
    if (!gConfirm.active) return 0;
    int sw = GetScreenWidth(), sh = GetScreenHeight();

    // Dim background
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.5f));

    // Dialog card
    int dw = 420, dh = 200;
    float dx = (float)(sw - dw) / 2, dy = (float)(sh - dh) / 2;
    Rectangle box = { dx, dy, (float)dw, (float)dh };
    DrawRectangleRounded(box, 0.04f, 8, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    DrawRectangleRoundedLinesEx(box, 0.04f, 8, 2.0f, Fade(WHITE, 0.15f));

    DrawText(gConfirm.title, (int)dx + 20, (int)dy + 16, 20, WHITE);
    DrawRectangle((int)dx + 20, (int)dy + 42, dw - 40, 1, Fade(WHITE, 0.15f));

    DrawText(gConfirm.message, (int)dx + 20, (int)dy + 60, 16, Fade(WHITE, 0.8f));
    int btnW = 110, btnH = 38;
    int btnY = (int)dy + dh - btnH - 20;
    if (GuiButton({ dx + dw - btnW * 2 - 30, (float)btnY, (float)btnW, (float)btnH }, "Yes")) {
        gConfirm.active = false;
        gConfirm.result = 1;
        return 1;
    }
    if (GuiButton({ dx + dw - btnW - 15, (float)btnY, (float)btnW, (float)btnH }, "No")) {
        gConfirm.active = false;
        gConfirm.result = 2;
        return 2;
    }
    return 0;
}

inline bool DrawBackButton(int x, int y, int w, int h) {
    return GuiButton({ (float)x, (float)y, (float)w, (float)h }, "< Back");
}

inline bool GuiPasswordBox(Rectangle bounds, char* text, int maxLen, bool* editMode) {
    int len = (int)strlen(text);
    char display[256] = {};
    for (int i = 0; i < len && i < 255; i++) display[i] = '*';

    int prevState = GuiGetState();
    if (*editMode) {
        GuiSetState(STATE_FOCUSED);
        GuiTextBox(bounds, display, maxLen, false);
        GuiSetState(prevState);
        int fs = GuiGetStyle(DEFAULT, TEXT_SIZE);
        int tw = MeasureText(display, fs);
        int cx = (int)bounds.x + tw + 12;
        int cy = (int)bounds.y + ((int)bounds.height - fs) / 2;
        if (((int)(GetTime() * 2.0)) % 2 == 0)
            DrawRectangle(cx, cy, 2, fs, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED)));
    } else {
        GuiTextBox(bounds, display, maxLen, false);
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *editMode = CheckCollisionPointRec(GetMousePosition(), bounds);
    }
    if (*editMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && len < maxLen - 1) {
                text[len++] = (char)key;
                text[len] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
            text[--len] = '\0';
        }
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ENTER)) {
            *editMode = false;
        }
    }
    return false;
}

inline bool GuiLabeledInput(int x, int y, int w, const char* label,
    char* buf, int bufSize, bool* editMode)
{
    int labelFs = 14;
    DrawText(label, x, y, labelFs, Fade(WHITE, 0.6f));
    bool toggled = GuiTextBox({ (float)x, (float)(y + labelFs + 4),
        (float)w, 36.0f }, buf, bufSize, *editMode);
    if (toggled) *editMode = !(*editMode);
    return toggled;
}

inline void GuiLabeledPassword(int x, int y, int w, const char* label,
    char* buf, int bufSize, bool* editMode)
{
    int labelFs = 14;
    DrawText(label, x, y, labelFs, Fade(WHITE, 0.6f));
    GuiPasswordBox({ (float)x, (float)(y + labelFs + 4),
        (float)w, 36.0f }, buf, bufSize, editMode);
}

inline void DrawInfoRow(int x, int y, const char* label, const char* value,
    int labelW, int fontSize, Color labelCol, Color valueCol)
{
    DrawText(label, x, y, fontSize, labelCol);
    DrawText(value, x + labelW, y, fontSize, valueCol);
}

inline void HandleScroll(int* scrollOffset, int maxScroll) {
    int wheel = (int)GetMouseWheelMove();
    *scrollOffset -= wheel * 3;
    if (*scrollOffset < 0) *scrollOffset = 0;
    if (*scrollOffset > maxScroll) *scrollOffset = maxScroll;
}
