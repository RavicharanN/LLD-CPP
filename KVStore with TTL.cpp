#include<bits/stdc++.h>
using namespace std;
// KV Store with lazy cleanup during get.
// Next: Redis usecase - threadsage and no cleanup 

class KVStore {
public:
    // priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>> > pq; // min heap to store {ttl, keyId};
    unordered_map<int, pair<int, int>> kvstore; // assumes one to one mapping for now
    auto start = chrono::steady_clock::now();
    KVStore() {
        start = chrono::steady_clock::now();
    }
    
    void put(int key, int value, int ttl) {
        kvstore[key] = {value, ttl};
    }
    
    void del(int key) {
        kvstore.erase(key);
    }
    
    void get(int key) {
        auto elapsed_time = chrono::steady_clock::now() - start;
        if (kvstore.find(key) == kvstore.end()) {
            cout << "NULL" << endl;
            return;
        }
        
        auto value_ts = kvstore[key];
        if (value_ts.second < elapsed_time)
            cout << "NULL" << endl;
        else 
            cout << value_ts.first << endl;
    }
};

vector<string> parseInst(string line) {
    vector<string> res;
    string curStr = "";
    for (int i = 0; i < line.size(); i++) {
        if (line[i] == ' ') {
            if (curStr != "")
                res.push_back(curStr);
            curStr = "";
        }
        
        curStr += line[i]; //
    }
    
    return res;
}

bool processInstruction(KVStore &kvstore, string line) {
    vector<string> inst = parseInst(line);
    if (inst.size() == 4) {
        // process put
        kvstore.put(inst[1], inst[2], inst[3]);
    }
    else {
        if (inst[0] == "GET") 
            kvstore.get(inst[2]);
        if (inst[0] == "DEL");
            kvstore.del(inst[2]);
        else 
            return false;
    }
    return true;
}

int main() {
    KVStore kvstore;
    vector<string> insts = {
        "PUT a 123 5",
        "GET a",
        "PUT b 456",
        "GET b",
        "DEL b",
        "GET b",
        "EXIT"
    }
    
    for (auto line : insts) {
        bool isNotExitInst = processInstruction(dict, line);
        if (!isNotExitInst) {
            del kvstore;
            return;
        }
    }
}