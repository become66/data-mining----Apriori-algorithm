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
#include <algorithm>
#include <ctime>
#include <thread>
#include <mutex>

using namespace std;
#define vectorSplitNum 100

mutex myMutex;

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

int getInputFileAndC1(ifstream &ifs, vector<set<int>> &transactionTable, unordered_map<set<int>, int, SetHasher> &itemsetTable){ // return transactions number
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
            itemsetTable[itemSet]++;
            transaction.insert(item);
        }
        transactionTable.push_back(transaction);
        ++transactionsCount;
    }
    ifs.close();
    return transactionsCount;
}

void writeOutputFile(const int &transactionNumber, ofstream &ofs, const unordered_map<set<int>, int, SetHasher> &itemsetTable){
    for(auto &item: itemsetTable){
        writeSet(ofs, item.first);
        ofs<<":"<<fixed<<setprecision(4)<<(float)item.second/transactionNumber<<"\n";
    }
}

void candidateToLargeItemset(const int & minSupport, unordered_map<set<int>, int, SetHasher> &itemsetTable){
    unordered_map<set<int>, int, SetHasher>::iterator it;
    for(it = itemsetTable.begin(); it != itemsetTable.end();){
        if(it->second < minSupport){
            it = itemsetTable.erase(it);
        }
        else{
            ++it;
        }
    }
}

bool canPrune(const set<int> &itemset, unordered_map<set<int>, int, SetHasher> &itemsetTable){
    set<int> temp = itemset;
    for(auto &i:temp){
        temp.erase(i);
        if(itemsetTable.find(temp) == itemsetTable.end()){
            return true;
        }
        else{
            temp.insert(i);
        }
    }   
    return false;
}

void largeToNextcandidateItemset(unordered_map<set<int>, int, SetHasher> &itemsetTable){
    unordered_map<set<int>, int, SetHasher> candidateItemsetTable;
    if(itemsetTable.begin()->first.size() == 1){ //L1
        for(auto &firstItemsetRecord: itemsetTable){
            for(auto &secondItemsetRecord: itemsetTable){
                if(firstItemsetRecord == secondItemsetRecord)
                    continue;
                set<int> newItemSet;
                newItemSet.insert(*firstItemsetRecord.first.begin());
                newItemSet.insert(*secondItemsetRecord.first.begin());
                candidateItemsetTable[newItemSet] = 0;
            }
        }
    }
    else{
        for(auto &firstItemsetRecord: itemsetTable){
            for(auto &secondItemsetRecord: itemsetTable){
                if(firstItemsetRecord == secondItemsetRecord)
                    continue;
                //join two set (if can join)
                set<int> firstItemSet = firstItemsetRecord.first, secondItemSet = secondItemsetRecord.first;
                int firstEnd = *firstItemSet.rbegin(), secondEnd = *secondItemSet.rbegin();
                firstItemSet.erase(firstEnd);
                secondItemSet.erase(secondEnd);
                if(firstItemSet == secondItemSet){
                    firstItemSet.insert(firstEnd);
                    firstItemSet.insert(secondEnd);
                    candidateItemsetTable[firstItemSet] = 0;
                }
            }
        }
        // prune
        // unordered_map<set<int>, int, SetHasher>::iterator it;
        // for(it = candidateItemsetTable.begin(); it != candidateItemsetTable.end();){
        //     if(canPrune(it->first, itemsetTable)){
        //         it = candidateItemsetTable.erase(it);
        //     }
        //     else{
        //         ++it;
        //     }
        // }
    }
    swap(itemsetTable, candidateItemsetTable);
}

bool firstContainSecond(const set<int> &A, const set<int> &B){
    return includes(A.begin(), A.end(), B.begin(), B.end());
}

void threadScan(vector<set<int>> &transactionTable, unordered_map<set<int>, int, SetHasher> &itemsetTable){
    for(auto &itemset: itemsetTable){
        for(auto &transaction: transactionTable){
            if(firstContainSecond(transaction, itemset.first)){
                myMutex.lock();
                itemsetTable[itemset.first]++;
                myMutex.unlock();
            }
        }
    }
}

void printItemsetTable(unordered_map<set<int>, int, SetHasher> &itemsetTable){
    for(auto &item: itemsetTable){
        printSet(item.first);
        cout<<":"<<item.second<<"\n";
    }
}


template<typename T>
std::vector<std::vector<T>> SplitVector(const std::vector<T>& vec, size_t n)
{
    std::vector<std::vector<T>> outVec;

    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < std::min(n, vec.size()); ++i)
    {
        end += (remain > 0) ? (length + !!(remain--)) : length;

        outVec.push_back(std::vector<T>(vec.begin() + begin, vec.begin() + end));

        begin = end;
    }

    return outVec;
}

void scan(vector<vector<set<int>>> &manyTransactionTable, unordered_map<set<int>, int, SetHasher> &itemsetTable){
    thread threads[vectorSplitNum];
    for (int i = 0; i < vectorSplitNum; i++) {
        threads[i] = thread(threadScan, ref(manyTransactionTable[i]), ref(itemsetTable));
    }
    for (int i = 0; i < vectorSplitNum; i++) {
        threads[i].join();
    }   
}

int main(int argc, char *argv[]) {
    // clock_t start, End;
    // double cpu_time_used;
    // start = clock();




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

    unordered_map<set<int>, int, SetHasher> itemsetTable;
    transactionNumber = getInputFileAndC1(ifs, transactionTable, itemsetTable); //generate C1
    vector<vector<set<int>>> manyTransactionTable = SplitVector(transactionTable, vectorSplitNum);
    transactionTable.clear();

    minSupport = minSupportRate*transactionNumber;
    // cout<<"minSupprt:"<<minSupport<<"\n";
    candidateToLargeItemset(minSupport, itemsetTable); //generate L1
    // int Lcount = 1;
    while(!itemsetTable.empty()){
        // cout<<"L"<<Lcount<<"\n";
        // ++Lcount;
        // printItemsetTable(itemsetTable);
        writeOutputFile(transactionNumber, ofs, itemsetTable);
        largeToNextcandidateItemset(itemsetTable);
        scan(manyTransactionTable , itemsetTable);
        candidateToLargeItemset(minSupport, itemsetTable);
    }

    ofs.close();


    // End = clock();
    // cpu_time_used = ((double) (End - start)) / CLOCKS_PER_SEC;
    // cout<<"cpu_time_used: "<<cpu_time_used<<"\n";
    return 0;
}