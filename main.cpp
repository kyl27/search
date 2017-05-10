#include <iostream>
#include <fstream>
#include <map>
#include <regex>
#include <string>

using namespace std;


struct Node {
    map<char, Node*> children;
    bool terminal;
};

bool isTokenDelimiter(char c) {
    return c < 'a' || c > 'z';
}

bool isWordDelimiter(char c) {
    return isTokenDelimiter(c) && (c < 'A' || c > 'Z') && (c < '-' || c > ':') && c != '_';
}

void insertToken(Node *root, string token) {
    for (int i = 0; i < token.length(); ++i) {
        if (root->children.find(token[i]) == root->children.end()) {
            root->children.emplace(token[i], new Node());
        }
        root = root->children[token[i]];
    }
    root->terminal = true;
}

void insertWord(Node *root, string word) {
    for (int i = 0; i < word.length(); ++i) {
        if (isTokenDelimiter(word[i])) {
            insertToken(root, word.substr(i));
        }
    }
    insertToken(root, word);
}

void printTerminals(Node *root, string& s) {
    if (root->terminal) {
        cout << s << endl;
    }
    for (auto it : root->children) {
        s += it.first;
        printTerminals(it.second, s);
        s.pop_back();
    }
}

bool query(Node *root, string query) {
    for (int i = 0; i < query.length(); ++i) {
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

int main(int argc, char *argv[]) {
    Node *root = new Node();

    string s (argv[1]);
    char c;

    ifstream file (s, ios::in);
    if (file.is_open()) {
        while (!file.eof()) {
            file.get(c);
            if (isWordDelimiter(c)) {
                insertWord(root, s);
                s.clear();
            } else {
                s += c;
            }
        }
        file.close();
    } else {
        cout << "Failed to open file: " << s << endl;
    }

    cout << "Tokens added: " << countTokens(root) << endl;

    cout << "Input query:" << endl;
    while (cin >> s && s != "done") {
        query(root, s);
        cout << "Input query:" << endl;
    }
    return 0;
}
