#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

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

void getInputFileAndC1(ifstream &ifs, vector<vector<int>> &transactions, unordered_map<vector<int>, int, VectorHasher> &candidateItemset){
    //get input file
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
        exit(EXIT_FAILURE);
    }

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
        transactions.push_back(transaction);
    }
    ifs.close();
}

void writeLineOutputFile(ofstream &ofs){

}


int main(int argc, char *argv[]) {
    double min_support = 0;
    vector<vector<int>> transactions;

    if(argc != 4){
        cout<<"arguments number is wrong!\n";
        exit(-1);
    }

    min_support = stod(argv[1]); //get input minimum support
    ifstream ifs(argv[2]);
    ofstream ofs(argv[3]);

    unordered_map<vector<int>, int, VectorHasher> candidateItemset;
    unordered_map<vector<int>, int, VectorHasher> largeItemset;
    getInputFileAndC1(ifs, transactions, candidateItemset);

    for(auto &i: candidateItemset){
        printVec(i.first);
        cout<<":"<<i.second<<"\n";
    }






    



    //write output file

    ofs.close();
    return 0;
}