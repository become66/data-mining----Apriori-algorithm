#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

void getInputFile(ifstream &ifs, vector<pair<set<int>, double>> &FPtable){ 
    //get input file
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
        exit(EXIT_FAILURE);
    }
    string FP;
    while ( getline (ifs,FP) ){
        stringstream ss(FP);
        string FPsetStr, supportStr;
        set<int> FPset;
        getline(ss, FPsetStr, ':');
        getline(ss, supportStr);
        double support = stod(supportStr);
        ss.clear();
        ss.str(FPsetStr);  // re-init with s1
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            int item = stoi(substr);
            FPset.insert(item);
        }
        FPtable.push_back(make_pair(FPset, support));
    }
    ifs.close();
}

void writeSet(ofstream &ofs, const set<int> &S){
    ofs<<*S.begin();
    for(auto it = ++S.begin(); it != S.end(); ++it){
        ofs<<","<<*it;
    }
}

void writeOutputFile(ofstream &ofs, vector<pair<set<int>, double>> &FPtable){
    for(auto rule: FPtable){
        writeSet(ofs, rule.first);
        ofs<<":"<<rule.second<<"\n";
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        cout<<"arguments number is wrong!\n";
        exit(-1);
    }
    ifstream ifs(argv[1]);
    ofstream ofs(argv[2]);
    vector<pair<set<int>, double>> FPtable;
    getInputFile(ifs, FPtable);
    sort(FPtable.begin(), FPtable.end());
    writeOutputFile(ofs, FPtable);


    return 0;
}