#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <iomanip>

using namespace std;

struct SetHasher {
    int operator()(const set<int> &V) const {
        int hash = V.size();
        for(auto &i : V) {
            hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

void printSet(const set<int> &V){
    for(auto const & i: V){
        cout<<i<<",";
    }
}

void writeSet(ofstream &ofs, const set<int> &S){
    ofs<<*S.begin();
    for(auto it = ++S.begin(); it != S.end(); ++it){
        ofs<<","<<*it;
    }
}

int getInputFileAndC1(ifstream &ifs, vector<set<int>> &transactionTable, unordered_map<set<int>, int, SetHasher> &itemset){ // return transactions number
    //get input file
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
        exit(EXIT_FAILURE);
    }
    int transactionsCount = 0;
    string temp;
    while ( getline (ifs,temp) ){
        set<int> transaction;
        stringstream ss(temp);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            int item = stoi(substr);
            set<int> itemSet{item};
            itemset[itemSet]++;
            transaction.insert(item);
        }
        transactionTable.push_back(transaction);
        ++transactionsCount;
    }
    ifs.close();
    return transactionsCount;
}

void writeOutputFile(const int &transactionNumber, ofstream &ofs, const unordered_map<set<int>, int, SetHasher> &itemset){
    for(auto &item: itemset){
        writeSet(ofs, item.first);
        ofs<<":"<<fixed<<setprecision(4)<<(float)item.second/transactionNumber<<"\n";
    }
}

void candidateToLargeItemset(const int & minSupport, unordered_map<set<int>, int, SetHasher> &itemset){
    unordered_map<set<int>, int, SetHasher>::iterator it;
    for(it = itemset.begin(); it != itemset.end();){
        if(it->second < minSupport){
            it = itemset.erase(it);
        }
        else{
            ++it;
        }
    }
}

int main(int argc, char *argv[]) {
    double minSupportRate = 0;
    int transactionNumber = 0, minSupport = 0;
    vector<set<int>> transactionTable;

    if(argc != 4){
        cout<<"arguments number is wrong!\n";
        exit(-1);
    }

    minSupportRate = stod(argv[1]); //get input minimum support
    ifstream ifs(argv[2]);
    ofstream ofs(argv[3]);

    unordered_map<set<int>, int, SetHasher> itemset;
    transactionNumber = getInputFileAndC1(ifs, transactionTable, itemset);
    minSupport = minSupportRate*transactionNumber;
    cout<<"minSupprt:"<<minSupport<<"\n";

    candidateToLargeItemset(minSupport, itemset);
    writeOutputFile(transactionNumber, ofs, itemset);

    




    



    //write output file

    ofs.close();
    return 0;
}