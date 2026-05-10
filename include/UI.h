#pragma once
#include "raylib.h"
#include "raygui.h"
#include <string>
#include <functional>
#include <cstring>
#include <cstdio>
using namespace std;

namespace Theme {
    constexpr int   WIN_W      = 1280;
    constexpr int   WIN_H      = 720;
    constexpr float SIDEBAR_W  = 220.f;
    constexpr float PAD        = 14.f;
    constexpr float BTN_H      = 42.f;
    constexpr float INPUT_H    = 42.f;
    constexpr float ROW_H      = 38.f;
    constexpr int   FS_TITLE   = 32;
    constexpr int   FS_HEAD    = 22;
    constexpr int   FS_BODY    = 18;
    constexpr int   FS_SMALL   = 16;

    inline Font     gFont;

    // Dynamic colors (initialized to defaults)
    inline Color BG_DARK      = {  9,  14,  26, 255 };
    inline Color BG_PANEL     = { 16,  24,  44, 255 };
    inline Color BG_PANEL2    = { 22,  34,  60, 255 };
    inline Color BG_ROW_ALT   = { 19,  28,  50, 255 };
    inline Color BORDER       = { 38,  68, 128, 255 };
    inline Color ACCENT       = { 58, 130, 247, 255 };
    inline Color ACCENT_HOV   = { 90, 158, 255, 255 };
    inline Color ACCENT_PRE   = { 34,  90, 195, 255 };
    inline Color ACCENT_DIM   = { 28,  54, 110, 255 };
    inline Color SUCCESS      = { 39, 174,  96, 255 };
    inline Color WARNING      = {220, 155,  30, 255 };
    inline Color DANGER       = {210,  55,  55, 255 };
    inline Color TXT_PRI      = {218, 228, 245, 255 };
    inline Color TXT_SEC      = {120, 142, 182, 255 };
    inline Color TXT_DIM      = { 62,  78, 110, 255 };
    inline Color SIDEBAR_BG   = { 11,  18,  34, 255 };

    inline Color withAlpha(Color c, unsigned char a) { c.a = a; return c; }

    inline Color statusColor(const string& s) {
        if (s == "completed") return SUCCESS;
        if (s == "pending")   return WARNING;
        if (s == "cancelled") return DANGER;
        if (s == "noshow")    return { 160, 80, 220, 255 };
        if (s == "paid")      return SUCCESS;
        if (s == "unpaid")    return DANGER;
        return TXT_SEC;
    }

    inline Color midCol(Color a, Color b, float t) {
        if (t < 0) t = 0; 
        if (t > 1) t = 1;
        return {
            (unsigned char)(a.r + (b.r - a.r) * t),
            (unsigned char)(a.g + (b.g - a.g) * t),
            (unsigned char)(a.b + (b.b - a.b) * t),
            (unsigned char)(a.a + (b.a - a.a) * t)
        };
    }

    inline void ResetToDefault() {
        BG_DARK      = {  9,  14,  26, 255 };
        BG_PANEL     = { 16,  24,  44, 255 };
        BG_PANEL2    = { 22,  34,  60, 255 };
        BG_ROW_ALT   = { 19,  28,  50, 255 };
        BORDER       = { 38,  68, 128, 255 };
        ACCENT       = { 58, 130, 247, 255 };
        ACCENT_HOV   = { 90, 158, 255, 255 };
        ACCENT_PRE   = { 34,  90, 195, 255 };
        ACCENT_DIM   = { 28,  54, 110, 255 };
        SUCCESS      = { 39, 174,  96, 255 };
        WARNING      = {220, 155,  30, 255 };
        DANGER       = {210,  55,  55, 255 };
        TXT_PRI      = {218, 228, 245, 255 };
        TXT_SEC      = {120, 142, 182, 255 };
        TXT_DIM      = { 62,  78, 110, 255 };
        SIDEBAR_BG   = { 11,  18,  34, 255 };
        
        GuiLoadStyleDefault();
    }

    inline void UpdateFromGuiStyle() {
        auto getC = [](int prop) { return GetColor(GuiGetStyle(DEFAULT, prop)); };
        
        BG_DARK    = getC(BACKGROUND_COLOR);
        TXT_PRI    = getC(TEXT_COLOR_NORMAL);
        TXT_SEC    = getC(TEXT_COLOR_FOCUSED);
        BORDER     = getC(BORDER_COLOR_NORMAL);
        ACCENT     = getC(LINE_COLOR);
        
        // Derived colors
        BG_PANEL   = withAlpha(BG_DARK, 255); 
        BG_PANEL.r = (BG_PANEL.r > 20) ? BG_PANEL.r - 5 : BG_PANEL.r + 10;
        BG_PANEL.g = (BG_PANEL.g > 20) ? BG_PANEL.g - 5 : BG_PANEL.g + 10;
        BG_PANEL.b = (BG_PANEL.b > 20) ? BG_PANEL.b - 5 : BG_PANEL.b + 10;

        BG_PANEL2  = getC(BASE_COLOR_NORMAL);
        BG_ROW_ALT = withAlpha(BG_PANEL2, 180);
        
        ACCENT_HOV = getC(BASE_COLOR_FOCUSED);
        ACCENT_PRE = getC(BASE_COLOR_PRESSED);
        ACCENT_DIM = withAlpha(ACCENT, 100);
        
        // Force raygui to use our preferred font sizing for its widgets
        GuiSetStyle(DEFAULT, TEXT_SIZE, FS_BODY);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
        
        SIDEBAR_BG = withAlpha(BG_DARK, 255);
        if (SIDEBAR_BG.r > 10) SIDEBAR_BG.r -= 5;
        if (SIDEBAR_BG.g > 10) SIDEBAR_BG.g -= 5;
        if (SIDEBAR_BG.b > 10) SIDEBAR_BG.b -= 5;

        TXT_DIM = withAlpha(TXT_SEC, 120);
    }
    // Themed DrawText wrapper using global font
    inline void DrawText(const char* text, int x, int y, int fontSize, Color color) {
        if (gFont.texture.id > 0) {
            DrawTextEx(gFont, text, { (float)x, (float)y }, (float)fontSize, 1.0f, color);
        } else {
            ::DrawText(text, x, y, fontSize, color);
        }
    }

    inline int MeasureText(const char* text, int fontSize) {
        if (gFont.texture.id > 0) {
            return (int)MeasureTextEx(gFont, text, (float)fontSize, 1.0f).x;
        } else {
            return ::MeasureText(text, fontSize);
        }
    }
}

inline void drawTextInRect(const char* txt, Rectangle r, int fs, Color col, bool centreH = false) {
    if (Theme::gFont.texture.id > 0) {
        Vector2 sz = MeasureTextEx(Theme::gFont, txt, (float)fs, 1.0f);
        float tx = centreH ? r.x + (r.width - sz.x) / 2.f : r.x + 8.f;
        float ty = r.y + (r.height - sz.y) / 2.f;
        DrawTextEx(Theme::gFont, txt, { tx, ty }, (float)fs, 1.0f, col);
    } else {
        int tw = MeasureText(txt, fs);
        int tx = centreH ? (int)(r.x + (r.width - tw) / 2.f) : (int)(r.x + 8);
        int ty = (int)(r.y + (r.height - fs) / 2.f);
        ::DrawText(txt, tx, ty, fs, col);
    }
}

inline void drawSep(float y, float x0 = Theme::SIDEBAR_W, float x1 = Theme::WIN_W) {
    DrawLine((int)x0, (int)y, (int)x1, (int)y, Theme::BORDER);
}

inline void drawCard(Rectangle r, Color fill, Color border = Theme::BORDER) {
    DrawRectangleRec(r, fill);
    DrawRectangleLinesEx(r, 1.f, border);
}

inline void drawBadge(const string& status, float x, float y) {
    Color col = Theme::statusColor(status);
    int fs = Theme::FS_SMALL;
    int tw = MeasureText(status.c_str(), fs);
    Rectangle pill = { x, y, (float)(tw + 12), (float)(fs + 8) };
    DrawRectangleRec(pill, Theme::withAlpha(col, 40));
    DrawRectangleLinesEx(pill, 1.f, Theme::withAlpha(col, 120));
    DrawText(status.c_str(), (int)(x + 6), (int)(y + 4), fs, col);
}

class Button {
public:
    enum class Style { Primary, Secondary, Danger, Success, Ghost };

private:
    Rectangle        mRect;
    char             mLabel[64];
    Color            mIdleCol, mHovCol, mPreCol, mCurCol;
    bool             mEnabled;
    float            mLerp;
    function<void()> mCb;

    void applyStyle(Style s) {
        switch (s) {
            case Style::Primary:
                this->mIdleCol = Theme::ACCENT;    this->mHovCol = Theme::ACCENT_HOV; this->mPreCol = Theme::ACCENT_PRE; break;
            case Style::Secondary:
                this->mIdleCol = Theme::BG_PANEL2; this->mHovCol = Theme::BORDER;     this->mPreCol = Theme::ACCENT_DIM; break;
            case Style::Danger:
                this->mIdleCol = {130,30,30,255};  this->mHovCol = Theme::DANGER;     this->mPreCol = {100,15,15,255};   break;
            case Style::Success:
                this->mIdleCol = {20,110,55,255};  this->mHovCol = Theme::SUCCESS;    this->mPreCol = {12,80,38,255};    break;
            case Style::Ghost:
                this->mIdleCol = {0,0,0,0};        this->mHovCol = Theme::BG_PANEL2;  this->mPreCol = Theme::BG_PANEL;   break;
        }
        this->mCurCol = this->mIdleCol;
    }

public:
    Button() {
        this->mRect = {0, 0, 0, 0};
        memset(this->mLabel, 0, 64);
        this->mIdleCol = Theme::ACCENT;
        this->mHovCol = Theme::ACCENT_HOV;
        this->mPreCol = Theme::ACCENT_PRE;
        this->mCurCol = Theme::ACCENT;
        this->mEnabled = true;
        this->mLerp = 0.f;
    }

    void init(const char* label, Rectangle rect, Style s = Style::Primary) {
        this->mRect = rect;
        strncpy(this->mLabel, label, 63);
        this->applyStyle(s);
    }

    void setCallback(function<void()> cb) { this->mCb = cb; }
    void setEnabled(bool e)               { this->mEnabled = e; }
    void setLabel(const char* l)          { strncpy(this->mLabel, l, 63); }
    void setStyle(Style s)                { this->applyStyle(s); }
    void setRect(Rectangle r)             { this->mRect = r; }
    Rectangle getRect() const             { return this->mRect; }

    // Call every frame
    void update() {
        if (!this->mEnabled) {
            this->mCurCol = Theme::ACCENT_DIM;
            return;
        }
        Vector2 mp = GetMousePosition();
        bool hov   = CheckCollisionPointRec(mp, this->mRect);
        bool press = hov && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        float target = hov ? 1.f : 0.f;
        this->mLerp += (target - this->mLerp) * 12.f * GetFrameTime();

        if (press) {
            this->mCurCol = this->mPreCol;
        }
        else {
            this->mCurCol = Theme::midCol(this->mIdleCol, this->mHovCol, this->mLerp);
        }

        if (hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && this->mCb) {
            this->mCb();
        }
    }

    void draw() const {
        DrawRectangleRec(this->mRect, this->mCurCol);
        DrawRectangleLinesEx(this->mRect, 1.f, Theme::withAlpha(Theme::BORDER, this->mEnabled ? 180 : 60));
        int fs = Theme::FS_BODY;
        int tw = MeasureText(this->mLabel, fs);
        float tx = this->mRect.x + (this->mRect.width  - tw) / 2.f;
        float ty = this->mRect.y + (this->mRect.height - fs) / 2.f;
        Theme::DrawText(this->mLabel, (int)tx, (int)ty, fs, this->mEnabled ? Theme::TXT_PRI : Theme::TXT_DIM);
        if (this->mEnabled && CheckCollisionPointRec(GetMousePosition(), this->mRect)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
    }
};

class TextBox {
    Rectangle mRect;
    char      mBuf[256];
    char      mPH[128];
    bool      mFocused;
    bool      mPassword;
    int       mMaxLen;
    float     mCursorT;
    bool      mCursorOn;

public:
    TextBox() {
        this->mRect = {0, 0, 0, 0};
        memset(this->mBuf, 0, sizeof(this->mBuf));
        memset(this->mPH, 0, sizeof(this->mPH));
        this->mFocused = false;
        this->mPassword = false;
        this->mMaxLen = 64;
        this->mCursorT = 0.f;
        this->mCursorOn = true;
    }

    void init(const char* placeholder, Rectangle rect,
              bool password = false, int maxLen = 64) {
        this->mRect = rect;
        strncpy(this->mPH, placeholder, 127);
        this->mPassword = password;
        this->mMaxLen   = maxLen < 255 ? maxLen : 255;
        memset(this->mBuf, 0, sizeof(this->mBuf));
    }

    void        clear()                    { memset(this->mBuf, 0, sizeof(this->mBuf)); }
    const char* getValue()   const         { return this->mBuf; }
    bool        isFocused()  const         { return this->mFocused; }
    void        setFocused(bool f)         { this->mFocused = f; }
    Rectangle   getRect()    const         { return this->mRect; }

    // Click-to-focus
    void handleClick() {
        this->mFocused = CheckCollisionPointRec(GetMousePosition(), this->mRect);
    }

    void update() {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->handleClick();
        }

        if (!this->mFocused) return;

        this->mCursorT += GetFrameTime();
        if (this->mCursorT >= 0.5f) {
            this->mCursorOn = !this->mCursorOn;
            this->mCursorT = 0.f;
        }

        int key = GetCharPressed();
        int len = (int)strlen(this->mBuf);
        while (key > 0) {
            if (key >= 32 && key < 127 && len < this->mMaxLen) {
                this->mBuf[len++] = (char)key;
                this->mBuf[len]   = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
            this->mBuf[--len] = '\0';
        }
    }

    void draw() const {
        Color border = mFocused ? Theme::ACCENT : Theme::BORDER;
        DrawRectangleRec(mRect, Theme::BG_PANEL2);
        DrawRectangleLinesEx(mRect, mFocused ? 1.5f : 1.f, border);

        int fs = Theme::FS_BODY;
        float ty = mRect.y + (mRect.height - fs) / 2.f;
        float tx = mRect.x + 10.f;

        int len = (int)strlen(mBuf);
        if (len == 0) {
            Theme::DrawText(mPH, (int)tx, (int)ty, fs, Theme::TXT_DIM);
        } else {
            if (mPassword) {
                char masked[256] = {};
                for (int i = 0; i < len && i < 255; i++) masked[i] = '*';
                Theme::DrawText(masked, (int)tx, (int)ty, fs, Theme::TXT_PRI);
            } else {
                char disp[256]; strncpy(disp, mBuf, 255);
                float maxW = mRect.width - 20.f;
                while (strlen(disp) > 0 && Theme::MeasureText(disp, fs) > (int)maxW) {
                    int dl = strlen(disp);
                    memmove(disp, disp + 1, dl);
                }
                Theme::DrawText(disp, (int)tx, (int)ty, fs, Theme::TXT_PRI);
            }
        }

        if (mFocused && mCursorOn) {
            char tmp[256] = {};
            if (mPassword) { for (int i=0;i<len&&i<255;i++) tmp[i]='*'; }
            else           { strncpy(tmp, mBuf, 255); }
            float cw = (float)Theme::MeasureText(tmp, fs);
            float cx = tx + cw + 1.f;
            if (cx < mRect.x + mRect.width - 6.f)
                DrawLine((int)cx, (int)(mRect.y + 6), (int)cx, (int)(mRect.y + mRect.height - 6), Theme::ACCENT);
        }

        if (CheckCollisionPointRec(GetMousePosition(), mRect))
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
};

constexpr int MAX_COLS = 8;
constexpr int MAX_ROWS = 300;
constexpr int MAX_CELL = 80;

struct TableCol { char header[48]; float width; };

struct TableRow {
    char    cells[MAX_COLS][MAX_CELL];
    int     numCells = 0;
    Color   highlight = {0,0,0,0};
};

class Table {
    Rectangle  mBounds    = {};
    TableCol   mCols[MAX_COLS];
    int        mNumCols   = 0;
    TableRow   mRows[MAX_ROWS];
    int        mNumRows   = 0;
    int        mSelected  = -1;
    float      mScroll    = 0.f;
    float      mScrollX   = 0.f;

    float colX(int c) const {
        float x = mBounds.x - mScrollX;
        for (int i = 0; i < c; i++) x += mCols[i].width;
        return x;
    }

    float totalWidth() const {
        float w = 0;
        for (int i = 0; i < mNumCols; i++) w += mCols[i].width;
        return w;
    }

public:
    void init(Rectangle bounds, const TableCol* cols, int numCols) {
        mBounds  = bounds;
        mNumCols = numCols < MAX_COLS ? numCols : MAX_COLS;
        for (int i = 0; i < mNumCols; i++) mCols[i] = cols[i];
        mNumRows = 0; mSelected = -1; mScroll = 0.f; mScrollX = 0.f;
    }

    void clearRows()                  { mNumRows = 0; mSelected = -1; mScroll = 0.f; mScrollX = 0.f; }
    int  getSelected()          const { return mSelected; }
    int  getNumRows()           const { return mNumRows; }
    const TableRow* getSelectedRow() const {
        if (mSelected >= 0 && mSelected < mNumRows) return &mRows[mSelected];
        return nullptr;
    }

    void addRow(const char* const* cells, int numCells, Color highlight = {0,0,0,0}) {
        if (mNumRows >= MAX_ROWS) return;
        TableRow& r = mRows[mNumRows];
        r.numCells  = numCells < MAX_COLS ? numCells : MAX_COLS;
        r.highlight = highlight;
        for (int i = 0; i < r.numCells; i++) strncpy(r.cells[i], cells[i], MAX_CELL - 1);
        mNumRows++;
    }

    void update() {
        Vector2 mp = GetMousePosition();
        if (CheckCollisionPointRec(mp, mBounds)) {
            float wheel = GetMouseWheelMove();
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                mScrollX -= wheel * 30.f;
            } else {
                mScroll -= wheel * Theme::ROW_H * 2.f;
            }
            
            float maxScroll = (float)(mNumRows) * Theme::ROW_H - (mBounds.height - Theme::ROW_H);
            if (maxScroll < 0) maxScroll = 0;
            if (mScroll < 0) mScroll = 0;
            if (mScroll > maxScroll) mScroll = maxScroll;

            float maxScrollX = totalWidth() - mBounds.width;
            if (maxScrollX < 0) maxScrollX = 0;
            if (mScrollX < 0) mScrollX = 0;
            if (mScrollX > maxScrollX) mScrollX = maxScrollX;
        }
        // Click to select
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mp, mBounds)) {
            float ry = mp.y - mBounds.y - Theme::ROW_H + mScroll;
            if (ry >= 0 && mp.y < mBounds.y + mBounds.height - 10) {
                int row = (int)(ry / Theme::ROW_H);
                if (row >= 0 && row < mNumRows)
                    mSelected = (mSelected == row) ? -1 : row;
            }
        }
    }

    void draw() const {
        // Background
        DrawRectangleRec(mBounds, Theme::BG_PANEL);
        DrawRectangleLinesEx(mBounds, 1.f, Theme::BORDER);

        // Clipping via scissor
        BeginScissorMode((int)mBounds.x, (int)mBounds.y, (int)mBounds.width, (int)mBounds.height);

        // Header
        Rectangle hdr = { mBounds.x, mBounds.y, mBounds.width, Theme::ROW_H };
        DrawRectangleRec(hdr, Theme::BG_PANEL2);
        for (int c = 0; c < mNumCols; c++) {
            Theme::DrawText(mCols[c].header, (int)(colX(c) + 8), (int)(mBounds.y + (Theme::ROW_H - Theme::FS_SMALL) / 2.f), Theme::FS_SMALL, Theme::TXT_SEC);
        }
        DrawLine((int)mBounds.x, (int)(mBounds.y + Theme::ROW_H), (int)(mBounds.x + mBounds.width), (int)(mBounds.y + Theme::ROW_H), Theme::BORDER);

        // Rows
        for (int r = 0; r < mNumRows; r++) {
            float ry = mBounds.y + Theme::ROW_H + r * Theme::ROW_H - mScroll;
            if (ry + Theme::ROW_H < mBounds.y + Theme::ROW_H) continue;
            if (ry > mBounds.y + mBounds.height)              break;

            Rectangle rowRect = { mBounds.x, ry, mBounds.width, Theme::ROW_H };

            if (r == mSelected) {
                DrawRectangleRec(rowRect, Theme::withAlpha(Theme::ACCENT, 45));
            } else if (mRows[r].highlight.a > 0) {
                DrawRectangleRec(rowRect, mRows[r].highlight);
            } else if (r % 2 == 1) {
                DrawRectangleRec(rowRect, Theme::BG_ROW_ALT);
            }

            for (int c = 0; c < mRows[r].numCells; c++) {
                float cx = colX(c) + 8.f;
                float cy = ry + (Theme::ROW_H - Theme::FS_BODY) / 2.f;
                Theme::DrawText(mRows[r].cells[c], (int)cx, (int)cy, Theme::FS_BODY, Theme::TXT_PRI);
            }
        }

        EndScissorMode();

        // Vertical Scrollbar
        float totalH = mNumRows * Theme::ROW_H;
        float visH   = mBounds.height - Theme::ROW_H;
        if (totalH > visH && totalH > 0) {
            float barH  = visH * visH / totalH;
            if (barH < 24) barH = 24;
            float barY  = mBounds.y + Theme::ROW_H + (mScroll / totalH) * visH;
            DrawRectangle((int)(mBounds.x + mBounds.width - 5), (int)barY, 4, (int)barH, Theme::BORDER);
        }

        // Horizontal Scrollbar
        float totW = totalWidth();
        if (totW > mBounds.width) {
            float barW = mBounds.width * (mBounds.width / totW);
            if (barW < 24) barW = 24;
            float barX = mBounds.x + (mScrollX / totW) * mBounds.width;
            DrawRectangle((int)barX, (int)(mBounds.y + mBounds.height - 5), (int)barW, 4, Theme::ACCENT);
        }
    }
};

// ---------------------------------------------------------------
//  MESSAGE BOX  (modal)
// ---------------------------------------------------------------
class MessageBox {
public:
    enum class Type { Info, Error, Warning, Confirm, Success };

private:
    char             mTitle[128];
    char             mBody[512];
    bool             mVisible;
    bool             mHasCancel;
    Color            mBarCol;
    Button           mOkBtn, mCancelBtn;
    function<void(bool)> mCb;
    Rectangle        mBox;

public:
    MessageBox() {
        memset(this->mTitle, 0, 128);
        memset(this->mBody, 0, 512);
        this->mVisible = false;
        this->mHasCancel = false;
        this->mBarCol = Theme::ACCENT;
        this->mBox = {0, 0, 0, 0};
    }

    void init() {
        float bw = 440.f, bh = 210.f;
        float bx = (Theme::WIN_W - bw) / 2.f;
        float by = (Theme::WIN_H - bh) / 2.f;
        this->mBox = { bx, by, bw, bh };

        this->mOkBtn.init("OK",     { bx + bw - 116.f, by + bh - 52.f, 100.f, 36.f }, Button::Style::Primary);
        this->mCancelBtn.init("Cancel", { bx + bw - 226.f, by + bh - 52.f, 100.f, 36.f }, Button::Style::Secondary);

        this->mOkBtn.setCallback([this]()     { this->mVisible = false; if (this->mCb) this->mCb(true);  });
        this->mCancelBtn.setCallback([this]() { this->mVisible = false; if (this->mCb) this->mCb(false); });
    }

    void show(const char* title, const char* body, Type t = Type::Info, function<void(bool)> cb = nullptr) {
        strncpy(this->mTitle, title, 127);
        strncpy(this->mBody,  body,  511);
        this->mCb        = cb;
        this->mHasCancel = (t == Type::Confirm);
        this->mVisible   = true;
        this->mOkBtn.setLabel(t == Type::Confirm ? "Confirm" : "OK");
        switch (t) {
            case Type::Error:   this->mBarCol = Theme::DANGER;  break;
            case Type::Warning: this->mBarCol = Theme::WARNING; break;
            case Type::Success: this->mBarCol = Theme::SUCCESS; break;
            default:            this->mBarCol = Theme::ACCENT;  break;
        }
    }

    void hide()            { this->mVisible = false; }
    bool isVisible() const { return this->mVisible; }

    // Returns true if event consumed
    bool update() {
        if (!this->mVisible) return false;
        this->mOkBtn.update();
        if (this->mHasCancel) this->mCancelBtn.update();
        return true;
    }

    void draw() const {
        if (!this->mVisible) return;
        DrawRectangle(0, 0, Theme::WIN_W, Theme::WIN_H, {0, 0, 0, 160});
        drawCard(this->mBox, Theme::BG_PANEL);
        DrawRectangle((int)this->mBox.x, (int)this->mBox.y, (int)this->mBox.width, 4, this->mBarCol);
        Theme::DrawText(this->mTitle, (int)(this->mBox.x + Theme::PAD), (int)(this->mBox.y + 20), Theme::FS_HEAD, Theme::TXT_PRI);
        float ty = this->mBox.y + 58.f;
        const char* p = this->mBody;
        char lineBuf[128] = {};
        int  li = 0;
        while (*p) {
            if (*p == '\n' || li >= 127) {
                lineBuf[li] = '\0';
                Theme::DrawText(lineBuf, (int)(this->mBox.x + Theme::PAD), (int)ty, Theme::FS_BODY, Theme::TXT_SEC);
                ty += Theme::FS_BODY + 4;
                li = 0;
            } else { lineBuf[li++] = *p; }
            p++;
        }
        lineBuf[li] = '\0';
        if (li > 0) Theme::DrawText(lineBuf, (int)(this->mBox.x + Theme::PAD), (int)ty, Theme::FS_BODY, Theme::TXT_SEC);

        this->mOkBtn.draw();
        if (this->mHasCancel) this->mCancelBtn.draw();
    }
};

constexpr int MAX_SIDEBAR_ITEMS = 10;

struct SidebarItem { char label[48]; function<void()> action; };

class Sidebar {
    char        mPortal[48];
    char        mUser[64];
    SidebarItem mItems[MAX_SIDEBAR_ITEMS];
    int         mNumItems;
    Button      mBtns[MAX_SIDEBAR_ITEMS];
    Button      mLogoutBtn;
    int         mActive;

public:
    Sidebar() {
        memset(this->mPortal, 0, 48);
        memset(this->mUser, 0, 64);
        this->mNumItems = 0;
        this->mActive = 0;
    }

    void init(const char* portal, const char* user,
              const SidebarItem* items, int numItems,
              function<void()> onLogout)
    {
        strncpy(this->mPortal, portal, 47);
        strncpy(this->mUser,   user,   63);
        this->mNumItems = numItems < MAX_SIDEBAR_ITEMS ? numItems : MAX_SIDEBAR_ITEMS;
        for (int i = 0; i < this->mNumItems; i++) {
            this->mItems[i] = items[i];
            float y   = 90.f + i * 42.f;
            this->mBtns[i].init(items[i].label, { 2.f, y, Theme::SIDEBAR_W - 4.f, 38.f }, Button::Style::Ghost);
            int idx = i;
            this->mBtns[i].setCallback([this, idx]() {
                this->mActive = idx;
                this->mItems[idx].action();
            });
        }
        this->mLogoutBtn.init("Logout",
            { 2.f, Theme::WIN_H - 52.f, Theme::SIDEBAR_W - 4.f, 38.f },
            Button::Style::Danger);
        this->mLogoutBtn.setCallback(onLogout);
    }

    void setActive(int i) { this->mActive = i; }

    void update() {
        for (int i = 0; i < this->mNumItems; i++) {
            this->mBtns[i].update();
        }
        this->mLogoutBtn.update();
    }

    void draw() const {
        // Background
        DrawRectangle(0, 0, (int)Theme::SIDEBAR_W, Theme::WIN_H, Theme::SIDEBAR_BG);
        DrawRectangle(0, 0, (int)Theme::SIDEBAR_W, 4, Theme::ACCENT);
        DrawLine((int)Theme::SIDEBAR_W, 0, (int)Theme::SIDEBAR_W, Theme::WIN_H, Theme::BORDER);

        // Portal name + user
        Theme::DrawText(this->mPortal, (int)Theme::PAD, 14, Theme::FS_HEAD, Theme::TXT_PRI);
        Theme::DrawText(this->mUser,   (int)Theme::PAD, 46, Theme::FS_SMALL, Theme::TXT_SEC);
        DrawLine((int)Theme::PAD, 78, (int)(Theme::SIDEBAR_W - Theme::PAD), 78, Theme::BORDER);

        for (int i = 0; i < this->mNumItems; i++) {
            if (i == this->mActive) {
                Rectangle r = this->mBtns[i].getRect();
                DrawRectangleRec({ r.x, r.y, r.width, r.height }, Theme::withAlpha(Theme::ACCENT, 38));
                DrawRectangle(0, (int)r.y, 3, (int)r.height, Theme::ACCENT);
            }
            this->mBtns[i].draw();
        }
        this->mLogoutBtn.draw();
    }
};

inline void drawLabel(const char* txt, float x, float y,
                      int fs = Theme::FS_SMALL, Color col = Theme::TXT_SEC) {
    Theme::DrawText(txt, (int)x, (int)y, fs, col);
}