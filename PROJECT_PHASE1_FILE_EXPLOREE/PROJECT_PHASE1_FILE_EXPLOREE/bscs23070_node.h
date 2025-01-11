#pragma once
#include <iostream>
#include <string>
#include<Windows.h>
#include <iomanip> 
#include <ctime>   
#include<fstream>
using namespace std;
class Node {
public:
    struct MetaData {
        time_t createdAt;
        time_t modifiedAt;
        size_t fileSize;
        string owner;
        unsigned int permissions;
        string toString() const {

        }
    };
    string name;
    bool isDirectory;
    string content;
    Node* firstChild;
    Node* nextSibling;
    Node* parent;
    MetaData meta;
    bool isSymLink;
    string linkTarget;

    Node(string name, bool isDirectory, Node* parent = nullptr)
        : name(name), isDirectory(isDirectory), firstChild(nullptr),
        nextSibling(nullptr), parent(parent) {
        meta.modifiedAt = time(0);
        meta.createdAt = time(0);
        meta.permissions = isDirectory ? 777 : 777;
        meta.owner = "root";
    }

    void setContent(const string& content) {
        meta.modifiedAt = time(0);
        if (isDirectory) {
            cout << "Cannot set content for a directory." << endl;
        }
        else {
            this->content = content;
        }
    }
    string getContent() {
        if (isDirectory) {
            return "";
        }
        return content;
    }

    Node(const Node* other, Node* newParent = nullptr)
        : name(other ? other->name : ""),
        isDirectory(other ? other->isDirectory : false),
        content(other ? other->content : ""),
        parent(newParent),
        isSymLink(other ? other->isSymLink : false),
        linkTarget(other ? other->linkTarget : "") {
        meta.createdAt = other ? other->meta.createdAt : time(nullptr);
        meta.modifiedAt = other ? other->meta.modifiedAt : time(nullptr);
        meta.fileSize = other ? other->meta.fileSize : 0;
        meta.owner = other ? other->meta.owner : "";
        meta.permissions = other ? other->meta.permissions : 0;
        if (other) {
            firstChild = other->firstChild ? new Node(other->firstChild, this) : nullptr;
            nextSibling = other->nextSibling ? new Node(other->nextSibling, newParent) : nullptr;
        }
        else {
            firstChild = nullptr;
            nextSibling = nullptr;
        }
    }
};