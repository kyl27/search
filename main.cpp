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
    map<int, set<int> > occurrences;
    bool terminal;
};

map<int, string> files;
Node *root;


bool isTokenDelimiter(char c) {
    return c < 'a' || c > 'z';
}

bool isCamlDelimiter(char c) {
    return c < 'A' || c > 'Z';
}

bool isWordDelimiter(char c) {
    return isTokenDelimiter(c) && isCamlDelimiter(c) && (c < '-' || c > ':') && c != '_';
}

void insertToken(Node *n, const string &token, int fileIndex, int line) {
    for (size_t i = 0; i < token.length(); ++i) {
        if (n->children.find(token[i]) == n->children.end()) {
            n->children.emplace(token[i], new Node());
        }
        n = n->children[token[i]];
    }
    n->occurrences[fileIndex].insert(line);
    n->terminal = true;
}

void insertWord(const string &word, int fileIndex, int line) {
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

void printTerminals(Node *n, string &s) {
    if (n->terminal) {
        for (auto it : n->occurrences) {
            string &file = files[it.first];
            for (auto it_line : it.second) {
                cout << file << ':' << it_line << ':' << s << endl;
            }
        }
    }
    for (auto it : n->children) {
        s += it.first;
        printTerminals(it.second, s);
        s.pop_back();
    }
}

bool query(Node *n, string &query) {
    for (size_t i = 0; i < query.length(); ++i) {
        if (n->children.find(query[i]) != n->children.end()) {
            n = n->children.find(query[i])->second;
        } else {
            cout << "No results for query: " << query << endl;
            return false;
        }
    }
    cout << "Results: " << endl;
    printTerminals(n, query);
    return true;
}

int countTokens(Node *n) {
    int count = n->terminal ? 1 : 0;
    for (auto it : n->children) {
        count += countTokens(it.second);
    }
    return count;
}

void indexFile(const string &filepath, int fileIndex) {
    char c;
    string s;
    int line = 1;
    std::ifstream file (filepath, ios::in);
    if (file.is_open()) {
        files.emplace(fileIndex, filepath);
        while (file.get(c) && c <= '~') {
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

int iterateFiles(const string &path) {
    int numFiles = 0;
    cout << "Root path: " << path << endl;
    recursive_directory_iterator it(path), end;
    while (it != end)
    {
        if (it->path().stem().string().length() == 0 || is_symlink(*it)) {
            it.no_push();
            ++it;
            continue;
        } else if (is_directory(*it)) {
            cout << "Processing directory: " << it->path().string() << endl;
            ++it;
            continue;
        }
        indexFile(it->path().string(), numFiles);
        ++it;
        ++numFiles;
    }
    return numFiles;
}

int main(int argc, char *argv[]) {
    root = new Node();

    string s (argv[1]);
    clock_t start = clock();
    iterateFiles(s);
    start = clock() - start;

    cout << "Trie construction time (s) : "
         << (double) start / CLOCKS_PER_SEC << endl;

    cout << "Tokens added: " << countTokens(root) << endl;

    cout << "Input query:" << endl;
    while (cin >> s && s != "done") {
        query(root, s);
        cout << "Input query:" << endl;
    }
    return 0;
}
