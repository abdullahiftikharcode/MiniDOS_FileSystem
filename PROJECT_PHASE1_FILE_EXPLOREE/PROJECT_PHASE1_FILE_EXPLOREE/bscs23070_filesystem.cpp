#include"bscs23070_filesystem.h"
FileSystem::FileSystem() {
    root = new Node("/", true);
    currentDir = root;
}
void FileSystem::rm(const string& path) {
   
    Node* prev = nullptr;
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = path.substr(0, lastSlash);
    string fileName = path.substr(lastSlash + 1);
    Node* parent = (findNode(parentPath, false));
    if (path.find('/') == string::npos && !(path.empty())) {
        parent = currentDir;
    }
    Node* file = parent->firstChild;
    while (file) {
        if (file->name == fileName && !file->isDirectory) {
            if (!hasPermission(file, 2 | 1)) {
                return; 
            }
            if (prev) {
                prev->nextSibling = file->nextSibling;
            }
            else {
                parent->firstChild = file->nextSibling;
            }
            delete file;
            SetClr(10);
            cout << "File " << fileName << " deleted." << endl;
            SetClr(7);
            return;
        }
        prev = file;
        file = file->nextSibling;
    }
    throw FileSystemException("Error: File not found.");
}

void getRowColbyLeftClick(int& rpos, int& cpos)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    do
    {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    } while (true);
}
void gotoRowCol(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}
int getTerminalSize(int& rows, int& cols) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return 0;
    }
    return -1;
}
void FileSystem::start_screeen() {
    int rows = 0, cols = 0;
    getTerminalSize(rows, cols);
    for (int i = 0; i < rows / 2;i++) {
        SetClr(9);
        system("cls");
        gotoRowCol(i,(cols/2)/2);
        cout << "  ____   ___  ____  ____   _____  __  ____    ___    ______   __     _    ____  ____  _   _ _     _        _    _   _ " << endl;
        gotoRowCol(i+1, (cols / 2) / 2);
        cout << " |  _ \\ / _ \\/ ___|| __ ) / _ \\ \\/ / |___ \\  / _ \\  | __ ) \\ / /    / \\  | __ )|  _ \\| | | | |   | |      / \\  | | | |" << endl;
        gotoRowCol(i+2, (cols / 2) / 2);
        cout << " | | | | | | \\___ \\|  _ \\| | | \\  /    __) || | | | |  _ \\\\ V /    / _ \\ |  _ \\| | | | | | | |   | |     / _ \\ | |_| |" << endl;
        gotoRowCol(i + 3, (cols / 2) / 2);
        cout << " | |_| | |_| |___) | |_) | |_| /  \\   / __/ | |_| | | |_) || |    / ___ \\| |_) | |_| | |_| | |___| |___ / ___ \\|  _  |" << endl;
        gotoRowCol(i + 4, (cols / 2) / 2);
        cout << " |____/ \\___/|____/|____/ \\___/_/\\_\\ |_____(_)___/  |____/ |_|   /_/   \\_\\____/|____/ \\___/|_____|_____/_/   \\_\\_| |_|" << endl;
        gotoRowCol(i + 5, (cols / 2) / 2);
        cout << "                                                                                                                      " << endl;
        Sleep(70);
    }
    SetClr(7);
}
Node* FileSystem::findNode(const string& path, bool createMissing = false) {
    Node* current = currentDir;
    if (path == "..") {
        if (currentDir->parent) {
            currentDir = currentDir->parent;
        }
        return currentDir;
    }
    if (path==".") {
        return current;
   }
    if (path.empty()) {
        return currentDir;
    }
    else if (path[0]=='/') {
        current = root;
    }

    size_t start = (path[0] == '/') ? 1 : 0;
    while (start < path.size()) {
        size_t end = path.find('/', start);
        string part = path.substr(start, end - start);
        start = (end == string::npos) ? path.size() : end + 1;

        if (part == "." || part.empty()) {
            continue;
        }

        if (part == "..") {
            if (current->parent) {
                current = current->parent;
            }
            continue;
        }

        Node* child = current->firstChild;
        bool found = false;

        while (child) {
            if (child->name == part && (child->isDirectory)) {
                current = child;
                found = true;
                break;
            }
            child = child->nextSibling;
        }

        if (!found) {
            if (createMissing) {
                Node* newDir = new Node(part, true, current);
                newDir->nextSibling = current->firstChild;
                current->firstChild = newDir;
                current = newDir;
            }
            else {
                return nullptr;
            }
        }
    }
    return current;
}


void FileSystem::mkdir(const string& path) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = (lastSlash == string::npos) ? "" : path.substr(0, lastSlash);
    string dirName = path.substr(lastSlash + 1);

    if (dirName.empty()) {
        throw FileSystemException("Error: Invalid directory name.");
        return;
    }
    Node* parent = findNode(parentPath);
    if (parent && parent->name == dirName) {
        SetClr(10);
        cout << "Directory " << path << " created." << endl;
        SetClr(7);
    }
    else if (parent && parent->isDirectory) {
        Node* finder = parent->firstChild;
        while (finder) {
            if (finder->name==dirName) {
                throw FileSystemException("ERROR : Directory " +dirName+" Already Exists");
                return;
            }
            finder = finder->nextSibling;
        }
        Node* newDir = new Node(dirName, true, parent);
        newDir->nextSibling = parent->firstChild;
        parent->firstChild = newDir;
        SetClr(10);
        cout << "Directory " << path << " created." << endl;
        SetClr(7);
    }
    else {
        throw FileSystemException( "Error: Invalid path or parent directory.");
    }
}


void FileSystem::cd(const string& path) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    if (path == "/") {
        currentDir = root;
    }
    else {
        Node* targetDir = findNode(path);
        if (targetDir && (targetDir->isDirectory||targetDir->isSymLink)) {
            if (targetDir->isSymLink) {
                string symlinkPath = targetDir->linkTarget;
                size_t lastSlash = symlinkPath.rfind('/');
                string parentPath = symlinkPath.substr(0, lastSlash);
                Node* targetNode = findNode(parentPath, false);
                if (!targetNode) {
                    throw FileSystemException("Error: Broken symbolic link: " + symlinkPath);
                }
                cd(symlinkPath);
                return;
            }
            currentDir = targetDir;
        }
        else {
            throw FileSystemException("Error: Directory not found.");
        }
    }
}
string FileSystem::pwd() {
    string path = "";
    Node* current = currentDir;
    while (current && current->name!="/") {
        path = "/" + current->name + path;
        current = current->parent;
    }
   string s =(path.empty() ? "/" : path);
   return s;
}
void FileSystem::ls() {
    Node* child = currentDir->firstChild;
    int i = 1;
    SetClr(3);
    while (child) {
        cout <<i<<". "<< child->name << (child->isDirectory ? "/" : "") << "\n";
        child = child->nextSibling;
        i++;
    }
    SetClr(7);
}
void FileSystem::touch(const string& path, const string& content = "") {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = path.substr(0, lastSlash);
    string fileName = path.substr(lastSlash + 1);

    Node* parent = findNode(parentPath, false);
    if (path.find('/') == string::npos) {
        parent = currentDir;
    }
    if (parent && parent->isDirectory) {
        Node* finder = parent->firstChild;
        while (finder) {
            if (finder->name == fileName) {
                throw FileSystemException("ERROR : File " + fileName + " Already Exists" );
                return;
            }
            finder = finder->nextSibling;
        }
        Node* newFile = new Node(fileName, false, parent);
        newFile->setContent(content);
        newFile->meta.fileSize = content.size();
        newFile->nextSibling = parent->firstChild;
        parent->firstChild = newFile;
        SetClr(10);
        cout << "File " << path << " created." << endl;
        SetClr(7);
    }
    else {
        throw FileSystemException("Error: Invalid path or parent directory.");
    }
}
void FileSystem::cat(const string& path) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = path.substr(0, lastSlash);
    string fileName = path.substr(lastSlash + 1);

    Node* file = (findNode(parentPath, false));
    if (path.find('/') == string::npos && !(path.empty())) {
        file = currentDir;
    }
    if (!file) {
        throw FileSystemException("Error: Invalid directory name.");
        return;
    }
    file = file->firstChild;
    while (file) {
        if (file->name == fileName && !file->isDirectory) {
            if (!hasPermission(file, 4)) {
                return;
            }
            if ( file->isSymLink) {
                string symlinkPath = file->linkTarget;
                size_t lastSlash = symlinkPath.rfind('/');
                string parentPath = symlinkPath.substr(0, lastSlash);
                Node* targetNode = findNode(parentPath, false);
                if (!targetNode) {
                    throw FileSystemException("Error: Broken symbolic link: " + symlinkPath);
                }
                cat(symlinkPath);
                return;
            }
            cout << file->getContent() << endl;
            return;
        }
        file = file->nextSibling;
    }
    throw FileSystemException("Error: File not found.");
}
bool FileSystem::path_length_exceed(int size) {
    return (size >= 255);
}
void FileSystem::mv(string source, string dest) {
    if (path_length_exceed(source.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlashsource = source.rfind('/');
    string parentsource = source.substr(0, lastSlashsource);
    string fileName = source.substr(lastSlashsource + 1);
    Node* sourcefile = (findNode(parentsource, false));
    if (source.find('/') == string::npos && !(source.empty())) {
        sourcefile = currentDir;
    }
    if (!sourcefile ) {
        throw FileSystemException("Error: Source parent directory does not exist.");
        return;
    }
    Node* destfile = findNode(dest, false);
    if (!destfile ) {
        throw FileSystemException("Error: Destination path is invalid or does not exist.");
        return;
    }
    Node* file = sourcefile->firstChild;
    Node* prev = nullptr;
    bool found = true;
    while (file) {
        if (file->name==fileName) {
            if (!hasPermission(file, 2)) { 
                return;
            }
            found = false;
            break;
        }
        prev = file;
        file = file->nextSibling;
    }
    if (found) {
        throw FileSystemException("ERROR : UNABLE TO FIND FILE TO MOVE FROM SOURCE .. ");
        return;
    }
    Node* finder = destfile->firstChild;
    while (finder) {
        if (finder->name == fileName) {
            throw FileSystemException("ERROR : File " + fileName + " Already Exists");
            return;
        }
        finder = finder->nextSibling;
    }
    if (sourcefile->firstChild->name==file->name) {
        sourcefile->firstChild = file->nextSibling;
    }
    else if (file->nextSibling==nullptr) {
        prev->nextSibling = nullptr;
    }
    else {
        // if it is somwhwere between and not first or last
        prev->nextSibling = file->nextSibling;
    }

    file->parent = destfile;
    file->nextSibling = destfile->firstChild;
    destfile->firstChild = file;
    SetClr(10);
    cout << "Moved from Source : "<<source<<" to Dest : "<<dest<<endl;
    SetClr(7);
}

void FileSystem::cp(string source, string dest) {
    if (path_length_exceed(source.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    if (path_length_exceed(dest.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlashsource = source.rfind('/');
    string parentsource = source.substr(0, lastSlashsource);
    string fileName = source.substr(lastSlashsource + 1);
    Node* sourcefile = (findNode(parentsource, false));
    if (source.find('/') == string::npos && !(source.empty())) {
        sourcefile = currentDir;
    }
    if (!sourcefile ) {
        throw FileSystemException("Error: Source parent directory does not exist.");
        return;
    }
    Node* destfile = findNode(dest, false);
    if (!destfile ) {
        throw FileSystemException("Error: Destination path is invalid or does not exist.");
        return;
    }
    Node* file = sourcefile->firstChild;
    Node* prev = nullptr;
    bool found = true;
    while (file) {
        if (file->name == fileName) {
            if (!hasPermission(file, 4)) { 
                return;
            }
            found = false;
            break;
        }
        prev = file;
        file = file->nextSibling;
    }
    if (found) {
        throw FileSystemException("ERROR : UNABLE TO FIND FILE TO COPY FROM SOURCE .. ");
        return;
    }
    Node* finder = destfile->firstChild;
    while (finder) {
        if (finder->name == fileName) {
            throw FileSystemException("ERROR : File " + fileName + " Already Exists");
            return;
        }
        finder = finder->nextSibling;
    }
    Node* copyfile = new Node (file);
    copyfile->nextSibling = nullptr;
    copyfile->parent = destfile;
    copyfile->nextSibling = destfile->firstChild;
    destfile->firstChild = copyfile;
    SetClr(10);
    cout << "Copied from Source : " << source << " to Dest : " << dest << endl;
    SetClr(7);
}
void FileSystem::rename(string oldName, string newName) {
    if (path_length_exceed(oldName.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlasholdName = oldName.rfind('/');
    string parentoldName = oldName.substr(0, lastSlasholdName);
    string fileName = oldName.substr(lastSlasholdName + 1);
    Node* oldNamefile = (findNode(parentoldName, false));
    if (oldName.find('/') == string::npos && !(oldName.empty())) {
        oldNamefile = currentDir;
    }
    if (!oldNamefile) {
        throw FileSystemException("Error: oldName parent directory does not exist.");
        return;
    }
    Node* file = oldNamefile->firstChild;
    Node* prev = nullptr;
    bool found = true;
    while (file) {
        if (file->name == fileName) {
            found = false;
            break;
        }
        prev = file;
        file = file->nextSibling;
    }
    if (found) {
        throw FileSystemException("ERROR : UNABLE TO FIND FILE TO COPY FROM oldName .. ");
        return;
    }
    file->name = newName;
    SetClr(10);
    cout << "STATUS : NAME CHANGED SUCCESSFULLY"<<endl;
    SetClr(7);
}

void FileSystem::rmdir(const string& path) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("Error: Path length exceeded.");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = (lastSlash == string::npos) ? "" : path.substr(0, lastSlash);
    string dirName = path.substr(lastSlash + 1);
    Node* parent = findNode(parentPath, false);
    if (!parent || !parent->isDirectory) {
        throw FileSystemException("Error: Parent directory not found.");
        return;
    }
    Node* prev = nullptr;
    Node* current = parent->firstChild;
    while (current) {
        if (current->name == dirName && current->isDirectory) {
            if (!hasPermission(current, 1)) {
                return;
            }
            if (currentDir == current || isDescendant(currentDir, current)) {
                currentDir = (current->parent) ? current->parent : root;
                SetClr(1);
                cout << "Warning: Current directory moved to '" << currentDir->name << "' before deletion." << endl;
                SetClr(7);
            }
            if (prev) {
                prev->nextSibling = current->nextSibling;
            }
            else {
                parent->firstChild = current->nextSibling;
            }
            deleteDirectory(current);
            SetClr(10);
            cout << "Directory '" << dirName << "' deleted successfully." << endl;
            SetClr(7);
            return;
        }

        prev = current;
        current = current->nextSibling;
    }
    throw FileSystemException("Error: Directory '" + dirName + "' not found.");
}

void FileSystem::deleteDirectory(Node* directory) {
    if (!directory) return;
    Node* child = directory->firstChild;
    while (child) {
        Node* next = child->nextSibling;
        deleteDirectory(child);
        child = next;
    }
    delete directory;
}

bool FileSystem::isDescendant(Node* node, Node* potentialAncestor) {
    while (node) {
        if (node == potentialAncestor) {
            return true;
        }
        node = node->parent;
    }
    return false;
}

void FileSystem::ln_s(const string& target, const string& linkName) {
    size_t lastSlash = linkName.rfind('/');
    string parentPath = (lastSlash == string::npos) ? "" : linkName.substr(0, lastSlash);
    string linkFileName = linkName.substr(lastSlash + 1);

    Node* parent = findNode(parentPath, false);
    if (!parent || !parent->isDirectory) {
        throw FileSystemException("Error: Invalid parent directory for link '" + linkName + "'." );
        return;
    }
    Node* finder = parent->firstChild;
    while (finder) {
        if (finder->name == linkFileName) {
            throw FileSystemException("ERROR : Link " + linkFileName + " Already Exists" );
            return;
        }
        finder = finder->nextSibling;
    }
    Node* newLink = new Node(linkFileName, false, parent);
    newLink->isSymLink = true;
    newLink->linkTarget = target;
    newLink->nextSibling = parent->firstChild;
    parent->firstChild = newLink;
    SetClr(10);
    cout << "Symbolic link '" << linkName << "' -> '" << target << "' created successfully." << endl;
    SetClr(7);
}

void FileSystem::stat(const string& path) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string dirName = path.substr(lastSlash + 1);
    Node* node = findNode(path, false);
    if (path.find('/') == string::npos && !(path.empty())) {
        node = currentDir;
    }
    if (!node) {
        throw FileSystemException("Error: Path not found.");
        return;
    }
    bool out = true;
   if (node->name!=dirName) {
       node = node->firstChild;
        while (node) {
            if (node->name == dirName) {
                out = false;
                break;
            }
            node = node->nextSibling;
        }
        if (out) {
            throw FileSystemException(" ERROR : FILE DOESNT EXIST CANT DISPLAY STATS ");
            return;
        }
    }
    SetClr(1);
    cout << "Metadata for '" << path << "':" << endl;
    cout << "Name: " << node->name << endl;
    cout << "Type: " << (node->isDirectory ? "Directory" : "File") << endl;
    if (!node->isDirectory) {
        cout << "Size: " << node->meta.fileSize << " bytes" << endl;
        cout << "Content: " << node->getContent() << endl;
    }

    char modifiedTime[26];  
    ctime_s(modifiedTime, sizeof(modifiedTime), &node->meta.modifiedAt);
    cout << "Last Modified: " << modifiedTime << endl;

    char createdTime[26]; 
    ctime_s(createdTime, sizeof(createdTime), &node->meta.createdAt);
    cout << "Created At: " << createdTime << endl;

    cout << "Owner: " << node->meta.owner << endl;
    cout << "Permissions: " << node->meta.permissions << endl;
    if (node->isSymLink) {
        cout << "Symbolic Link: Yes" << endl;
        cout << "Link Target: " << node->linkTarget << endl;
    }
    else {
        cout << "Symbolic Link: No" << endl;
    }
    if (node->parent) {
        cout << "Parent Directory: " << node->parent->name << endl;
    }
    else {
        cout << "Parent Directory: N/A" << endl;
    }
    SetClr(7);
}

void FileSystem::chmod(string path, unsigned int mode) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    if (mode > 777) {
        throw FileSystemException("ERROR: Invalid permission mode. Must be between 000 and 777.");
        return;
    }
    unsigned int owner = mode/100;  
    unsigned int group = (mode/10)%10; 
    unsigned int other = mode%10;   
    if (owner > 7 || group > 7 || other > 7) {
        throw FileSystemException("ERROR: Invalid permission digits. Each permission digit must be between 0 and 7.");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string dirName = path.substr(lastSlash + 1);
    Node* node = findNode(path, false);
    if (path.find('/') == string::npos && !(path.empty())) {
        node = currentDir;
    }
    if (!node) {
        throw FileSystemException("Error: Path not found.");
        return;
    }

    bool out = true;
    if (node->name != dirName) {
        node = node->firstChild;
        while (node) {
            if (node->name == dirName) {
                out = false;
                break;
            }
            node = node->nextSibling;
        }
        if (out) {
            throw FileSystemException("ERROR: File doesn't exist. Cannot update permissions.");
            return;
        }
    }
    if (!hasPermission(node, 1)) {
        return;
    }
    node->meta.permissions = mode;
    SetClr(10);
    cout << "Permissions updated successfully for " << path << " to " <<  mode << endl;
    SetClr(7);
}
void FileSystem::chown(string path, string newOwner) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED" );
        return;
    }
    size_t lastSlash = path.rfind('/');
    string dirName = path.substr(lastSlash + 1);
    Node* node = findNode(path, false);
    if (path.find('/') == string::npos && !(path.empty())) {
        node = currentDir;
    }
    if (!node) {
        throw FileSystemException("Error: Path not found.");
        return;
    }

    bool out = true;
    if (node->name != dirName) {
        node = node->firstChild;
        while (node) {
            if (node->name == dirName) {
                out = false;
                break;
            }
            node = node->nextSibling;
        }
        if (out) {
            throw FileSystemException("ERROR: File doesn't exist. Cannot update permissions.");
            return;
        }
    }
    node->meta.owner = newOwner;
    SetClr(10);
    cout << "Owner updated successfully for " << path << " to " << newOwner << endl;
    SetClr(7);
}
void FileSystem::search(Node* node, string currentPath, string pattern) {
    if (!node) return;
    if (node->name.find(pattern) != string::npos) {
        SetClr(1);
        cout << currentPath + "/" + node->name << endl;
        SetClr(7);
    }
    if (node->firstChild) {
        if (node->name!="/" && currentPath!="/") {
            search(node->firstChild, currentPath + "/" + node->name, pattern);
        }
        else {
            search(node->firstChild, currentPath+ node->name, pattern);
        }
    }
    if (node->nextSibling) {
        search(node->nextSibling, currentPath, pattern);
    }
}
void FileSystem::find(string pattern) {
    Node* searcho=root;
    if (currentDir) {
        searcho = currentDir;
    }
    SetClr(10);
    cout << "Searching for pattern \"" << pattern << "\":" << endl;
    SetClr(7);
    search(searcho, "", pattern);
}

void FileSystem::find_i(string pattern) {
    Node* searcho = root;
    if (currentDir) {
        searcho = currentDir;
    }
    SetClr(10);
    cout << "Searching for pattern \"" << pattern << "\":" << endl;
    SetClr(7);
    search_i(searcho, "", pattern);
}
void FileSystem::search_i(Node* node, string currentPath, string pattern) {
    if (!node) return;
    if (toLowerCase(node->name).find(toLowerCase(pattern)) != string::npos) {
        SetClr(1);
        cout << currentPath + "/" + node->name << endl;
        SetClr(7);
    }
    if (node->firstChild) {
        if (node->name != "/" && currentPath != "/") {
            search_i(node->firstChild, currentPath + "/" + node->name, pattern);
        }
        else {
            search_i(node->firstChild, currentPath + node->name, pattern);
        }
    }
    if (node->nextSibling) {
        search_i(node->nextSibling, currentPath, pattern);
    }
}
std::string FileSystem::toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    for (char& c : lowerStr) {
        c = std::tolower(c);
    }
    return lowerStr;
}

void FileSystem::grep(string content) {
    if (!currentDir) {
        throw FileSystemException("Error: No current directory to search from.");
        return;
    }
    SetClr(10);
    cout << "Searching for content \"" << content << "\":" << endl;
    SetClr(7);
    searchContent(currentDir, content, "");
}

void FileSystem::searchContent(Node* node, const string& content, const string& currentPath) {
    if (!node) return;

    if (!node->isDirectory) {
        if (node->content.find(content) != string::npos) {
            SetClr(1);
            cout << "Match found in file: " << currentPath + "/" + node->name << endl;
            SetClr(7);
        }
    }
    if (node->firstChild) {
        if (node->name != "/" && currentPath != "/") {
            searchContent(node->firstChild, content,currentPath + "/" + node->name);
        }
        else {
            searchContent(node->firstChild,content, currentPath + node->name);
        }
    }
    if (node->nextSibling) {
        searchContent(node->nextSibling, content, currentPath);
    }
}

void FileSystem::saveToFile(string filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw FileSystemException("Error: Could not open file for saving.");
        return;
    }

    serializeNode(root, outFile, 0); 
    outFile.close();
    SetClr(10);
    cout << "File system saved to " << filename << endl;
    SetClr(7);
}

void FileSystem::serializeNode(Node* node, ofstream& outFile, int depth) {
    if (!node) return;
    string indent(depth * 2, ' ');
    outFile << indent << "{\n";
    string name = node->name;
    if (node==root) {
        name = "/";
    }
    outFile << indent << "  \"name\": \"" << name << "\",\n";
    outFile << indent << "  \"isDirectory\": " << (node->isDirectory ? "true" : "false") << ",\n";
    outFile << indent << "  \"content\": \"" << node->content << "\",\n";
    outFile << indent << "  \"createdAt\": " << node->meta.createdAt << ",\n";
    outFile << indent << "  \"modifiedAt\": " << node->meta.modifiedAt << ",\n";
    outFile << indent << "  \"fileSize\": " << node->meta.fileSize << ",\n";
    outFile << indent << "  \"owner\": \"" << node->meta.owner << "\",\n";
    outFile << indent << "  \"permissions\": " << node->meta.permissions << ",\n";
    outFile << indent << "  \"isSymLink\": " << (node->isSymLink ? "true" : "false") << ",\n";
    outFile << indent << "  \"linkTarget\": \"" << node->linkTarget << "\"";
    if (node->firstChild) {
        outFile << ",\n" << indent << "  \"firstChild\": ";
        serializeNode(node->firstChild, outFile, depth + 1);
    }
    else {
        outFile << ",\n" << indent << "  \"firstChild\": null";
    }

    if (node->nextSibling) {
        outFile << ",\n" << indent << "  \"nextSibling\": ";
        serializeNode(node->nextSibling, outFile, depth);
    }
    else {
        outFile << ",\n" << indent << "  \"nextSibling\": null";
    }

    outFile << "\n" << indent << "}";
}


void FileSystem::loadFromFile(string filename) {
    ifstream inFile(filename);  
    if (!inFile.is_open()) {
        throw FileSystemException("Error: Could not open file for loading.");
        return;
    }
    delete root;
    root = deserializeNode(inFile);  
    currentDir = root; 
    SetClr(10);
    cout << "File system loaded from " << filename << endl;
    SetClr(7);
}

Node* FileSystem::deserializeNode(ifstream& inFile) {
    string line;
    Node* node = new Node("", true);
    while (getline(inFile, line)) {
        if (line.find("\"name\"") != string::npos) {
            node->name = extractValue(line);
        }
        else if (line.find("\"isDirectory\"") != string::npos) {
            node->isDirectory = (extractValue(line) == "true");
        }
        else if (line.find("\"content\"") != string::npos) {
            node->content = extractValue(line);
        }
        else if (line.find("\"createdAt\"") != string::npos) {
            node->meta.createdAt = stoi(extractValue(line));
        }
        else if (line.find("\"modifiedAt\"") != string::npos) {
            node->meta.modifiedAt = stoi(extractValue(line));
        }
        else if (line.find("\"fileSize\"") != string::npos) {
            node->meta.fileSize = stoi(extractValue(line));
        }
        else if (line.find("\"owner\"") != string::npos) {
            node->meta.owner = extractValue(line);
        }
        else if (line.find("\"permissions\"") != string::npos) {
            node->meta.permissions = stoi(extractValue(line));
        }
        else if (line.find("\"isSymLink\"") != string::npos) {
            node->isSymLink = (extractValue(line) == "true");
        }
        else if (line.find("\"linkTarget\"") != string::npos) {
            node->linkTarget = extractValue(line);
        }
        else if (line.find("\"firstChild\"") != string::npos) {
            if (line.find("null") == string::npos) {
                node->firstChild = deserializeNode(inFile);  
                node->firstChild->parent = node;  
            }
        }
        else if (line.find("\"nextSibling\"") != string::npos) {
            if (line.find("null") == string::npos) {
                node->nextSibling = deserializeNode(inFile);  
            }
            break;  
        }
        else if (line.find("}") != string::npos) {
            break;  
        }
    }

    return node;
}


string FileSystem::extractValue(const string& line) {
    size_t start = line.find(":") + 1;
    if (start == string::npos) return "";

    while (start < line.size() && isspace(line[start])) {
        start++;
    }

    string value;
    bool inString = false;
    bool escape = false;

    for (size_t i = start; i < line.size(); ++i) {
        char currentChar = line[i];

        if (escape) {
           
            value += currentChar;
            escape = false;
        }
        else {
            if (currentChar == '\\') {
                escape = true; 
            }
            else if (currentChar == '"') {
               
                if (inString) {
                    break; 
                }
                else {
                    inString = true; 
                }
            }
            else {
                value += currentChar;
            }
        }
    }
    value.erase(remove(value.begin(), value.end(), ','), value.end());
    value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
    return value;
}

void FileSystem::write(const string& path,const string & content) {
    if (path_length_exceed(path.size())) {
        throw FileSystemException("PATH LENGTH EXCEEDED");
        return;
    }
    size_t lastSlash = path.rfind('/');
    string parentPath = path.substr(0, lastSlash);
    string fileName = path.substr(lastSlash + 1);

    Node* file = (findNode(parentPath, false));
    if (path.find('/') == string::npos && !(path.empty())) {
        file = currentDir;
    }
    file = file->firstChild;
    while (file) {
        if (file->name == fileName && !file->isDirectory) {
            if (!hasPermission(file, 2)) {
                return; 
            }
            if (file->isSymLink) {
                string symlinkPath = file->linkTarget;
                size_t lastSlash = symlinkPath.rfind('/');
                string parentPath = symlinkPath.substr(0, lastSlash);
                Node* targetNode = findNode(parentPath, false);
                if (!targetNode) {
                    throw FileSystemException("Error: Broken symbolic link: " + symlinkPath);
                }
                write(symlinkPath,content);
                return;
            }
            file->setContent(content);
            file->meta.fileSize = content.size();
            SetClr(10);
            cout << "Succesfully Overwritten File : " << file->name<<endl;
            SetClr(7);
            return;
        }
        file = file->nextSibling;
    }
    throw FileSystemException("Error: File not found.");
}
bool FileSystem::hasPermission(Node* node, unsigned int requiredPermission) {
    if (!node) {
        throw FileSystemException("ERROR: Node does not exist.");
        return false;
    }
    unsigned int permissions = node->meta.permissions;
    unsigned int owner = permissions / 100;       
    unsigned int group = (permissions / 10) % 10; 
    unsigned int others = permissions % 10;       
    if ((requiredPermission & 4 && !(owner & 4)) || 
        (requiredPermission & 2 && !(owner & 2)) || 
        (requiredPermission & 1 && !(owner & 1))) { 
        throw FileSystemException("ERROR: Insufficient permissions to perform this operation.");
        return false;
    }

    return true;
}

void FileSystem::clearNodes(Node* node) {
    if (!node) return;
    clearNodes(node->firstChild);
    clearNodes(node->nextSibling);
    delete node;
}
FileSystem::~FileSystem() {
    clearNodes(root);
}
void FileSystem::SetClr(int clr)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}
void FileSystem::executeCommand(const string &command) {
    if (command.substr(0,6)=="mkdir ") {
        string dirName = command.substr(6);
        mkdir(dirName);
    }
    else if (command.substr(0, 3) == "cd ") {
        string dirName = command.substr(3);
        cd(dirName);
    }
    else if (command.substr(0, 3) == "pwd" && command.size() == 3) {
        SetClr(1);
        cout << "Current Directory: ";
        pwd();
        SetClr(7);
        cout << endl;
    }
    else if (command.substr(0, 2) == "ls" && command.size() == 2) {
        ls();
    }
    else if (command.substr(0, 3) == "cls" && command.size()==3) {
        system("cls");
    }
    else if (command.substr(0, 6) == "touch ") {
        string rest = command.substr(6);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path = rest.substr(0, spacePos);
            string content = rest.substr(spacePos + 1);

            touch(path, content);
        }
        else {
            throw FileSystemException("ERROR: Touch command requires both path and content");
        }
    }
    else if (command.substr(0, 6) == "write ") {
        string rest = command.substr(6);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path = rest.substr(0, spacePos);
            string content = rest.substr(spacePos + 1);

            write(path, content);
        }
        else {
            throw FileSystemException("ERROR: write command requires both path and content");
        }
    }
    else if (command.substr(0, 3) == "cp ") {
        string rest = command.substr(3);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path = rest.substr(0, spacePos);
            string content = rest.substr(spacePos + 1);

            cp(path, content);
        }
        else {
            throw FileSystemException("ERROR: copy command requires both source and destination");
        }
    }
    else if (command.substr(0, 3) == "mv ") {
        string rest = command.substr(3);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path1 = rest.substr(0, spacePos);
            string path2 = rest.substr(spacePos + 1);

            mv(path1,path2);
        }
        else {
            throw FileSystemException("ERROR: move command requires both source and destination");
        }
    }
    else if (command.substr(0, 7) == "rename ") {
        string rest = command.substr(7);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path1 = rest.substr(0, spacePos);
            string path2 = rest.substr(spacePos + 1);

            rename(path1, path2);
        }
        else {
            throw FileSystemException("ERROR: rename command requires both oldname and newname");
        }
    }
    else if (command.substr(0,5) == "ln-s ") {
        string rest = command.substr(5);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string target = rest.substr(0, spacePos);
            string linkname = rest.substr(spacePos + 1);

            ln_s(target, linkname);
        }
        else {
            throw FileSystemException("ERROR: link command requires both target and linkname");
        }
    }
    else if (command.substr(0, 6) == "chmod ") {
        string rest = command.substr(6);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path = rest.substr(0, spacePos);
            string code = rest.substr(spacePos + 1);
            int i = stoi(code);
            chmod(path, i);
        }
        else {
            throw FileSystemException("ERROR: CHMOD command requires both path and permissioncode");
        }
    }
    else if (command.substr(0, 6) == "chown ") {
        string rest = command.substr(6);
        size_t spacePos = rest.find(' ');

        if (spacePos != string::npos) {
            string path = rest.substr(0, spacePos);
            string owner = rest.substr(spacePos + 1);
            chown(path,owner);
        }
        else {
            throw FileSystemException("ERROR: CHOWN command requires both path and owner name");
        }
    }
    else if (command.substr(0, 5) == "find ") {
        string rest = command.substr(5);
        find(rest);
     }
    else if (command.substr(0, 7) == "find-i ") {
        string rest = command.substr(7);
        find_i(rest);
     }
    else if (command.substr(0, 5) == "grep ") {
        string rest = command.substr(5);
        grep(rest);
     }
    else if (command.substr(0, 5) == "save ") {
        string rest = command.substr(5);
        saveToFile(rest);
     }
    else if (command.substr(0, 5) == "load ") {
         string rest = command.substr(5);
         loadFromFile(rest);
     }
    else if (command.substr(0, 5) == "stat ") {
        string rest = command.substr(5);
        stat(rest);
    }
    else if (command.substr(0, 6) == "rmdir ") {
        string rest = command.substr(6);
        rmdir(rest);
    }
    else if (command.substr(0, 3) == "rm ") {
        string rest = command.substr(3);
        rm(rest);
    }
    else if (command.substr(0, 4) == "cat ") {
        string rest = command.substr(4);
        cat(rest);
    }
    else if (command=="exit") {
        exit(1);
    }
    else {
        throw FileSystemException("ERROR : Unknown Command Enterd");
    }
}
void FileSystem::startCLI() {
    start_screeen();
    system("cls");
    while (true) {
        try {
            cout <<pwd()<< "> ";
            string command;
            getline(cin, command);
            executeCommand(command);
        }
        catch (FileSystemException e) {
            SetClr(4);
            cout << e.what()<<endl;
            SetClr(7);
        }
    }
}

