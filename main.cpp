#include <iostream>
#include <map>
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
    bool terminal;
};

bool isTokenDelimiter(char c) {
    return c < 'a' || c > 'z';
}

bool isCamlDelimiter(char c) {
    return c < 'A' || c > 'Z';
}

bool isWordDelimiter(char c) {
    return isTokenDelimiter(c) && isCamlDelimiter(c) && (c < '-' || c > ':') && c != '_';
}

void insertToken(Node *root, const string &token) {
    for (size_t i = 0; i < token.length(); ++i) {
        if (root->children.find(token[i]) == root->children.end()) {
            root->children.emplace(token[i], new Node());
        }
        root = root->children[token[i]];
    }
    root->terminal = true;
}

void insertWord(Node *root, const string &word) {
    for (size_t i = 0; i < word.length(); ++i) {
        if (isTokenDelimiter(word[i])) {
            if (isCamlDelimiter(word[i])) {
                insertToken(root, word.substr(i + 1));
            } else {
                insertToken(root, word.substr(i));
            }
        }
    }
    insertToken(root, word);
}

void printTerminals(Node *root, string &s) {
    if (root->terminal) {
        cout << s << endl;
    }
    for (auto it : root->children) {
        s += it.first;
        printTerminals(it.second, s);
        s.pop_back();
    }
}

bool query(Node *root, string &query) {
    for (size_t i = 0; i < query.length(); ++i) {
        if (root->children.find(query[i]) != root->children.end()) {
            root = root->children.find(query[i])->second;
        } else {
            cout << "No results for query: " << query << endl;
            return false;
        }
    }
    cout << "Results: " << endl;
    printTerminals(root, query);
    return true;
}

int countTokens(Node *root) {
    int count = root->terminal ? 1 : 0;
    for (auto it : root->children) {
        count += countTokens(it.second);
    }
    return count;
}

void indexFile(const string &filepath, Node *root) {
    char c;
    string s;
    std::ifstream file (filepath, ios::in);
    if (file.is_open()) {
        while (file.get(c) && c <= '~') {
            if (isWordDelimiter(c) && s.length()) {
                insertWord(root, s);
                s.clear();
            } else if (c > ' ') {
                s += c;
            }
        }
        file.close();
    } else {
        cout << "Failed to open file: " << s << endl;
    }
}

int iterateFiles(const string &path, Node *root) {
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
        indexFile(it->path().string(), root);
        ++it;
        ++numFiles;
    }
    return numFiles;
}

int main(int argc, char *argv[]) {
    Node *root = new Node();

    string s (argv[1]);
    clock_t start = clock();
    iterateFiles(s, root);
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
