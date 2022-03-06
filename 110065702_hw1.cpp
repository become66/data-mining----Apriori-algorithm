#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

void getInputFile(ifstream &ifs, vector<vector<int>> &transactions){
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
            transaction.push_back(stoi(substr));
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

    getInputFile(ifs, transactions);

    for(auto v: transactions){
        for(auto num:v){
            cout<<num<<" ";
        }
        cout<<"\n";
    }




    



    //write output file

    ofs.close();
    return 0;
}