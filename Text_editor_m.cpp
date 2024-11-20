#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <conio.h> 
#include <windows.h> 
#include <cctype>
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <climits>    


struct letter {
    char ch;
    letter* next;
    // Add whatever you like
};

class Line {
public:
    letter* head;

    Line() : head(nullptr) {}

    void insertChar(int position, char ch) {
        letter* newLetter = new letter{ ch, nullptr };
        if (!head) {
            head = newLetter;
            return;
        }
        if (position == 0) {
            newLetter->next = head;
            head = newLetter;
            return;
        }
        letter* current = head;
        int index = 0;
        while (current->next && index < position - 1) {
            current = current->next;
            index++;
        }
        newLetter->next = current->next;
        current->next = newLetter;
    }

    void deleteChar(int position) {
        if (!head) return;
        if (position == 0) {
            letter* temp = head;
            head = head->next;
            delete temp;
            return;
        }
        letter* current = head;
        int index = 0;
        while (current->next && index < position - 1) {
            current = current->next;
            index++;
        }
        if (current->next) {
            letter* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }

    std::string getString() {
        std::string result;
        letter* current = head;
        while (current) {
            result += current->ch;
            current = current->next;
        }
        return result;
    }

    void clear() {
        letter* current = head;
        while (current) {
            letter* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }

    void toUpper(int start, int end) {
        letter* current = head;
        int index = 0;
        while (current && index <= end) {
            if (index >= start) {
                current->ch = toupper(current->ch);
            }
            current = current->next;
            index++;
        }
    }

    void toLower(int start, int end) {
        letter* current = head;
        int index = 0;
        while (current && index <= end) {
            if (index >= start) {
                current->ch = tolower(current->ch);
            }
            current = current->next;
            index++;
        }
    }



    int length() {
        int len = 0;
        letter* current = head;
        while (current) {
            len++;
            current = current->next;
        }
        return len;
    }

    std::vector<std::string> getWords() {
        std::vector<std::string> words;
        std::string word = getString();
        std::stringstream ss(word);
        while (ss >> word) {
            words.push_back(word);
        }
        return words;
    }
};

class Document {
public:
    std::vector<Line> lines;

    Document() {
        // Start with an empty line
        lines.push_back(Line());
    }

    void insertChar(int row, int col, char ch) {
        if (row >= lines.size()) {
            lines.resize(row + 1);
        }
        lines[row].insertChar(col, ch);
    }

    void deleteChar(int row, int col) {
        if (row < lines.size()) {
            lines[row].deleteChar(col);
        }
    }

    void saveToFile(const std::string& filename) {
        std::ofstream ofs(filename);
        if (!ofs) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }
        for (auto& line : lines) {
            ofs << line.getString() << std::endl;
        }
        ofs.close();
    }

    void toUpper(int row, int start, int end) {
        if (row < lines.size()) {
            lines[row].toUpper(start, end);
        }
    }

    void toLower(int row, int start, int end) {
        if (row < lines.size()) {
            lines[row].toLower(start, end);
        }
    }

    // Convert current word to uppercase
    void toUpperCurrentWord(int row, int cursorX) {
        if (row >= lines.size()) return;
        std::string lineStr = lines[row].getString();
        int start = cursorX;
        int end = cursorX;

        while (start > 0 && !isspace(lineStr[start - 1])) start--;
        while (end < lineStr.length() && !isspace(lineStr[end])) end++;

        lines[row].toUpper(start, end - 1);
    }

    void toLowerCurrentWord(int row, int cursorX) {
        if (row >= lines.size()) return;
        std::string lineStr = lines[row].getString();
        int start = cursorX;
        int end = cursorX;

        while (start > 0 && !isspace(lineStr[start - 1])) start--;
        while (end < lineStr.length() && !isspace(lineStr[end])) end++;

        lines[row].toLower(start, end - 1);
    }


    // Find word
    std::vector<std::pair<int, int>> findWord(const std::string& word, bool caseSensitive = true) {
        std::vector<std::pair<int, int>> occurrences;
        for (int i = 0; i < lines.size(); ++i) {
            std::string line = lines[i].getString();
            std::string searchLine = caseSensitive ? line : toLowerString(line);
            std::string searchWord = caseSensitive ? word : toLowerString(word);
            size_t pos = searchLine.find(searchWord);
            while (pos != std::string::npos) {
                occurrences.emplace_back(i, static_cast<int>(pos));
                pos = searchLine.find(searchWord, pos + word.length());
            }
        }
        return occurrences;
    }

    // Replace word
    void replaceWord(const std::string& oldWord, const std::string& newWord, bool replaceFirstOnly = false) {
        for (auto& line : lines) {
            std::string lineStr = line.getString();
            size_t pos = lineStr.find(oldWord);
            if (pos != std::string::npos) {
                for (size_t i = 0; i < oldWord.length(); ++i) {
                    line.deleteChar(pos);
                }
                for (size_t i = 0; i < newWord.length(); ++i) {
                    line.insertChar(pos + i, newWord[i]);
                }
                if (replaceFirstOnly) break;
            }
        }
    }


    // Add prefix to word
    void addPrefixToWord(const std::string& targetWord, const std::string& prefix) {
        replaceWord(targetWord, prefix + targetWord, false);
    }

    // Add postfix to word
    void addPostfixToWord(const std::string& targetWord, const std::string& postfix) {
        replaceWord(targetWord, targetWord + postfix, false);
    }

    // Average word length
    double averageWordLength() {
        int totalLength = 0;
        int wordCount = 0;
        for (auto& line : lines) {
            auto words = line.getWords();
            wordCount += words.size();
            for (auto& word : words) {
                totalLength += word.length();
            }
        }
        return wordCount ? static_cast<double>(totalLength) / wordCount : 0.0;
    }

    // Count substrings
    int substringCount(const std::string& substring) {
        int count = 0;
        for (auto& line : lines) {
            std::string lineStr = line.getString();
            size_t pos = lineStr.find(substring);
            while (pos != std::string::npos) {
                count++;
                pos = lineStr.find(substring, pos + substring.length());
            }
        }
        return count;
    }

    // Count special characters
    int specialCharCount() {
        int count = 0;
        for (auto& line : lines) {
            std::string lineStr = line.getString();
            for (char ch : lineStr) {
                if (!isalnum(ch) && !isspace(ch))
                    count++;
            }
        }
        return count;
    }

    // Count sentences
    int sentenceCount() {
        int count = 0;
        for (auto& line : lines) {
            std::string lineStr = line.getString();
            for (char ch : lineStr) {
                if (ch == '.' || ch == '!' || ch == '?')
                    count++;
            }
        }
        return count;
    }

    // Count paragraphs
    int paragraphCount() {
        int count = 0;
        bool newPara = true;
        for (auto& line : lines) {
            std::string lineStr = line.getString();
            if (lineStr.empty()) {
                newPara = true;
            }
            else {
                if (newPara) {
                    count++;
                    newPara = false;
                }
            }
        }
        return count;
    }

    // Largest word length
    int largestWordLength() {
        int maxLen = 0;
        for (auto& line : lines) {
            auto words = line.getWords();
            for (auto& word : words) {
                if (word.length() > maxLen)
                    maxLen = word.length();
            }
        }
        return maxLen;
    }

    // Smallest word length
    int smallestWordLength() {
        int minLen = INT32_MAX;
        for (auto& line : lines) {
            auto words = line.getWords();
            for (auto& word : words) {
                if (word.length() < minLen)
                    minLen = word.length();
            }
        }
        return minLen == INT32_MAX ? 0 : minLen;
    }

    // Word-Game
    std::string wordGame() {
        std::unordered_set<std::string> wordSet;
        for (auto& line : lines) {
            auto words = line.getWords();
            for (auto& word : words) {
                wordSet.insert(word);
            }
        }

        std::string bestWord;
        int maxCount = 0;

        for (const auto& word : wordSet) {
            int count = 0;
            for (const auto& subWord : wordSet) {
                if (subWord.length() <= word.length()) {
                    if (std::all_of(subWord.begin(), subWord.end(), [&](char c) {
                        return std::count(word.begin(), word.end(), c) >= std::count(subWord.begin(), subWord.end(), c);
                        })) {
                        count++;
                    }
                }
            }
            if (count > maxCount) {
                maxCount = count;
                bestWord = word;
            }
        }
        return bestWord;
    }

    // Helper to convert string to lowercase
    std::string toLowerString(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    void clear() {
        for (auto& line : lines) {
            line.clear();
        }
        lines.clear();
    }
};

// Replace deleteChar with range deletion
void replaceRange(Line& line, int start, int end, const std::string& replacement) {
    for (int i = start; i < end; ++i) {
        line.deleteChar(start);
    }
    for (size_t i = 0; i < replacement.size(); ++i) {
        line.insertChar(start + i, replacement[i]);
    }
}

void gotoxy(int x, int y) {
    COORD coord{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearScreen() {
    system("cls");
}

void displayDocument(Document& doc) {
    clearScreen();
    for (int i = 0; i < doc.lines.size(); ++i) {
        gotoxy(0, i);
        std::cout << doc.lines[i].getString();
    }
}

int main() {
    Document doc;
    int cursorX = 0;
    int cursorY = 0;
    bool running = true;

    displayDocument(doc);

    while (running) {
        gotoxy(cursorX, cursorY);
        int ch = _getch();

        if (ch == 224) { // Arrow keys
            ch = _getch();
            switch (ch) {
            case 72: // Up arrow
                if (cursorY > 0) cursorY--;
                break;
            case 80: // Down arrow
                if (cursorY < doc.lines.size() - 1) cursorY++;
                else {
                    doc.lines.push_back(Line());
                    cursorY++;
                }
                break;
            case 75: // Left arrow
                if (cursorX > 0) cursorX--;
                break;
            case 77: // Right arrow
                if (cursorX < doc.lines[cursorY].length()) cursorX++;
                break;
            }
        }
        else if (ch == 0) { // Function keys
            ch = _getch();
            switch (ch) {
            case 59: // F1: ToUpper
                doc.toUpperCurrentWord(cursorY, cursorX);
                displayDocument(doc);
                break;
            case 60: // F2: ToLower
                doc.toLowerCurrentWord(cursorY, cursorX);
                displayDocument(doc);
                break;
            case 61: // F3: Find
            {
                std::string wordToFind;
                gotoxy(0, doc.lines.size() + 1); // Move cursor to bottom
                std::cout << "Enter word to find: ";
                std::cin >> wordToFind;
                auto occurrences = doc.findWord(wordToFind);
                if (occurrences.empty()) {
                    std::cout << "\nWord not found.\n";
                }
                else {
                    cursorY = occurrences[0].first;
                    cursorX = occurrences[0].second;
                    std::cout << "\nWord found at line " << cursorY + 1 << ", position " << cursorX + 1 << "\n";
                }
                system("pause");
                displayDocument(doc);
            }
            break;
            case 62: // F4: Replace
            {
                std::string oldWord, newWord;
                gotoxy(0, doc.lines.size() + 1); // Move cursor to bottom
                std::cout << "Enter word to replace: ";
                std::cin >> oldWord;
                std::cout << "Enter new word: ";
                std::cin >> newWord;
                doc.replaceWord(oldWord, newWord);
                std::cout << "\nReplacement done.\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 63: // F5: Add Prefix to Word
            {
                std::string targetWord, prefix;
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Enter word to add prefix to: ";
                std::cin >> targetWord;
                std::cout << "Enter prefix: ";
                std::cin >> prefix;
                doc.addPrefixToWord(targetWord, prefix);
                std::cout << "\nPrefix added.\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 64: // F6: Add Postfix to Word
            {
                std::string targetWord, postfix;
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Enter word to add postfix to: ";
                std::cin >> targetWord;
                std::cout << "Enter postfix: ";
                std::cin >> postfix;
                doc.addPostfixToWord(targetWord, postfix);
                std::cout << "\nPostfix added.\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 65: // F7: Display Average Word Length
            {
                double avgLength = doc.averageWordLength();
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Average word length: " << avgLength << "\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 66: // F8: Count Substrings
            {
                std::string substring;
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Enter substring to count: ";
                std::cin >> substring;
                int count = doc.substringCount(substring);
                std::cout << "Substring '" << substring << "' occurs " << count << " times.\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 67: // F9: Count Special Characters
            {
                int count = doc.specialCharCount();
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Total special characters: " << count << "\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 68: // F10: Count Sentences
            {
                int count = doc.sentenceCount();
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Total sentences: " << count << "\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 133: // F11: Count Paragraphs
            {
                int count = doc.paragraphCount();
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Total paragraphs: " << count << "\n";
                system("pause");
                displayDocument(doc);
            }
            break;
            case 134: // F12: Word Game
            {
                std::string bestWord = doc.wordGame();
                gotoxy(0, doc.lines.size() + 1);
                std::cout << "Best word for word game: " << bestWord << "\n";
                system("pause");
                displayDocument(doc);
            }
            break;

            default:

                break;
            }
        }
        else if (ch == 19) { // Ctrl+S to save
            doc.saveToFile("document.txt");
            std::cout << "\nDocument saved to document.txt\n";
            system("pause");
            displayDocument(doc);
        }
        else if (ch == 27) { // Escape key to exit
            running = false;
        }
        else if (ch == '\r') { // Enter key
            cursorX = 0;
            cursorY++;
            if (cursorY >= doc.lines.size()) {
                doc.lines.push_back(Line());
            }
        }
        else if (ch == '\b') { // Backspace key
            if (cursorX > 0) {
                cursorX--;
                doc.deleteChar(cursorY, cursorX);
                displayDocument(doc);
            }
            else if (cursorY > 0) {
                cursorY--;
                cursorX = doc.lines[cursorY].length();
                // Optionally merge lines here
            }
        }
        else if (isprint(ch)) {
            doc.insertChar(cursorY, cursorX, ch);
            cursorX++;
            std::cout << static_cast<char>(ch);
        }
    }

    doc.clear();
    return 0;
}

