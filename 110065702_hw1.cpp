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
        //prune
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

void scan(vector<set<int>> &transactionTable, unordered_map<set<int>, int, SetHasher> &itemsetTable){
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

void splitVector(vector<set<int>> &transactionTable, vector<set<int>> &transactionTable1, vector<set<int>> &transactionTable2){
    size_t const half_size = transactionTable.size() / 2;
    vector<set<int>> TransactionTable1(transactionTable.begin(), transactionTable.begin() + half_size);
    swap(transactionTable1, TransactionTable1);
    vector<set<int>> TransactionTable2(transactionTable.begin() + half_size, transactionTable.end());
    swap(transactionTable2, TransactionTable2);
}



void scanV2(vector<set<int>> &transactionTable1, vector<set<int>> &transactionTable2, unordered_map<set<int>, int, SetHasher> &itemsetTable){
    thread thread1( scan, ref(transactionTable1), ref(itemsetTable) );
    thread thread2( scan, ref(transactionTable2), ref(itemsetTable) );
    thread1.join();
    thread2.join();
}

int main(int argc, char *argv[]) {
    clock_t start, End;
    double cpu_time_used;
    start = clock();




    double minSupportRate = 0;
    int transactionNumber = 0, minSupport = 0;
    vector<set<int>> transactionTable;
    vector<set<int>> transactionTable1, transactionTable2;

    if(argc != 4){
        cout<<"arguments number is wrong!\n";
        exit(-1);
    }

    minSupportRate = stod(argv[1]); //get input minimum support
    ifstream ifs(argv[2]);
    ofstream ofs(argv[3]);

    unordered_map<set<int>, int, SetHasher> itemsetTable;
    transactionNumber = getInputFileAndC1(ifs, transactionTable, itemsetTable); //generate C1
    splitVector(transactionTable, transactionTable1, transactionTable2);
    transactionTable.clear();

    minSupport = minSupportRate*transactionNumber;
    // cout<<"minSupprt:"<<minSupport<<"\n";
    candidateToLargeItemset(minSupport, itemsetTable); //generate L1
    int Lcount = 1;
    while(!itemsetTable.empty()){
        cout<<"L"<<Lcount<<"\n";
        ++Lcount;
        // printItemsetTable(itemsetTable);
        writeOutputFile(transactionNumber, ofs, itemsetTable);
        largeToNextcandidateItemset(itemsetTable);
        // scan(transactionTable, itemsetTable);
        scanV2(transactionTable1, transactionTable2, itemsetTable);
        candidateToLargeItemset(minSupport, itemsetTable);
    }

    ofs.close();


    End = clock();
    cpu_time_used = ((double) (End - start)) / CLOCKS_PER_SEC;
    cout<<"cpu_time_used: "<<cpu_time_used<<"\n";
    return 0;
}