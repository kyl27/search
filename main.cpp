#include <iostream>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <ctime>

using namespace std;


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

void insertToken(Node *root, string token) {
    for (unsigned int i = 0; i < token.length(); ++i) {
        if (root->children.find(token[i]) == root->children.end()) {
            root->children.insert(make_pair(token[i], new Node()));
        }
        root = root->children[token[i]];
    }
    root->terminal = true;
}

void insertWord(Node *root, string word) {
    for (unsigned int i = 0; i < word.length(); ++i) {
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

void printTerminals(Node *root, string& s) {
    if (root->terminal) {
        cout << s << endl;
    }
    for (map<char, Node*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
        s += it->first;
        printTerminals(it->second, s);
        s.erase(s.size() - 1, 1);
    }
}

bool query(Node *root, string query) {
    for (unsigned int i = 0; i < query.length(); ++i) {
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
    for (map<char, Node*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
        count += countTokens(it->second);
    }
    return count;
}

int main(int argc, char *argv[]) {
    Node *root = new Node();

    string s (argv[1]);
    char c;
    clock_t start = clock();

    ifstream file (s, ios::in);
    if (file.is_open()) {
        while (file.get(c)) {
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
    start = clock() - start;

    cout << "Trie construction time (s) : " << (double) start / CLOCKS_PER_SEC << endl;
    cout << "Tokens added: " << countTokens(root) << endl;

    cout << "Input query:" << endl;
    while (cin >> s && s != "done") {
        query(root, s);
        cout << "Input query:" << endl;
    }
    return 0;
}
