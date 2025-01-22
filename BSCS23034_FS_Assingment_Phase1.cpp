#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <fstream>
using namespace std;
class System;
struct Node {
    string name;
    bool isDirectory;
    string content;
    Node* firstChild;
    Node* nextSibling;
    Node* parent;
    time_t createdat;
    time_t modifiedat;
    size_t filesize;
    string owner;
    unsigned int permission;
    bool issymlink;
    string linktarget;
    friend class System;
    Node(string name, bool isDirectory, Node* parent = nullptr) {
        this->name = name;
        this->isDirectory = isDirectory;
        this->content = "";
        this->firstChild = nullptr;
        this->nextSibling = nullptr;
        this->parent = parent;
        this->createdat = time(nullptr);
        this->modifiedat = time(nullptr);
        this->filesize = 0;
        this->owner = "";
        this->permission = 7;
        this->issymlink = false;
        this->linktarget = "";
    }
    Node(Node* n) {
        this->name = n->name;
        this->isDirectory = n->isDirectory;
        this->content = n->content;
        this->firstChild = n->firstChild;
        this->nextSibling = n->nextSibling;
        this->parent = n->parent;
        this->createdat = time(nullptr);
        this->modifiedat = n->modifiedat;
        this->filesize = n->filesize;
        this->owner = n->owner;
        this->permission = n->permission;
        this->issymlink = n->issymlink;
        this->linktarget = n->linktarget;
    }
};
string toLowerCase(const string& input) {
    string result = input;
    for (char& c : result) {
        if (c >= 'A' && c <= 'Z') {
            c = c + ('a' - 'A');
        }
    }
    return result;
}
class System {
    Node* root;
    Node* current;


    bool splitpath(string path,vector<string>& parts) {
        bool check = false;
        if (path[0] == '/') {
            path = path.substr(1, path.size());
            check = true;
        }
        if (path.size()>0 && path[path.size() - 1] == '/') {
            path = path.substr(0, path.size()-1);
        }
        string s = "";
        for (int i = 0; i < path.size(); i++) {
            if (path[i] != '/') {
                s += path[i];
            }
            else {
                parts.push_back(s);
                s = "";
            }
        }
        parts.push_back(s);
        return check;
    }
    Node* getNode(string path) {
        if (path == "")
            return current;
        Node* n;
        vector<string> parts;
        if (splitpath(path, parts)) {
            path = path.substr(1, path.size());
            n = root;
        }
        else {
            n = current;
        }
        int offset = 0;
        if (parts[parts.size() - 1] == "txt") {
            offset = 1;
        }
        for (int i = 0; i < parts.size()-offset; i++) {
            if (n->permission != 4 && n->permission != 5 &&
                n->permission != 6 && n->permission != 7)
            {
                cout << "Permission Denied. Read Operation Prohibited for : "<< getpwd(n) << endl;
                return nullptr;
            }
            if (n->name == parts[parts.size()-(1+offset)]) {
                return n;
            }
            if (n->issymlink) {
                n = getNode(n->linktarget);
            }
            n = getchild(n, parts[i]);
            if (!n) {
                cout << "**The path chain is broken or incorrect**" << endl;
                return n;
            }
        }
        if (!n)
            cout << "**The path chain is broken or incorrect**" << endl;
        return n;

    }
    Node* igetNode(string path) {
        if (path == "")
            return current;
        Node* n;
        vector<string> parts;
        if (splitpath(path, parts)) {
            path = path.substr(1, path.size());
            n = root;
        }
        else {
            n = current;
        }
        int offset = 0;
        if (parts[parts.size() - 1] == "txt") {
            offset = 1;
        }
        for (int i = 0; i < parts.size() - offset; i++) {
            if (n->name == parts[parts.size() - (1 + offset)]) {
                return n;
            }
            if (n->issymlink) {
                n = getNode(n->linktarget);
            }
            n = getchild(n, parts[i]);
            if (!n) {
                cout << "**The path chain is broken or incorrect**" << endl;
                return n;
            }
        }
        if (!n)
            cout << "**The path chain is broken or incorrect**" << endl;
        return n;

    }
    Node* getchild(Node* n, string name) {
        Node* child = n->firstChild;
        while (child != nullptr && child->name != name) {
            child = child->nextSibling;
        }
        return child;
    }
    string getpwd(Node* n) {
        string path = "";
        while (n != nullptr) {
            path = "/" + n->name + path;
            n = n->parent;
        }
        path = path.substr(1, path.size() - 1);
        return path;
    }
    void rm_depth(Node* n) {
        if (n == nullptr)
            return;
        Node* parent = n;
        n = n->firstChild;
        Node* nsb;
        while (n != nullptr) {
            nsb = n->nextSibling;
            rm_depth(n);
            n = nsb;
        }
        delete parent;
    }
    void cp_depth(Node* k) {
        if (k == nullptr)
            return;
        Node* parent = k;
        k = k->firstChild ? new Node(k->firstChild) : nullptr;
        parent->firstChild = k;
        while (k != nullptr) {
            k->parent = parent;
            cp_depth(k);
            Node* csb = k->nextSibling ? new Node(k->nextSibling) : nullptr;;
            k->nextSibling = csb;
            k = csb;
        }
    }
    void print(Node* n, int k) {
        if (n == nullptr)
            return;
        int ck = k;
        k = k * 2;
        for (; k > 0; k--) cout << ' ';
        if (n->issymlink)
            cout << "*" << n->name<<"`" << endl;
        else if (n->isDirectory)
            cout << n->name << "/" << endl;
        else
            cout << "[F]" << n->name << endl;
        n = n->firstChild;
        while (n != nullptr) {
            print(n, ck + 1);
            n = n->nextSibling;
        }

    }
    void search_n(Node* n, vector<Node*>& nodes, string sub) {
        if (n == nullptr)
            return;
        if (n->name.find(sub) != string::npos) {
            nodes.push_back(n);
        }
        n = n->firstChild;
        while (n != nullptr) {
            search_n(n,nodes,sub);
            n = n->nextSibling;
        }

    }
    void search_c(Node* n, vector<Node*>& nodes, string sub) {
        if (n == nullptr)
            return;
        if (n->content.find(sub) != string::npos) {
            nodes.push_back(n);
        }
        n = n->firstChild;
        while (n != nullptr) {
            search_c(n, nodes, sub);
            n = n->nextSibling;
        }

    }
    void search_n_i(Node* n, vector<Node*>& nodes, string sub) {
        if (n == nullptr)
            return;
        string name = toLowerCase(n->name);
        if (name.find(sub) != string::npos) {
            nodes.push_back(n);
        }
        n = n->firstChild;
        while (n != nullptr) {
            search_n_i(n, nodes, sub);
            n = n->nextSibling;
        }
    }

    void serializenode(Node* node, ofstream& out) {
        if (!node) return;

        out << (node->name.empty() ? "\"\"" : node->name) << '\n';
        out << node->isDirectory << '\n';
        out << (node->content.empty() ? "\"\"" : node->content) << '\n';
        out << node->createdat << '\n';
        out << node->modifiedat << '\n';
        out << node->filesize << '\n';
        out << (node->owner.empty() ? "\"\"" : node->owner) << '\n';
        out << node->permission << '\n';
        out << node->issymlink << '\n';
        out << (node->linktarget.empty() ? "\"\"" : node->linktarget) << '\n';

        out << (node->firstChild != nullptr) << '\n';
        out << (node->nextSibling != nullptr) << '\n';

        serializenode(node->firstChild, out);
        serializenode(node->nextSibling, out);
    }
    Node* deserializenode(ifstream& in, Node* parent = nullptr) {

        string name, content, owner, linktarget, temp;
        bool isDirectory, issymlink, hasChild, hasSibling;
        size_t filesize;
        unsigned int permission;
        time_t createdat, modifiedat;

        if (!getline(in, name)) return nullptr;
        if (name == "\"\"") name = "";

        if (!getline(in, temp)) return nullptr;
        isDirectory = stoi(temp);

        if (!getline(in, content)) return nullptr;
        if (content == "\"\"") content = "";

        if (!getline(in, temp)) return nullptr;
        createdat = stol(temp);
        if (!getline(in, temp)) return nullptr;
        modifiedat = stol(temp);
        if (!getline(in, temp)) return nullptr;
        filesize = stoul(temp);

        if (!getline(in, owner)) return nullptr;
        if (owner == "\"\"") owner = "";

        if (!getline(in, temp)) return nullptr;
        permission = stoi(temp);
        if (!getline(in, temp)) return nullptr;
        issymlink = stoi(temp);

        if (!getline(in, linktarget)) return nullptr;
        if (linktarget == "\"\"") linktarget = "";

        if (!getline(in, temp)) return nullptr;
        hasChild = stoi(temp);
        if (!getline(in, temp)) return nullptr;
        hasSibling = stoi(temp);

        Node* node = new Node(name, isDirectory, parent);
        node->content = content;
        node->createdat = createdat;
        node->modifiedat = modifiedat;
        node->filesize = filesize;
        node->owner = owner;
        node->permission = permission;
        node->issymlink = issymlink;
        node->linktarget = linktarget;

        if (hasChild) {
            node->firstChild = deserializenode(in, node);
        }
        if (hasSibling) {
            node->nextSibling = deserializenode(in, parent);
        }

        return node;
    }
    bool exits(string name, Node* n) {
        if (!n) return false;
        n = n->parent->firstChild;
        while (n) {
            if (n->name == name) return true;
            n = n->nextSibling;
        }
        return false;
    }

public:
    System() {
        root = new Node("", true);
        current = root;
    }
    void mkdir(string path) {
        bool redundency = true;
        vector<string> parts;
        Node* n;
        if (splitpath(path, parts)) {
            n = root;
        }
        else {
            n = current;
        }
        Node* prev = n;
        for (int i = 0; i < parts.size(); i++) {
            n = getchild(n, parts[i]);
            if (!n) {
                redundency = false;
                n = new Node(parts[i], true, prev);
                n->nextSibling = prev->firstChild;
                prev->firstChild = n;
                n->parent = prev;
            }
            prev = n;
        }
        string created_path = "";
        while (n) {
            created_path = "/"+n->name+created_path;
            n = n->parent;
        }
        created_path = created_path.substr(1, created_path.size());
        cout << (redundency==true? "already existed :" : "created :") << created_path << "" << endl;
    }
    void touch(string path, string content) {
        bool redundency = true;
        vector<string> parts;
        Node* n;
        if (splitpath(path, parts)) {
            n = root;
        }
        else {
            n = current;
        }
        Node* prev = n;
        for (int i = 0; i < parts.size(); i++) {
            n = getchild(n, parts[i]);

            if (!n) {
                if (i == parts.size() - 1) {
                    redundency = false;
                    n = new Node(parts[i], false, prev);
                    n->content = content;
                }
                else {
                    redundency = false; // if the chain is broken , the upper if condition to this one will automatically be executed and will make the redundency false , so there is no need for redundency = false; here.
                    n = new Node(parts[i], true, prev);
                }
                n->nextSibling = prev->firstChild;
                prev->firstChild = n;
                n->parent = prev;
            }
            else {
                if (i == parts.size() - 1) {
                    n->content = content;
                    n->modifiedat = time(nullptr);
                }
                if (n->permission != 2 && n->permission != 3 &&
                    n->permission != 6 && n->permission != 7)
                {
                    cout << "Permission Denied. Overwrite Operation Prohibited" << endl;
                    return;
                }
            }
            prev = n;
        }
        string created_path = "";
        while (n) {
            created_path = "/" + n->name + created_path;
            n = n->parent;
        }
        created_path = created_path.substr(1, created_path.size());
        cout << (redundency?"updated : ": "created : ")<< created_path << "" << endl;
    }
    void ls(string path = "") {
        Node* n;
        if (path == "") {
            if (current->permission != 4 && current->permission != 5 &&
                current->permission != 6 && current->permission != 7)
            {
                cout << "Permission Denied. Read Operation Prohibited" << endl;
                return;
            }
            cout << getpwd(current)<<"/" << endl;
            n = current->firstChild;

        }
        else {
            n = getNode(path);
            if (!n)
                return;
            cout << getpwd(n)<<"/" << endl;
            n = n->firstChild;
        }
        while (n != nullptr) {
            if (current->permission != 4 && current->permission != 5 &&
                current->permission != 6 && current->permission != 7)
            {
                cout << "Permission Denied. Read Operation Prohibited" << endl;
                return;
            }
            cout << (n->isDirectory ? "   [DIR]  " + n->name : "   [FILE] " + n->name) << endl;
            n = n->nextSibling;
        }
    }
    void cd(string path) {
        if (path == "/..") {
            if (current->parent) {
                current = current->parent;
               
            }
            return;
        }
        if (getNode(path))
            current = getNode(path);
    }
    void pwd() {
        Node* n = current;
        string path = "";
        while (n != nullptr) {
            path = "/"+ n->name+path;
            n = n->parent;
        }
        path = path.substr(1, path.size() - 1);
        cout << path << "> ";
    }
    void cat(string path) {
        Node* n = getNode(path);
        if (n == nullptr || n->isDirectory) {
            cout << "**The path is not a readable file**" << endl;
        }
        else {
            if (n->permission != 4 && n->permission != 5 &&
                n->permission != 6 && n->permission != 7)
            {
                cout << "Permission Denied. Read Operation Prohibited" << endl;
                return;
            }
            cout << "(" << n->content << ")" << endl;
        }
    }
    void rm(string path) {
        Node* n = getNode(path);
        if (n == root) {
            cout << "root is not a File and Not permitted to Remove the root" << endl;
            return;
        }
        if (n == nullptr) {
            cout << "No Folder or File could be found with Specified name" << endl;
            return;
        }
        if (n != nullptr && n->isDirectory) {
            cout << "The Node is not a File" << endl;
            return;
        }
        Node* k = n->parent->firstChild;
        if (n == k) {
            n = n->nextSibling;
            k->parent->firstChild = n;
            delete k;
            return;
        }
        while (k != nullptr ) {
            if (k->nextSibling == n) {
                k->nextSibling = n->nextSibling;
                delete n;
                return;
            }
            k = k->nextSibling;
        }
    }
    void mv(string path1, string path2) {
        // takes the last node at path1 and make it first child of path2
        bool check_absoulte = false;
        vector<string> parts1;
        Node* n1;
        if (splitpath(path1,parts1)) {
            check_absoulte = true;
        }
        else {
            check_absoulte = false;
        }
        vector<string> parts2;
        Node* n2;
        if (splitpath(path2, parts2)) {
            if (!check_absoulte) {
                cout << "*Both paths must be relative or absolute*" << endl;
                return;
            }
        }
        else {
            if (check_absoulte) {
                cout << "*Both paths must be relative or absolute*" << endl;
                return;
            }
        }
        n1 = getNode(path1);
        n2 = getNode(path2);
        if (n1 == nullptr || n2 == nullptr) {
            cout << "Invalid paths , eiher chain is broken or typo mistake" << endl;
            return;
        }
        Node* k = n1->parent->firstChild;
        if (n1 == k) { // if n1 is the firstchild
            k->parent->firstChild = n1->nextSibling;
        }
        else {
            while (k != nullptr) {
                if (k->nextSibling == n1) {
                    k->nextSibling = n1->nextSibling;
                }
                k = k->nextSibling;
            }
        }
        n1->nextSibling = n2->firstChild;
        n2->firstChild = n1;
        n1->parent = n2;
    }
    void cp(string path1, string path2) {
        // copies the entire last node at path1 and make it first child of path2
        bool check_absoulte = false;
        vector<string> parts1;
        Node* n1;
        if (splitpath(path1, parts1)) {
            check_absoulte = true;
        }
        else {
            check_absoulte = false;
        }
        vector<string> parts2;
        Node* n2;
        if (splitpath(path2, parts2)) {
            if (!check_absoulte) {
                cout << "*Both paths must be relative or absolute*" << endl;
                return;
            }
        }
        else {
            if (check_absoulte) {
                cout << "*Both paths must be relative or absolute*" << endl;
                return;
            }
        }
        n1 = getNode(path1);
        n2 = getNode(path2);
        if (n1 == nullptr || n2 == nullptr) {
            cout << "Invalid paths , eiher chain is broken or typo mistake" << endl;
            return;
        }
        if (exits(n1->name, n2->firstChild)) {
            cout << "Duplicate files , Data Redundency is not allowed" << endl;
            return;
        }

        Node* k = new Node(n1); // if i had created Node k(n1); it would have been destroyed when we return
        k->nextSibling = nullptr; // breaking the chain of copied node because we only want that node copy from the chain
        cp_depth(k); // copying the inner nodes
        k->nextSibling = n2->firstChild;
        n2->firstChild = k;
        k->parent = n2;
    }
    void rename(string path, string newname) {
        Node* n = getNode(path);
        vector<string> parts;
        splitpath(newname,parts);
        newname = parts[0];
        if (!n)
        {
            cout << "No such file or Directory exits" << endl;
            return;
        }
        if (n->permission != 2 && n->permission != 3 &&
            n->permission != 6 && n->permission != 7)
        {
            cout << "Permission Denied. Overwrite Operation Prohibited" << endl;
            return;
        }
        n->name = newname;
        n->modifiedat = time(nullptr);
    }
    void rmdir(string path) {
        Node* n = getNode(path);
        if (n == root)
        {
            cout << "You cannot Delete the root" << endl;
            return;
        }
        Node* k = n->parent->firstChild;
        if (n == k) {
            n = n->nextSibling;
            k->parent->firstChild = n;
            rm_depth(k);
            return;
        }
        while (k != nullptr) {
            if (k->nextSibling == n) {
                k->nextSibling = n->nextSibling;
                rm_depth(n);
                return;
            }
            k = k->nextSibling;
        }
        
    }
    void ln_s(string path, string link) {
        Node* n = getNode(path);
        if (n->firstChild) {
            cout << "The directory is a folder with sub_folders, cannot generate a symbolic link" << endl;
            return;
        }
        if (n->permission != 2 && n->permission != 3 &&
            n->permission != 6 && n->permission != 7)
        {
            cout << "Permission Denied. Overwrite Operation Prohibited" << endl;
            return;
        }
        n->issymlink = true;
        n->linktarget = link;
    }
    void stat(string path) {
        Node* n = getNode(path);
        if (!n) return;
        cout << getpwd(n) << " {" << endl
            << "created : " << n->createdat << endl
            << "modified : " << n->modifiedat << endl
            << "file size : " << n->filesize << endl
            << "is symbolic link :" << n->issymlink << endl
            << "target link :" << n->linktarget << endl
            << "owner : " << n->owner << endl
            << "permissions : " << n->permission << endl
            << "}"<<endl;
    }
    void chmod(string path, int permission) {
        Node* n = igetNode(path);
        n->permission = permission;
        n->modifiedat = time(nullptr);
    }
    void chown(string path, string owner) {
        Node* n = getNode(path);
        if (n->permission != 2 && n->permission != 3 &&
            n->permission != 6 && n->permission != 7)
        {
            cout << "Permission Denied. Overwrite Operation Prohibited" << endl;
            return;
        }
        n->owner = owner;
        n->modifiedat = time(nullptr);
    }
    void printtree() {
        cout << endl << endl << endl;
        Node* n = root;
        print(n, 0);
        cout << endl << endl << endl;
    }
    void search_pattren(string path, string sub) {
        Node* n = igetNode(path);
        if (!n) {
            cout << "No such file or directory exits given in path" << endl;
            return;
        }
        vector<Node*> nodes;
        search_n(n, nodes, sub);
        cout <<"search by name : " << "{" << endl;
        for (int i = 0; i < nodes.size(); i++) {
            cout <<"(" << i + 1 << ") " << getpwd(nodes[i]) << endl;
        }
        cout << "{" << endl;
    }
    void search_pattren_i(string path, string sub) {
        sub = toLowerCase(sub);
        Node* n = igetNode(path);
        if (!n) {
            cout << "No such file or directory exits given in path" << endl;
            return;
        }
        vector<Node*> nodes;
        search_n_i(n, nodes, sub);
        cout << "search by name : " << "{" << endl;
        for (int i = 0; i < nodes.size(); i++) {
            cout << "(" << i + 1 << ") " << getpwd(nodes[i]) << endl;
        }
        cout << "{" << endl;
    }
    void grep(string path, string sub) {
        Node* n = igetNode(path);
        if (!n) {
            cout << "No such file or directory exits given in path" << endl;
            return;
        }
        vector<Node*> nodes;
        search_c(n, nodes, sub);
        cout << "search by name : " << "{" << endl;
        for (int i = 0; i < nodes.size(); i++) {
            cout << i + 1 << " " << getpwd(nodes[i]) << "   (" << nodes[i]->content<<")" << endl;
        }
        cout << "{" << endl;
    }
    void savetofile(string filename) {
        ofstream out(filename, ios::binary);
        if (!out) {
            cerr << "*Unable to open file for saving*" << endl;
            return;
        }
        serializenode(root, out);
        out.close();
        cout << "File system saved to: " << filename << endl;
    }
    void loadfromfile(string filename) {
        cleanup();
        ifstream in(filename, ios::binary);
        if (!in) {
            cerr << "Error: Unable to open file for loading." << endl;
            return;
        }
        root = deserializenode(in);
        current = root;
        in.close();
        cout << "File system loaded from " << filename << endl;
    }
    void cleanup() {
        Node* n = root->firstChild;
        while (n) {
            Node* nsb = n->nextSibling;
            if (n->isDirectory) {
                rmdir(getpwd(n));
            }
            else {
                rm(getpwd(n));
            }
            n = nsb;
        }
    }
    void startCLI() {
        vector<string> parts;
        while (1) {
            pwd();
            string command;
            getline(cin, command);
            splitcommand(command, parts);
            executecommand(parts);
        }
    }
    void executecommand(vector<string>v) {
        try {
            if (v[0] == "mkdir") {
                if (v.size() == 2)
                    mkdir(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "touch") {
                if (v.size() == 3)
                    touch(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "ls") {
                if (v.size() == 2)
                    ls(v[1]);
                else if (v.size() == 1)
                    ls();
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "cd") {
                if (v.size() == 2)
                    cd(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "pwd") {
                if (v.size() == 1) {
                    pwd();
                    cout << endl;
                }
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "cat") {
                if (v.size() == 2)
                    cat(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "rm") {
                if (v.size() == 2)
                    rm(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "mv") {
                if (v.size() == 3)
                    mv(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "cp") {
                if (v.size() == 3)
                    cp(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "rename") {
                if (v.size() == 3)
                    rename(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "rmdir") {
                if (v.size() == 2)
                    rmdir(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "ln_s") {
                if (v.size() == 3)
                    ln_s(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "stat") {
                if (v.size() == 2)
                    stat(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "chmod") {
                if (v.size() == 3)
                    chmod(v[1], stoi(v[2]));
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "chown") {
                if (v.size() == 3)
                    chown(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "sear_nm") {
                if (v.size() == 3)
                    search_pattren(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "sear_nmi") {
                if (v.size() == 3)
                    search_pattren_i(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "grep") {
                if (v.size() == 3)
                    grep(v[1], v[2]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "svf") {
                if (v.size() == 2)
                    savetofile(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "ldf") {
                if (v.size() == 2)
                    loadfromfile(v[1]);
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "ht") {
                if (v.size() == 2 && v[1] == "-u")
                    printtree();
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "cln") {
                if (v.size() == 2 && v[1] == "-u")
                    cleanup();
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "cls") {
                if (v.size() == 1)
                    system("cls");
                else
                    cout << "[command not valid]" << endl;
            }
            else if (v[0] == "exit") {
                if (v.size() == 1)
                    exit(0);
                else
                    cout << "[command not valid]" << endl;
            }
            else {
                cout << "[command not found]" << endl;
            }
        }
        catch (const exception& e) {
            cerr << "Error [][][][][]: " << e.what() << endl;
        }
    }
    void splitcommand(string s, vector<string>& v) {
        v.clear();
        bool check = false;
        string sub = "";
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '~')
                check = true;
            else if (s[i] != ' ' || check) sub += s[i];
            else if (!check){
                v.push_back(sub);
                sub = "";
            }
        }
        if (sub != "") v.push_back(sub);
    }
};
void maint() {
    System s;

    cout << "=== Test Case 1: Creating Directories ===" << endl;
    s.mkdir("/home");
    s.mkdir("/home/user");
    s.mkdir("/docs");
    s.touch("/cridentials.txt","User name and password");
    s.cat("cridentials.txt");
    s.rm("docs");
    s.ls("/");
    s.printtree();
    s.mv("cridentials.txt", "home/user");
    s.printtree();
    s.mv("/home/user/cridentials.txt", "/");
    s.cp("/home", "/docs");
    s.cd("/docs/home/user");
    s.cp("/cridentials.txt", "/docs/home/user");
    s.mkdir("/aplha/client");
    s.printtree();
    s.pwd();
    s.rename("cridentials.txt", "azz.txt");
    s.mv("/docs/home/user/azz.txt", "/aplha/client/");
    s.rename("home", "zair");
    s.ln_s("/home/user", "/aplha/");
    s.printtree();
    s.cd("/home/user/client");
    s.pwd();
    s.printtree();
    s.cat("/home/user/client/azz.txt");
    s.stat("/home/user/client/azz.txt");
    s.chmod("/home/user/client/azz.txt", 4);
    s.chown("/home/user/client/azz.txt", "zair");
    s.stat("/home/user/client/azz.txt");
    s.stat("/home/user");
    s.search_pattren_i("/", ".txt");
    s.savetofile("data.txt");
    
    cout << "*****************************************" << endl;
    s.printtree();
    s.mkdir("/home/user");
    s.printtree();
    s.touch("/cridentials.txt", "hello cheema");
    s.printtree();
    s.pwd();
    s.cd("/");
    s.cat("cridentials.txt");
}
void maintt() {
    System s;
    s.loadfromfile("data.txt");
    s.printtree();
    s.mv("/cridentials.txt", "/docs/home/user/");
    s.printtree();
    s.chmod("/docs/home/", 4);
    s.cd("/docs/home/user/");
    s.touch("/docs/home/user/cridentials.txt", "changed");
    s.cat("/docs/home/user/cridentials.txt");
    //s.chown("/docs/home/", "zair");
    s.ls();
    s.pwd();
}
void maink() {
    System s;
    s.mkdir("/home");
    s.touch("/home/dip.txt", "helloworld");
    s.cat("/home/dip.txt");
    s.loadfromfile("data.txt");
    s.printtree();
}
void main() {
    System s;
    s.startCLI();
}
