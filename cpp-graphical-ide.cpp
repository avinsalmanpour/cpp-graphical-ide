#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstring>
#include <regex>
#include <algorithm>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_clipboard.h>
const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 600;
const int MENU_BAR_HEIGHT = 30;
const int FONT_SIZE = 24;
int scrollOffset = 0;
const int LINE_HEIGHT = FONT_SIZE + 6;
bool cursorVisible = true;
bool darkMode = false;
bool showFileMenu = false;
bool showEditMenu = false;
bool showViewMenu = false;
int mouseX = 0;
int mouseY = 0;
struct SyntaxRule {
    std::string keyword;
    SDL_Color color;
};
std::vector<SyntaxRule> lightModeRules = {
        {"class ",    {0,   51,  102, 255}},
        {"if ",      {0,   51,  102, 255}},
        {"while ",    {0,   51,  102, 255}},
        {"return",   {0,   51,  102, 255}},
        {"int ",      {0,   128, 128, 255}},
        {"float ",    {0,   128, 128, 255}},
        {"double ",   {0,   128, 128, 255}},
        {"char ",     {0,   128, 128, 255}},
        {"0",{128, 0, 128,255}},
        {"1",{128, 0, 128,255}},
        {"2",{128, 0, 128,255}},
        {"3",{128, 0, 128,255}},
        {"4",{128, 0, 128,255}},
        {"5",{128, 0, 128,255}},
        {"6",{128, 0, 128,255}},
        {"7",{128, 0, 128,255}},
        {"8",{128, 0, 128,255}},
        {"9",{128, 0, 128,255}},
        {"\"",       {0,   100, 0,   255}},
        {"'",        {0,   100, 0,   255}},
        {"//",       {128, 128, 128, 255}},
        {"#include ", {139, 139, 0,   255}},
        {"+",        {128, 0,   0,   255}},
        {"-",        {128, 0,   0,   255}},
        {"*",        {128, 0,   0,   255}},
        {"/", {128, 0, 0, 255}},
        {"{", {184, 134, 11, 255}},
        {"}", {184, 134, 11, 255}},
        {"(", {184, 134, 11, 255}},
        {")", {184, 134, 11, 255}},
        {"[", {184, 134, 11, 255}},
        {"]", {184, 134, 11, 255}}
};
std::vector<SyntaxRule> darkModeRules = {
        {"class ", {221, 120, 198, 255}},
        {"if ", {221, 120, 198, 255}},
        {"while ", {221, 120, 198, 255}},
        {"return", {221, 120, 198, 255}},
        {"int ", {117, 108, 224, 255}},
        {"float ", {117, 108, 224, 255}},
        {"double ", {117, 108, 224, 255}},
        {"char ", {117, 108, 224, 255}},
        {"0",{209, 154, 102,255}},
        {"1",{209, 154, 102,255}},
        {"2",{209, 154, 102,255}},
        {"3",{209, 154, 102,255}},
        {"4",{209, 154, 102,255}},
        {"5",{209, 154, 102,255}},
        {"6",{209, 154, 102,255}},
        {"7",{209, 154, 102,255}},
        {"8",{209, 154, 102,255}},
        {"9",{209, 154, 102,255}},
        {"\"", {121, 195, 152, 255}},
        {"'", {121, 195, 152, 255}},
        {"//", {112, 99, 92, 255}},
        {"#include ", {194, 182, 86, 255}},
        {"+", {0, 94, 213, 255}},
        {"-", {0, 94, 213, 255}},
        {"*", {0, 94, 213, 255}},
        {"/", {0, 94, 213, 255}},
        {"{", {171, 178, 191, 255}},
        {"}", {171, 178, 191, 255}},
        {"(", {171, 178, 191, 255}},
        {")", {171, 178, 191, 255}},
        {"[", {171, 178, 191, 255}},
        {"]", {171, 178, 191, 255}}
};

std::unordered_map<std::string, bool> libraries = {
        {"<iostream>", false},{"<cmath>", false},{"<bits/stdc++.h>", false},{"<vector>", false},
        {"<algorithm>", false},{"<string>", false},{"<set>", false},{"<map>", false} };
std::vector<SyntaxRule>& getCurrentRules() {
    return darkMode ? darkModeRules : lightModeRules;
}
struct EditorState {
    std::vector<std::string> lines;
    int cursorPos;
    int currentLine;
};
std::vector<std::string> savedProjects;
std::vector<EditorState> undoStack;
std::string clipboardText;
int selectionStart = -1, selectionEnd = -1;
void copyText(const std::vector<std::string>& lines);
void pasteText(std::vector<std::string>& lines, int& cursorPos, int& currentLine);
void cutText(std::vector<std::string>& lines, int& cursorPos, int currentLine);
void selectAll(int& selectionStart, int& selectionEnd, const std::vector<std::string>& lines);
void undo(std::vector<std::string>& lines, int& cursorPos, int& currentLine, SDL_Renderer* renderer, TTF_Font* font);
void goToLine(int& currentLine, int totalLines);
void saveProject(SDL_Renderer* renderer, TTF_Font* font, const std::vector<std::string>& lines);
void handleShortcuts(const SDL_Event& e, std::vector<std::string>& lines, int& cursorPos, int& currentLine, SDL_Renderer* renderer, TTF_Font* font) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) {
        switch (e.key.keysym.sym) {
            case SDLK_c: copyText(lines); break;           // Ctrl + C
            case SDLK_v: pasteText(lines, cursorPos, currentLine); break; // Ctrl + V
            case SDLK_x: cutText(lines, cursorPos, currentLine); break;   // Ctrl + X
            case SDLK_a: selectAll(selectionStart, selectionEnd, lines); break; // Ctrl + A
            case SDLK_z: undo(lines, cursorPos, currentLine,renderer,font); break;     // Ctrl + Z
            case SDLK_s: saveProject(renderer, font, lines); break;    // Ctrl + S
            case SDLK_g: goToLine(currentLine, lines.size()); break;
        }
    }
}
void copyText(const std::vector<std::string>& lines) {
    if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
        clipboardText.clear();
        int charCount = 0;
        for (const auto& line : lines) {
            int lineStart = (charCount >= selectionStart) ? 0 : selectionStart - charCount;
            int lineEnd = (charCount + line.size() >= selectionEnd) ? selectionEnd - charCount : line.size();
            if (lineStart < lineEnd) {
                clipboardText += line.substr(lineStart, lineEnd - lineStart);
                clipboardText += '\n'; // Add newline after each line
            }
            charCount += line.size() + 1; // +1 for newline
            if (charCount >= selectionEnd) {
                break; // Stop copying after reaching selectionEnd
            }
        }
        SDL_SetClipboardText(clipboardText.c_str());
    }
}
void pasteText(std::vector<std::string>& lines, int& cursorPos, int& currentLine) {
    char* text = SDL_GetClipboardText();
    if (text) {
        undoStack.push_back({lines, cursorPos, currentLine}); // Save current state for undo
        std::stringstream ss(text);
        std::string line;
        bool firstLine = true;
        while (std::getline(ss, line, '\n')) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (firstLine) {
                lines[currentLine].insert(cursorPos, line);
                cursorPos += line.size();
                firstLine = false;
            } else {
                currentLine++;
                lines.insert(lines.begin() + currentLine, line);
                cursorPos = line.size();
            }
        }
        SDL_free(text);
    }
}
void cutText(std::vector<std::string>& lines, int& cursorPos, int currentLine) {
    if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
        undoStack.push_back({lines, cursorPos, currentLine}); // Save current state for undo
        copyText(lines);
        int charCount = 0;
        for (auto& line : lines) {
            int lineStart = (charCount >= selectionStart) ? 0 : selectionStart - charCount;
            int lineEnd = (charCount + line.size() >= selectionEnd) ? selectionEnd - charCount : line.size();
            if (lineStart < lineEnd) {
                line.erase(lineStart, lineEnd - lineStart);
            }
            charCount += line.size() + 1;
            if (charCount >= selectionEnd) {
                break;
            }
        }
        cursorPos = selectionStart;
    }
}
void selectAll(int& selectionStart, int& selectionEnd, const std::vector<std::string>& lines) {
    if (!lines.empty()) {
        selectionStart = 0;
        selectionEnd = 0;
        for (const auto& line : lines) {
            selectionEnd += line.size() + 1;
        }
        selectionEnd--;
    }
}
void ensureLastLineVisible(int currentLine, int &scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines);
void renderTextWithSyntax(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y);
std::vector<EditorState> redoStack;
void undo(std::vector<std::string>& lines, int& cursorPos, int& currentLine, SDL_Renderer* renderer, TTF_Font* font) {
    if (!undoStack.empty()) {
        EditorState lastState = undoStack.back();
        redoStack.push_back({lines, cursorPos, currentLine}); // Save for redo

        lines = lastState.lines;
        cursorPos = lastState.cursorPos;
        currentLine = lastState.currentLine;
        undoStack.pop_back();

    }
    else {
        // If no previous state, delete the last character or word
        if (cursorPos > 0) {
            // Delete last word (if space is before) or last character
            size_t pos = lines[currentLine].find_last_of(" ", cursorPos - 1);
            if (pos != std::string::npos && pos < cursorPos - 1) {
                lines[currentLine].erase(pos + 1, cursorPos - pos - 1);
                cursorPos = pos + 1;
            } else {
                lines[currentLine].erase(cursorPos - 1, 1);
                cursorPos--;
            }
        } else if (currentLine > 0) {
            // If at the start of a line, merge it with the previous line
            cursorPos = lines[currentLine - 1].size();
            lines[currentLine - 1] += lines[currentLine];
            lines.erase(lines.begin() + currentLine);
            currentLine--;
        }
    }
    ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, (int)lines.size());
    SDL_SetRenderDrawColor(renderer, darkMode ? 40 : 255, darkMode ? 44 : 255, darkMode ? 52 : 255, 255);
    SDL_RenderClear(renderer);
    int y = -scrollOffset + MENU_BAR_HEIGHT + 10;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (y + LINE_HEIGHT > 0 && y < SCREEN_HEIGHT) {
            renderTextWithSyntax(renderer, font, lines[i], 10, y);
        }
        y += LINE_HEIGHT;
    }
    if (cursorVisible) {
        int cursorX = 10;
        if (cursorPos > 0) {
            std::string sub = lines[currentLine].substr(0, cursorPos);
            TTF_SizeText(font, sub.c_str(), &cursorX, nullptr);
            cursorX += 10;
        }
        SDL_SetRenderDrawColor(renderer, darkMode ? 255 : 0, darkMode ? 255 : 0, darkMode ? 255 : 0, 255);
        SDL_RenderDrawLine(renderer, cursorX, y, cursorX, y + LINE_HEIGHT);
    }
    SDL_RenderPresent(renderer);
}
void redo(std::vector<std::string>& lines, int& cursorPos, int& currentLine, SDL_Renderer* renderer, TTF_Font* font) {
    if (!redoStack.empty()) {
        EditorState lastState = redoStack.back();
        undoStack.push_back({lines, cursorPos, currentLine}); // Save for undo
        lines = lastState.lines;
        cursorPos = lastState.cursorPos;
        currentLine = lastState.currentLine;
        redoStack.pop_back();
    }
    SDL_SetRenderDrawColor(renderer, darkMode ? 40 : 255, darkMode ? 44 : 255, darkMode ? 52 : 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}
void goToLine(int& currentLine, int totalLines) {
    int lineNumber;
    std::cout << "Enter line number to go to: ";
    std::cin >> lineNumber;

    if (lineNumber >= 1 && lineNumber <= totalLines) {
        currentLine = lineNumber - 1;
        std::cout << "Moved to line " << lineNumber << std::endl;
    } else {
        std::cerr << "Invalid line number." << std::endl;
    }
}
std::string getProjectName(SDL_Renderer* renderer, TTF_Font* font) {
    std::string projectName;
    SDL_Event e;
    bool done = false;

    SDL_Rect inputBox = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 25, 300, 50};
    SDL_Color boxColor = {70, 70, 70, 255};
    SDL_Color textColor = {255, 255, 255, 255};

    SDL_StartTextInput();  // Enable text input

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {  // Allow cancel with ESC
                    projectName.clear();
                    done = true;
                } else if (e.key.keysym.sym == SDLK_BACKSPACE && !projectName.empty()) {
                    projectName.pop_back();
                }
            } else if (e.type == SDL_TEXTINPUT) {
                projectName += e.text.text;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderClear(renderer);

        // Draw input box
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(renderer, &inputBox);

        // Render current text
        if (!projectName.empty()) {
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, projectName.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = {inputBox.x + 10, inputBox.y + 10, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();  // Disable text input
    return projectName;
}
bool isCommentLine(const std::string& line, size_t& commentPos) {
    commentPos = line.find("//");
    return commentPos != std::string::npos;
}
void renderSimpleText(SDL_Renderer* renderer, TTF_Font* font,const std::string& text, SDL_Color color,int* x, int y)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {*x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    *x += surface->w;
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
void renderMenuBar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect menuBar = {0, 0, SCREEN_WIDTH, MENU_BAR_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    SDL_RenderFillRect(renderer, &menuBar);

    SDL_Color textColor = {255, 255, 255, 255};
    int x = 10;  // Starting x-coordinate

    renderSimpleText(renderer, font, "File", textColor, &x, 5);
    int fileButtonX = x - 40;
    x += 40;  // Add spacing between items
    renderSimpleText(renderer, font, "Edit", textColor, &x, 5);
    int editButtonX = x - 40;
    x += 40;  // Add spacing between items
    renderSimpleText(renderer, font, "View", textColor, &x, 5);
    x += 40;  // Add spacing between items
    int viewButtonX = x - 90;
    renderSimpleText(renderer, font, "Debug & Compile", textColor, &x, 5);
    x += 40;  // Add spacing between items
    renderSimpleText(renderer, font, "Run", textColor, &x, 5);

    if (showFileMenu) {
        SDL_Rect fileMenu = {fileButtonX, MENU_BAR_HEIGHT, 150, 90};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &fileMenu);

        int menuX = fileButtonX + 10;
        int menuY = MENU_BAR_HEIGHT + 5;
        renderSimpleText(renderer, font, "Project New", textColor, &menuX, menuY);
        menuY += 25;
        menuX = fileButtonX + 10;
        renderSimpleText(renderer, font, "Project Save", textColor, &menuX, menuY);
        menuY += 25;
        menuX = fileButtonX + 10;
        renderSimpleText(renderer, font, "Exit", textColor, &menuX, menuY);
    }

    if (showEditMenu) {
        SDL_Rect editMenu = {editButtonX, MENU_BAR_HEIGHT, 150, 60};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &editMenu);
        int menuX = editButtonX + 10;
        int menuY = MENU_BAR_HEIGHT + 5;
        renderSimpleText(renderer, font, "Undo", textColor, &menuX, menuY);
        menuY += 25;
        menuX = editButtonX + 10;
        renderSimpleText(renderer, font, "Redo", textColor, &menuX, menuY);
    }
    if (showViewMenu) {
        SDL_Rect viewMenu = {viewButtonX, MENU_BAR_HEIGHT, 150, 60};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &viewMenu);

        int menuX = viewButtonX + 10;
        int menuY = MENU_BAR_HEIGHT + 5;
        renderSimpleText(renderer, font, "Dark Mode", textColor, &menuX, menuY);
        menuY += 25;
        menuX = viewButtonX + 10;
        renderSimpleText(renderer, font, "Light Mode", textColor, &menuX, menuY);
    }
}
void renderTextWithSyntax(SDL_Renderer* renderer, TTF_Font* font,const std::string& text, int x, int y)
{
    int currentX = x;
    std::string remainingText = text;
    SDL_Color defaultColor = darkMode ? SDL_Color{255, 255, 255, 255}
                                      : SDL_Color{  0,   0,   0, 255};

    size_t commentPos;
    if (isCommentLine(text, commentPos)) {
        // اگر // در خط وجود دارد، قسمت قبل از // را با قوانین سینتکس عادی نمایش بده
        std::string beforeComment = text.substr(0, commentPos);
        std::string comment = text.substr(commentPos);

        // قسمت قبل از //
        while (!beforeComment.empty()) {
            bool matched = false;
            for (const auto& rule : getCurrentRules()) {
                size_t pos = beforeComment.find(rule.keyword);
                if (pos == 0) {
                    renderSimpleText(renderer, font, rule.keyword, rule.color, &currentX, y);
                    beforeComment = beforeComment.substr(rule.keyword.length());
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                renderSimpleText(renderer, font, beforeComment.substr(0, 1),
                                 defaultColor, &currentX, y);
                beforeComment = beforeComment.substr(1);
            }
        }

        // قسمت بعد از // (کامنت)
        SDL_Color commentColor = {128, 128, 128, 255}; // رنگ طوسی برای کامنت
        renderSimpleText(renderer, font, comment, commentColor, &currentX, y);
    } else {
        // اگر // در خط وجود ندارد، از قوانین سینتکس عادی استفاده کن
        while (!remainingText.empty()) {
            bool matched = false;
            for (const auto& rule : getCurrentRules()) {
                size_t pos = remainingText.find(rule.keyword);
                if (pos == 0) {
                    renderSimpleText(renderer, font, rule.keyword, rule.color, &currentX, y);
                    remainingText = remainingText.substr(rule.keyword.length());
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                renderSimpleText(renderer, font, remainingText.substr(0, 1),
                                 defaultColor, &currentX, y);
                remainingText = remainingText.substr(1);
            }
        }
    }
}
const std::vector<std::string> wordSuggestions = {"int", "float", "double"};
std::string suggestWord(const std::string& prefix) {
    for (const auto& word : wordSuggestions) {
        if (word.find(prefix) == 0) { // if prefix matches the start of the word
            return word;
        }
    }
    return "";  // Return empty string if no match
}
void handleTextInput(SDL_Event& e, std::vector<std::string>& lines, int& currentLine, int& cursorPos, std::string& typedPrefix) {
    if (e.text.text) {
        char inputChar = e.text.text[0];
        typedPrefix += inputChar;  // Update the typed prefix

        // Check for word suggestion based on the typed prefix
        std::string suggestion = suggestWord(typedPrefix);

        if (!suggestion.empty()) {
            // Replace the current prefix with the full suggestion (like 'int', 'float', etc.)
            lines[currentLine].replace(cursorPos - typedPrefix.size(), typedPrefix.size(), suggestion);
            cursorPos = lines[currentLine].size(); // Move the cursor to the end of the inserted word
        } else {
            // Insert the character as normal
            lines[currentLine].insert(cursorPos, e.text.text);
            cursorPos += strlen(e.text.text);
        }

        // Ensure the last line is visible (handle scrolling if needed)
        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
    }
}
void ensureLastLineVisible(int currentLine, int& scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines) {
    if (currentLine * LINE_HEIGHT - scrollOffset > SCREEN_HEIGHT - LINE_HEIGHT) {
        scrollOffset = (currentLine + 1) * LINE_HEIGHT - SCREEN_HEIGHT;
    }
}
int getCursorPosFromMouse(const std::string& line, int mouseRelativeX,TTF_Font* font)
{
    if (mouseRelativeX <= 0) {
        return 0;
    }
    int length = (int)line.size();
    for (int i = 1; i <= length; i++) {
        std::string sub = line.substr(0, i);
        int w = 0;
        TTF_SizeText(font, sub.c_str(), &w, nullptr);
        if (w > mouseRelativeX) {
            return i - 1;
        }
    }
    return length;
}
void detectLibraries(const std::vector<std::string>& lines) {
    for (const auto& line : lines) {
        for (auto& lib : libraries) {
            if (line.find("#include " + lib.first) != std::string::npos) {
                lib.second = true; // فعال‌کردن کتابخانه
            }
        }
    }
}
void updateSyntaxRulesBasedOnLibraries(std::vector<SyntaxRule>& rules) {
    if (libraries["<cmath>"]) {
        rules.push_back({"sqrt", {0, 128, 128, 255}}); // رنگ برای توابع ریاضی
        rules.push_back({"pow", {0, 128, 128, 255}});
        rules.push_back({"sin", {0, 128, 128, 255}});
        rules.push_back({"cos", {0, 128, 128, 255}});
        rules.push_back({"tan", {0, 128, 128, 255}});
    }

    if (libraries["<iostream>"]) {
        rules.push_back({"std::cout", {0, 51, 102, 255}}); // رنگ برای توابع ورودی/خروجی
        rules.push_back({"std::cin", {0, 51, 102, 255}});
        rules.push_back({"std::endl", {0, 51, 102, 255}});
    }

    if (libraries["<algorithm>"]) {
        rules.push_back({"std::sort", {0, 51, 102, 255}}); // رنگ برای توابع الگوریتم
        rules.push_back({"std::find", {0, 51, 102, 255}});
    }
}
struct Project {
    std::string name;
    std::vector<std::string> content;
};
std::vector<Project> projects;
bool showProjectList = false;
void saveProject(SDL_Renderer* renderer, TTF_Font* font, const std::vector<std::string>& lines) {
    SDL_StopTextInput();  // Ensure clean state before getting project name
    std::string projectName = getProjectName(renderer, font);

    if (projectName.empty()) {
        std::cerr << "Invalid project name. Project not saved." << std::endl;
        SDL_StartTextInput();  // Re-enable text input after cancellation
        return;
    }

    std::ofstream outFile(projectName + ".cpp");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for saving." << std::endl;
        SDL_StartTextInput();  // Re-enable text input after error
        return;
    }

    for (const auto& line : lines) {
        outFile << line << "\n";
    }
    outFile.close();

    savedProjects.push_back(projectName); // Add project to saved list

    std::cout << "Project saved successfully as " << projectName << ".cpp" << std::endl;
    SDL_StartTextInput();  // Re-enable text input after successful save
}
void handleUndoButton(std::vector<std::string>& lines, int& cursorPos, int& currentLine, SDL_Renderer* renderer, TTF_Font* font) {
    undo(lines, cursorPos, currentLine, renderer, font);
}
void renderProjectList(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect menuBar = {SCREEN_WIDTH - 200, -5, 200, SCREEN_HEIGHT};
    SDL_Color backgroundColor = {30, 30, 30, 255};  // Darker background
    SDL_Color borderColor = {255, 255, 255, 255};   // White border
    SDL_Color textColor = {240, 255, 240, 255};

    // Draw background
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &menuBar);

    // Draw border
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &menuBar);

    int y = 20;

    if (savedProjects.empty()) {
        // Show "nothing" if no projects are saved
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Nothing to Show",{255, 0, 34, 255});
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {SCREEN_WIDTH - 190, y, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    } else {
        // Show saved project names
        for (const auto& project : savedProjects) {
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, project.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = {SCREEN_WIDTH - 190, y, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                y += textRect.h + 10; // Spacing between project names
            }
        }
    }
}
int sssbu=  SCREEN_WIDTH-260;
void renderSaveButton(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect button = {sssbu, 0, 60, 30};
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    SDL_RenderFillRect(renderer, &button);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, "Save", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {sssbu+5, 5, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
bool compile(const std::string &filename)
{
    std::string compileCommand = "g++ " + filename + " -o project"; // Adjust as needed
    int result = std::system(compileCommand.c_str());
    return (result == 0);
}
void run(const std::string &filename)
{
    std::string runCommand = ".\\" + filename + ".exe ";
    std::system(runCommand.c_str());
}
void run_in_another_window(const std::string &filename)
{
#ifdef _WIN32 // Windows
    std::string runCommand = "start cmd /c \"" + filename + " & pause\"";
#elif __linux__ // Linux
    std::string runCommand = "x-terminal-emulator -e \"" + filename + "; read -n 1 -s -r -p 'Press any key to continue...'\"";
#elif __APPLE__ // macOS
    std::string runCommand = "osascript -e 'tell application \"Terminal\" to do script \"" + filename + "\"'";
#else
    std::cerr << "Unsupported operating system." << std::endl;
    return;
#endif

    std::system(runCommand.c_str());
}
void run_in_cmd_window() {
    std::string runCommand = "start cmd /k \" echo hello Avin and Parisa && pause\"";
    int result = std::system(runCommand.c_str());
    if (result != 0) {
        std::cerr << "Error running the executable." << std::endl;
    }
}
void debug(const std::string &filename)
{
    std::cout << "Debugging " << filename << " (simulated):\n";
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        int lineNumber = 1;
        while (std::getline(file, line))
        {
            std::cout << lineNumber << ": " << line << std::endl;
            lineNumber++;
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file for debugging." << std::endl;
    }
}
SDL_Rect compileButton = { 350,0 ,50 ,50  };
SDL_Rect runButton = { 480, 0, 40, 50 };
SDL_Rect debugButton = { 290, 0, 40, 50 };
// maaaaaaaaain
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: "
                  << SDL_GetError() << std::endl;
        return -1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF could not initialize! TTF_Error: "
                  << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("SDL Text Editor",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    TTF_Font* font = TTF_OpenFont(R"(C:\Windows\Fonts\bahnschrift.ttf)", FONT_SIZE);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: "
                  << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    std::vector<std::string> lines = {""};
    int currentLine = 2;
    int cursorPos   = 0;
    int scrollOffset= 0;
    const int LINE_HEIGHT = TTF_FontHeight(font);
    Uint32 lastCursorToggle = SDL_GetTicks();
    bool cursorVisible = true;
    const Uint32 CURSOR_BLINK_INTERVAL = 500;
    SDL_Event e;
    bool quit = false;
    SDL_ShowCursor(SDL_ENABLE);
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastCursorToggle + CURSOR_BLINK_INTERVAL) {
            cursorVisible = !cursorVisible;
            lastCursorToggle = currentTime;
        }
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y > 0) {
                    scrollOffset = std::max(0, scrollOffset - LINE_HEIGHT);
                } else if (e.wheel.y < 0) {
                    scrollOffset += LINE_HEIGHT;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int x = e.button.x;
                    int y = e.button.y;
                    if (y <= MENU_BAR_HEIGHT) {  // Detect clicks within the top menu bar area
                        if (x >= 10 && x < 50) {  // File button clicked
                            showFileMenu = !showFileMenu;
                            showEditMenu = false;
                            showViewMenu = false;
                        } else if (x >= 90 && x < 130) {  // Edit button clicked
                            showEditMenu = !showEditMenu;
                            showFileMenu = false;
                            showViewMenu = false;
                        } else if (x >= 170 && x < 210) {  // View button clicked
                            showViewMenu = !showViewMenu;
                            showFileMenu = false;
                            showEditMenu = false;
                        }
                        else if (x > sssbu && x <= sssbu+60) {
                            if (y > 0 && y <= MENU_BAR_HEIGHT ) {
                                saveProject(renderer,font,lines);
                            }
                        }
                    }
                    else if (showFileMenu && y > MENU_BAR_HEIGHT && y <= MENU_BAR_HEIGHT + 90) {
                        if (y > MENU_BAR_HEIGHT + 30 && y <= MENU_BAR_HEIGHT + 60) { // Save button clicked
                            saveProject(renderer,font,lines);
                            showFileMenu = false;
                        }
                    }
                    else if (showViewMenu && y > MENU_BAR_HEIGHT && y <= MENU_BAR_HEIGHT + 60) {
                        if (y > MENU_BAR_HEIGHT && y <= MENU_BAR_HEIGHT + 30) {
                            darkMode = true;
                            showViewMenu = false;
                        } else if (y > MENU_BAR_HEIGHT + 30 && y <= MENU_BAR_HEIGHT + 60) {
                            darkMode = false;
                            showViewMenu = false;
                        }
                    }
                    else if (showEditMenu && y > MENU_BAR_HEIGHT && y <= MENU_BAR_HEIGHT + 60) {
                        if (y > MENU_BAR_HEIGHT && y <= MENU_BAR_HEIGHT + 30) {
                            // Undo button clicked
                            handleUndoButton(lines, cursorPos, currentLine, renderer, font);
                            showEditMenu = false;  // Close edit menu after clicking
                        }
                        else if (y > MENU_BAR_HEIGHT + 30 && y <= MENU_BAR_HEIGHT + 60) {
                            redo(lines, cursorPos, currentLine, renderer, font);
                            showEditMenu = false;
                        }

                    }

                    SDL_GetMouseState(&x, &y);
                    if (x >= runButton.x && x <= runButton.x + 50 &&
                        y >= runButton.y && y <= runButton.y + 50) {
                        std::cout << "Run button clicked!" << std::endl;
                        run_in_cmd_window();  // Run the program in a new CMD window
                    }
                    else if (mouseX >= compileButton.x && mouseX <= compileButton.x + compileButton.w &&
                        mouseY >= compileButton.y && mouseY <= compileButton.y + compileButton.h) {
                        // Compile the source code
                        std::string filename = "example.cpp"; // Replace with actual file logic
                        if (compile(filename)) {
                            std::cout << "Compilation successful!" << std::endl;
                        } else {
                            std::cout << "Compilation failed!" << std::endl;
                        }
                    }
                     else if (mouseX >= debugButton.x && mouseX <= debugButton.x + debugButton.w &&
                               mouseY >= debugButton.y && mouseY <= debugButton.y + debugButton.h) {
                        // Debug the source code
                        std::string filename = "example.cpp";
                        debug(filename);
                    }
                    if (x >= SCREEN_WIDTH - 200 && x <= SCREEN_WIDTH) {  // Click inside project list
                            int yOffset = 5;
                            for (size_t i = 0; i < savedProjects.size(); ++i) {
                                if (y >= yOffset && y <= yOffset + 35) {  // Check if click is within project name bounds
                                    std::string projectName = savedProjects[i];
                                    std::string filePath = projectName + ".cpp";
                                    std::ifstream inFile(filePath);
                                    if (inFile.is_open()) {
                                        lines.clear(); // Clear current editor content
                                        std::string line;
                                        while (std::getline(inFile, line)) {
                                            lines.push_back(line);
                                        }
                                        inFile.close();

                                        // Reset cursor position and scrolling
                                        currentLine = 0;
                                        cursorPos = 0;
                                        scrollOffset = 0;

                                        std::cout << "Loaded project: " << projectName << std::endl;
                                    } else {
                                        std::cerr << "Failed to open project file: " << filePath << std::endl;
                                    }

                                    break; // Stop checking after one project is selected
                                }
                                yOffset += 35; // Spacing between project names
                            }
                    }

                    else {
                        x = e.button.x;
                        y = e.button.y;
                        int clickedLine = (mouseY + scrollOffset) / LINE_HEIGHT;
                        if (clickedLine < 0) clickedLine = 0;
                        if (clickedLine >= (int)lines.size()) {
                            clickedLine = (int)lines.size() - 1;
                        }
                        if(clickedLine>=1)
                            currentLine = clickedLine-1;
                        else
                            currentLine = clickedLine;


                        int relativeX = mouseX - 10;
                        if (relativeX < 0) relativeX = 0;
                        cursorPos = getCursorPosFromMouse(lines[currentLine], relativeX, font);

                        if (cursorPos < 0) cursorPos = 0;
                        if (cursorPos > (int)lines[currentLine].size()) {
                            cursorPos = (int)lines[currentLine].size();
                        }
                    }
                }
            }
            if (e.type == SDL_KEYDOWN) {
                handleShortcuts(e, lines, cursorPos, currentLine,renderer,font);
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    if (cursorPos > 0 && cursorPos <= (int)lines[currentLine].size()) {
                        lines[currentLine].erase(cursorPos - 1, 1);
                        cursorPos--;
                    } else if (currentLine > 0) {
                        cursorPos = (int)lines[currentLine - 1].size();
                        lines[currentLine - 1] += lines[currentLine];
                        lines.erase(lines.begin() + currentLine);
                        currentLine--;
                    }

                    if (lines.empty()) {
                        lines.push_back("");
                        currentLine = 0;
                        cursorPos = 0;
                    }
                }
                else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (cursorPos <= (int)lines[currentLine].size()) {
                        std::string remainder = lines[currentLine].substr(cursorPos);
                        lines[currentLine] = lines[currentLine].substr(0, cursorPos);
                        lines.insert(lines.begin() + currentLine + 1, remainder);
                        currentLine++;
                        cursorPos = 0;
                        ensureLastLineVisible(currentLine, scrollOffset,SCREEN_HEIGHT, LINE_HEIGHT,(int)lines.size());
                    }
                }
                else if (e.key.keysym.sym == SDLK_TAB) {
                    lines[currentLine].insert(cursorPos, "    ");
                    cursorPos += 4;
                }
                else if (e.key.keysym.sym == SDLK_LEFT) {
                    if (cursorPos > 0) {
                        cursorPos--;
                    } else if (currentLine > 0) {
                        currentLine--;
                        cursorPos = (int)lines[currentLine].size();
                    }
                }
                else if (e.key.keysym.sym == SDLK_RIGHT) {
                    if (cursorPos < (int)lines[currentLine].size()) {
                        cursorPos++;
                    } else if (currentLine < (int)lines.size() - 1) {
                        currentLine++;
                        cursorPos = 0;
                    }
                }
                else if (e.key.keysym.sym == SDLK_UP) {
                    if (currentLine > 0) {
                        currentLine--;
                        cursorPos = std::min(cursorPos,
                                             (int)lines[currentLine].size());
                        ensureLastLineVisible(currentLine, scrollOffset,
                                              SCREEN_HEIGHT, LINE_HEIGHT,

                                              (int)lines.size());
                    }
                }
                else if (e.key.keysym.sym == SDLK_DOWN) {
                    if (currentLine < (int)lines.size() - 1) {
                        currentLine++;
                        cursorPos = std::min(cursorPos,
                                             (int)lines[currentLine].size());
                        ensureLastLineVisible(currentLine, scrollOffset,
                                              SCREEN_HEIGHT, LINE_HEIGHT,
                                              (int)lines.size());
                    }
                }
                else if (e.key.keysym.sym == SDLK_F1) {
                    darkMode = !darkMode;
                    SDL_SetRenderDrawColor(renderer,
                                           darkMode ? 40 : 255,
                                           darkMode ? 44 : 255,
                                           darkMode ? 52 : 255,
                                           255);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                }
            }
            else if (e.type == SDL_TEXTINPUT) {
                if (e.text.text) {
                    char openChar = e.text.text[0];
                    char closeChar = '\0';

                    // Determine the matching closing character
                    switch (openChar) {
                        case '{': closeChar = '}'; break;
                        case '(': closeChar = ')'; break;
                        case '[': closeChar = ']'; break;
                        case '<': closeChar = '>'; break;
                    }

                    // Insert both opening and closing characters if applicable
                    if (closeChar != '\0') {
                        lines[currentLine].insert(cursorPos, 1, openChar);
                        lines[currentLine].insert(cursorPos + 1, 1, closeChar);
                        cursorPos++; // Move cursor between the paired characters
                    } else {
                        lines[currentLine].insert(cursorPos, e.text.text);
                        cursorPos += (int)std::strlen(e.text.text);
                    }

                    ensureLastLineVisible(currentLine, scrollOffset,
                                          SCREEN_HEIGHT, LINE_HEIGHT,
                                          (int)lines.size());
                }
            }
        }
        detectLibraries(lines);
        updateSyntaxRulesBasedOnLibraries(getCurrentRules());
        SDL_SetRenderDrawColor(renderer,darkMode ? 40 : 255,darkMode ? 44 : 255,darkMode ? 52 : 255,255);
        SDL_RenderClear(renderer);
        int y = -scrollOffset+ MENU_BAR_HEIGHT + 10;;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (y + LINE_HEIGHT > 0 && y < SCREEN_HEIGHT) {
                renderTextWithSyntax(renderer, font, lines[i], 10, y);
                if (i == (size_t)currentLine && cursorVisible) {
                    int cursorX = 10;
                    if (cursorPos > 0) {
                        std::string sub = lines[i].substr(0, cursorPos);
                        TTF_SizeText(font, sub.c_str(), &cursorX, nullptr);
                        cursorX += 10;
                    }
                    if (darkMode) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    } else {
                        SDL_SetRenderDrawColor(renderer,   0,   0,   0, 255);
                    }
                    SDL_RenderDrawLine(renderer, cursorX, y, cursorX, y + LINE_HEIGHT);
                }
            }
            y += LINE_HEIGHT;
        }
        renderMenuBar(renderer, font);
        renderSaveButton(renderer, font);
        renderProjectList(renderer, font);
        if (showProjectList) {
            renderProjectList(renderer, font);
        }
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}