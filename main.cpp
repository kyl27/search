#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <regex>
#include <string>
#include <vector>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

using namespace std;
using namespace boost::filesystem;

struct Node {
    map<char, Node*> children;
    map<long, set<long> > occurrences;
    bool terminal;
};

long tokens, newTokens;
map<long, string> files;
Node *root;

string helpStr = "Commands: load <directory> | query <string> | help | quit";


bool isTokenDelimiter(char c) {
    return c < 'a' || c > 'z';
}

bool isCamlDelimiter(char c) {
    return c < 'A' || c > 'Z';
}

bool isWordDelimiter(char c) {
    return isTokenDelimiter(c) && isCamlDelimiter(c) && (c < '-' || c > ':') && c != '_';
}

void insertToken(Node *n, const string &token, long fileIndex, long line) {
    for (size_t i = 0; i < token.length(); ++i) {
        if (n->children.count(token[i]) == 0) {
            n->children[token[i]] = new Node();
        }
        n = n->children[token[i]];
    }
    if (n->occurrences[fileIndex].insert(line).second) {
        ++tokens;
    }
    ++newTokens;
    n->terminal = true;
}

void insertWord(const string &word, long fileIndex, long line) {
    for (size_t i = 0; i < word.length(); ++i) {
        if (isTokenDelimiter(word[i])) {
            if (isCamlDelimiter(word[i])) {
                insertToken(root, word.substr(i + 1), fileIndex, line);
            } else {
                insertToken(root, word.substr(i), fileIndex, line);
            }
        }
    }
    insertToken(root, word, fileIndex, line);
}

void printResults(Node *n) {
    for (auto it : n->occurrences) {
        string &filepath = files[it.first];
        std::ifstream file (filepath, ios::in);
        if (file.is_open()) {
            long lineNo = 1;
            string line;
            for (auto it_line : it.second) {
                while (lineNo != it_line) {
                    file.ignore(numeric_limits<streamsize>::max(), '\n');
                    ++lineNo;
                }
                getline(file, line);
                cout << filepath << ':' << it_line << ':' << line << endl;
                ++lineNo;
            }
            file.close();
        } else {
            cout << "Failed to open file: " << filepath << endl;
        }
    }
}

void printTerminals(Node *n, string &s) {
    if (n->terminal) {
        printResults(n);
    }
    for (auto it : n->children) {
        s += it.first;
        printTerminals(it.second, s);
        s.pop_back();
    }
}

bool query(Node *n, string &query) {
    for (size_t i = 0; i < query.length(); ++i) {
        if (n->children.count(query[i])) {
            n = n->children[query[i]];
        } else {
            cout << "No results for query: " << query << endl;
            return false;
        }
    }
    cout << "Results: " << endl;
    printTerminals(n, query);
    return true;
}

long countTokens(Node *n) {
    long count = n->terminal ? 1 : 0;
    for (auto it : n->children) {
        count += countTokens(it.second);
    }
    return count;
}

void indexFile(const string &filepath, long fileIndex) {
    char c;
    string s;
    long line = 1;
    std::ifstream file (filepath, ios::in);
    if (file.is_open()) {
        files.emplace(fileIndex, filepath);
        while (file.get(c) && c > 0 && c < '~') {
            if (isWordDelimiter(c) && s.length()) {
                insertWord(s, fileIndex, line);
                s.clear();
            } else if (c > ' ') {
                s += c;
            }
            if (c == '\n') {
                ++line;
            }
        }
        file.close();
    } else {
        cout << "Failed to open file: " << s << endl;
    }
}

long iterateFiles(const string &path) {
    long numFiles = 0;
    cout << "Indexing root directory: " << path << endl;
    recursive_directory_iterator it(path), end;
    while (it != end)
    {
        if (it->path().stem().string().length() == 0 || is_symlink(*it)) {
            it.no_push();
        } else if (is_directory(*it)) {
            cout << "Indexing subdirectory: " << it->path().string() << endl;
        } else {
            indexFile(it->path().string(), numFiles);
            ++numFiles;
        }
        ++it;
    }
    return numFiles;
}

int main(int argc, char *argv[]) {
    root = new Node();

    string s;

    while (true) {
        cout << "Input command or \"help\":" << endl;
        cin >> s;

        if (s == "quit") {
            break;
        } else if (s == "help") {
            cout << helpStr << endl;
        } else if (s == "load" && cin >> s && is_directory(s)) {
            newTokens = 0;
            clock_t start = clock();
            long numFiles = iterateFiles(s);
            start = clock() - start;

            cout << newTokens << " tokens added from "
                 << numFiles << " files in "
                 << (double) start / CLOCKS_PER_SEC << " seconds " << endl;
            cout << "Total unique tokens: " << tokens << endl;
        } else if (s == "query" && cin >> s) {
            query(root, s);
        } else {
            cout << "Invalid command or directory does not exist." << endl;
        }
    }
    return 0;
}
