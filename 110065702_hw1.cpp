#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

void getInputFile(ifstream &ifs, vector<string> &transactions){
    //get input file
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
        exit(EXIT_FAILURE);
    }
    string temp;
    while ( getline (ifs,temp) ){
        transactions.push_back(temp);
    }
    ifs.close();
}

void writeLineOutputFile(ofstream &ofs, const string &str){
    ofs<<str<<"\n";
}

int main(int argc, char *argv[]) {
    double min_support = 0;
    vector<string> transactions;

    if(argc != 4){
        cout<<"arguments number is wrong!\n";
    }

    min_support = stod(argv[1]); //get input minimum support
    ifstream ifs(argv[2]);
    ofstream ofs(argv[3]);

    getInputFile(ifs, transactions);





    



    //write output file
    for(auto str: transactions){
        writeLineOutputFile(ofs, str);
    }
    ofs.close();
    return 0;
}