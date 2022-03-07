#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <utility>

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

void printVec(vector<int> V){
    for(auto const & i: V){
        cout<<i<<" ";
    }
}

int getInputFileAndC1(ifstream &ifs, vector<vector<int>> &transactionTable, unordered_map<vector<int>, int, VectorHasher> &candidateItemset){ // return transactions number
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
            candidateItemset[itemVector]++;
            transaction.push_back(item);
        }
        transactionTable.push_back(transaction);
        ++transactionsCount;
    }
    ifs.close();
    return transactionsCount;
}

void writeLineOutputFile(ofstream &ofs){

}

void candidateToLargeItemset(const int & minSupport, unordered_map<vector<int>, int, VectorHasher> &candidateItemset){
    unordered_map<vector<int>, int, VectorHasher>::iterator it;
    for(it = candidateItemset.begin(); it != candidateItemset.end();){
        if(it->second < minSupport){
            it = candidateItemset.erase(it);
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

    unordered_map<vector<int>, int, VectorHasher> candidateItemset;
    unordered_map<vector<int>, int, VectorHasher> largeItemset;
    transactionNumber = getInputFileAndC1(ifs, transactionTable, candidateItemset);
    minSupport = minSupportRate*transactionNumber;
    cout<<"minSupprt:"<<minSupport<<"\n";

    candidateToLargeItemset(minSupport, candidateItemset);
    for(auto &item: candidateItemset){
        printVec(item.first);
        cout<<": "<<item.second<<"\n";
    }

    




    



    //write output file

    ofs.close();
    return 0;
}