#pragma once
#include"bscs23070_node.h"
class FileSystem {
private:
    Node* root;
    Node* currentDir;
    Node* findNode(const string& path, bool createMissing);
public:
    FileSystem();
    void mkdir(const string& path);
    void cd(const string& path);
    string pwd();
    void ls();
    void touch(const string& path, const string& content);
    void write(const string& path, const string& content);
    void cat(const string& fileName);
    void rm(const string& fileName);
    bool path_length_exceed(int size);
    void mv(string source, string dest);
    void cp(string source, string dest);
    void rename(string oldName, string newName);
    void rmdir(const string& path);
    bool isDescendant(Node* node, Node* potentialAncestor);
    void deleteDirectory(Node* directory);
    void ln_s(const string& target, const string& linkName);
    void stat(const string& path);
    void chmod(string path, unsigned int mode);
    void chown(string path, string newOwner);
    void search(Node* node, string currentPath, string pattern);
    void find(string pattern);
    void find_i(string pattern);
    void search_i(Node* node, string currentPath, string pattern);
    std::string toLowerCase(const std::string& str);
    void grep(string content);
    void searchContent(Node* node, const string& content, const string& currentPath);
    void saveToFile(string filename);
    void serializeNode(Node* node, ofstream& outFile, int depth);
    void loadFromFile(string filename);
    Node* deserializeNode(ifstream& inStream);
    string extractValue(const string& line);
    bool hasPermission(Node* node, unsigned int requiredPermission);
    void clearNodes(Node* node);
    class FileSystemException : public std::exception {
        string message;
    public:
        FileSystemException(string msg) : message(msg) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    };
    void startCLI();
    void SetClr(int clr);
    void executeCommand(const string& command);
    void start_screeen();
    ~FileSystem();
};