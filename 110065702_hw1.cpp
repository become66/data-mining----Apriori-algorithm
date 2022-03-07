#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <iomanip>

using namespace std;

struct VectorHasher {
    int operator()(const vector<int> &V) const {
        int hash = V.size();
        for(auto &i : V) {
            hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

void printVec(const vector<int> &V){
    for(auto const & i: V){
        cout<<i<<",";
    }
}

void writeVec(ofstream &ofs, const vector<int> &V){
    ofs<<V[0];
    for(size_t idx = 1; idx < V.size(); ++idx){
        ofs<<","<<V[idx];
    }
}

int getInputFileAndC1(ifstream &ifs, vector<vector<int>> &transactionTable, unordered_map<vector<int>, int, VectorHasher> &itemset){ // return transactions number
    //get input file
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
        exit(EXIT_FAILURE);
    }
    int transactionsCount = 0;
    string temp;
    while ( getline (ifs,temp) ){
        vector<int> transaction;
        stringstream ss(temp);
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            int item = stoi(substr);
            vector<int> itemVector{item};
            itemset[itemVector]++;
            transaction.push_back(item);
        }
        transactionTable.push_back(transaction);
        ++transactionsCount;
    }
    ifs.close();
    return transactionsCount;
}

void writeOutputFile(const int &transactionNumber, ofstream &ofs, const unordered_map<vector<int>, int, VectorHasher> &itemset){
    for(auto &item: itemset){
        writeVec(ofs, item.first);
        ofs<<":"<<fixed<<setprecision(4)<<(float)item.second/transactionNumber<<"\n";
    }
}

void candidateToLargeItemset(const int & minSupport, unordered_map<vector<int>, int, VectorHasher> &itemset){
    unordered_map<vector<int>, int, VectorHasher>::iterator it;
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
    vector<vector<int>> transactionTable;

    if(argc != 4){
        cout<<"arguments number is wrong!\n";
        exit(-1);
    }

    minSupportRate = stod(argv[1]); //get input minimum support
    ifstream ifs(argv[2]);
    ofstream ofs(argv[3]);

    unordered_map<vector<int>, int, VectorHasher> itemset;
    transactionNumber = getInputFileAndC1(ifs, transactionTable, itemset);
    minSupport = minSupportRate*transactionNumber;
    cout<<"minSupprt:"<<minSupport<<"\n";

    candidateToLargeItemset(minSupport, itemset);
    writeOutputFile(transactionNumber, ofs, itemset);

    




    



    //write output file

    ofs.close();
    return 0;
}