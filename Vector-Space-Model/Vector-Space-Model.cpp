// WAHAJ JAVED ALAM (20K-0208) BCS-6A
// USED THREADS JUST FOR THE SAKE OF GUI
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<ctype.h>
#include<string>
#include<stdlib.h>
#include<algorithm>
#include<stdlib.h>
#include<thread>
#include <locale>
#include <codecvt>
#include<cmath>
#include<Windows.h>
#include "OleanderStemmingLibrary-master/src/english_stem.h"
using namespace std;

class VectorIndex{
    public:
        vector<vector<float>> vectors;
    public:
        VectorIndex(){
            vectors.resize(30);
        }
};
//constants for similarity scores
const float ALPHA = 0.34;
float threshold = 0;
// Oleander Stemming Library
stemming::english_stem<> StemEnglish;
// converter for wstring to string
wstring_convert<std::codecvt_utf8<wchar_t>> converter;
// file path to the data set directory
string inputFilePath="Dataset/";
string outputFilePath="Dictionary.txt";
string tokenFilePath="Tokens.txt";
// input and output file streams
ifstream inputFileStream;
ofstream outputFileStream;
// map to determine if a token is a stopword
map<string,bool> stopwords;
// dictionary consisting of all document vectors
VectorIndex dictionary;
// list of tokens for storing the dictionary into a file in document processing
vector<string> tokens;
map<string,int> documentFrequency;
// a variable to stop the progress bar thread
bool dictionaryRead = false;
// function to read the stopwords into a map of strings to boolean
void processStopwords(){
    inputFileStream.open("Stopword-List.txt");
    string line,fileContents;
    while(inputFileStream>>line){
        stopwords[line] = true;
    }
    cout<<"Stopwords Processed"<<endl;
    inputFileStream.close();
}

// check if the given token is a stopword by matching from dictionary
bool isStopword(string token){
    return stopwords[token];
}
// clean the token by removing all extra characters
void normalizeToken(string &token){
    string normalizedToken = "";
    for(int i=0;i<token.length();i++){
        if(isalpha(token[i]) || isdigit(token[i])){
            normalizedToken += token[i];
        }
    }
    token = normalizedToken;
}
// lower case the token
void casefoldToken(string &token){
    string caseFoldedToken = "";
    for(int i=0;i<token.length();i++){
        if(isalpha(token[i])){
            caseFoldedToken += tolower(token[i]);
        }else{
            caseFoldedToken += token[i];
        }
    }
    token = caseFoldedToken;
}
// write the dictionary to a file in the format <term,documentFrequency,termFrequency;docID:positions;>
void writeDictionaryToFile(){
    outputFileStream.open(tokenFilePath);
    for(int i=0;i<tokens.size();i++){
        outputFileStream<<tokens[i]<<":"<<documentFrequency[tokens[i]]<<endl;
    }
    outputFileStream.close();
    outputFileStream.open(outputFilePath);
    for(int i=0;i<dictionary.vectors.size();i++){
        outputFileStream<<"<Document"<<i+1<<":";
        for(int j=0;j<dictionary.vectors[i].size();j++){
            outputFileStream<<dictionary.vectors[i][j];
            if(j != dictionary.vectors[i].size()-1){
                outputFileStream<<",";
            }
        }
        outputFileStream<<">";
        outputFileStream<<endl;
    }
    outputFileStream.close();
}
// read the dictionary from the created file previously
void readDictionaryFromFile(){
    inputFileStream.open(tokenFilePath);
    string line;
    while(inputFileStream>>line){
        string token="";
        string freq="";
        int i=0;
        while(line[i] != ':'){
            token += line[i];
            i++;
        }
        i++;
        tokens.push_back(token);
        documentFrequency[token] = stoi(line.substr(i));
    }
    inputFileStream.close();
    inputFileStream.open(outputFilePath);
    int docNum = 0;
    while(inputFileStream>>line){
        string token="";
        int i=1;
        while(line[i] != ':'){
            i++;
        }
        i++;
        while(line[i]){
            token = "";
            while(line[i] != ',' && line[i] != '>'){
                token += line[i];
                i++;
            }
            i++;    
            dictionary.vectors[docNum].push_back(stof(token));
            if(line[i] == '>'){
                break;
            }
        }
        docNum++;
        i++;
    }
    inputFileStream.close();
}
// function to check if a token has a number in it
bool hasNumber(string text){
    for(int i=0;i<text.length();i++){
        if(isdigit(text[i])){
            return true;
        }
    }
    return false;
}
// function to process documents
void processDocuments(){
    map<string,vector<pair<int,int>>> postingList;
    for(int i=1;i<=30;i++){
        map<string,bool> markedTokens;
        string path = inputFilePath + to_string(i) + ".txt";
        inputFileStream.open(path);
        string line;
        while(inputFileStream>>line){
            if(!isStopword(line) && !hasNumber(line)){
                
                normalizeToken(line);
                if(line == ""){
                    continue;
                }
                casefoldToken(line);
                wstring word = converter.from_bytes(line);
                StemEnglish(word);
                line = converter.to_bytes(word);
                if(postingList[line].empty()){
                    tokens.push_back(line);
                }    
                if(!markedTokens[line]){
                    documentFrequency[line] +=1;
                    markedTokens[line] = true;
                }
                bool found = false;
                // check if the token is already present in the posting list
                // if yes, increment the term frequency
                // else, add the token to the posting list
                for(int j=0;j<postingList[line].size();j++){
                    if(postingList[line][j].first == i){
                        found = true;
                        postingList[line][j].second++;
                        break;
                    }
                }
                if(!found){
                    postingList[line].push_back(make_pair(i,1));
                }
            }else{
                continue;
            }
        }
        inputFileStream.close();
    }
    sort(tokens.begin(),tokens.end());
    for(int i=0;i<tokens.size();i++){
        int j=0,k=1;
        while(j<postingList[tokens[i]].size() && k <= 30){
            if(postingList[tokens[i]][j].first == k){
                float idf = log(30.0/(float)documentFrequency[tokens[i]]);
                dictionary.vectors[k-1].push_back(postingList[tokens[i]][j].second * idf);
                j++;
                k++;
            }else{
                dictionary.vectors[k-1].push_back(0);
                k++;
            }
        }
        while(k<=30){
            dictionary.vectors[k-1].push_back(0);
            k++;
        }
    }
}
// print the whole dictionary and indexes
void printDictionary(){
    for(int i=0;i<dictionary.vectors.size();i++){
        cout<<"<"<<"Document"<<i+1<<":";
        for(int j=0;j<dictionary.vectors[i].size();j++){
            cout<<dictionary.vectors[i][j];
            if(j != dictionary.vectors[i].size()-1){
                cout<<",";
            }
        }
        cout<<">"<<endl;
    }
}
// function to process query token
void processQueryToken(string &token){
    if(!isStopword(token)){
        normalizeToken(token);
        if(token == ""){
            return;
        }
        casefoldToken(token);
        wstring word = converter.from_bytes(token);
        StemEnglish(word);
        token = converter.to_bytes(word);
    }else{
        return;
    }
}
// function to generate the query vectorfrom the query
vector<float> generateQueryVector(vector<string> query){
    map<string,int> queryMap;
    for(int i=0;i<query.size();i++){
        if(queryMap[query[i]] == 0){
            queryMap[query[i]] = 1;
        }else{
            queryMap[query[i]]++;
        }
    }
    vector<float> queryVector;
    for(int i=0;i<tokens.size();i++){
        if(queryMap[tokens[i]]){
            float idf = log(30.0/(float)documentFrequency[tokens[i]]);
            queryVector.push_back(queryMap[tokens[i]] * idf);
        }else{
            queryVector.push_back(0);
        }
    }
    return queryVector;
}
// function to calculate cosine similarity
vector<pair<int,float>> calculateCosineSimilarities(vector<float> &queryVector){
    vector<pair<int,float>> result;
    float numerator = 0;
    float denominator1 = 0;
    float denominator2 = 0;
    for(int docNum=0;docNum < dictionary.vectors.size();docNum++){
        for(int i=0;i<queryVector.size();i++){
            numerator += queryVector[i] * dictionary.vectors[docNum][i];
            denominator1 += queryVector[i] * queryVector[i];
            denominator2 += dictionary.vectors[docNum][i] * dictionary.vectors[docNum][i];
        }
        float cosineSimilarity = numerator / (sqrt(denominator1) * sqrt(denominator2));
        result.push_back(pair(docNum+1,cosineSimilarity));
        numerator = 0;
        denominator1 = 0;
        denominator2 = 0;
    }
    return result;
}
// function to rank documents based on cosine similarity
vector<int> rankDocuments(vector<pair<int,float>> &cosineSimilarities){
    sort(cosineSimilarities.begin(),cosineSimilarities.end(),[](pair<int,float> &a,pair<int,float> &b){
        return a.second > b.second;
    });
    threshold = ALPHA * cosineSimilarities[0].second;
    vector<int> result;
    for(int i=0;i<cosineSimilarities.size();i++){
        if(cosineSimilarities[i].second == 0|| cosineSimilarities[i].second < threshold || isnan(cosineSimilarities[i].second)){
            continue;
        }
        result.push_back(cosineSimilarities[i].first);
    }
    return result;
}
// process the whole query and return results
vector<int> processQuery(string query){
    vector<string> terms;
    string token="";
    for(int i=0;i<query.length();i++){
        if(query[i] == ' '){
            processQueryToken(token);
            if(token == ""){
                continue;
            }
            terms.push_back(token);
            token="";
            continue;
        }
        token += query[i];
    }
    processQueryToken(token);
    if(token != ""){
        terms.push_back(token);
    }
    sort(terms.begin(),terms.end()); 
    vector<float> queryVector = generateQueryVector(terms);
    
    vector<pair<int,float>> cosineSimilarities = calculateCosineSimilarities(queryVector);
    
    vector<int> rankedDocuments = rankDocuments(cosineSimilarities);
    return rankedDocuments;
}
void loading_screen_start(){
    cout << "\n\n\t\t\t\t Vector Space Model\n";
    cout << "\t\t\t\t Created by: k200208 Wahaj Javed Alam \n\n\t\t\t\t";
    printf("%c", 219);
    for (int a = 1; a < 40; a++){
        Sleep(60);
        printf("%c", 219);
    }
    Sleep(30);
    system("cls");
}
void loadingDictionary(string text){
        system("cls");
        cout << "\n\n\t\t\t\t "<<text<<" ...\n\n\n\t\t\t\t";
        printf("%c", 219);
        int counter = 0;
        while (!dictionaryRead){
            Sleep(30);
            printf("%c", 219);
            counter++;
            if(counter == 30){
                counter = 0;
                system("cls");
                cout << "\n\n\t\t\t\t "<<text<<" ...\n\n\n\t\t\t\t";
                printf("%c", 219);
            }
        }
        Sleep(30);
        system("cls");
}
// a function that reads the dictionary if it is already created else creates it
void LoadDictionary(){
    processStopwords();
    inputFileStream.open(outputFilePath);
    if(!inputFileStream){
        inputFileStream.close();
        thread th1(loadingDictionary,"Parsing Dataset");
        thread th2(processDocuments);
        th2.join();
        dictionaryRead = true;
        th1.join();
        dictionaryRead = false;
        thread th3(loadingDictionary,"Writing To The File Dictionary.txt");
        thread th4(writeDictionaryToFile);
        th4.join();
        dictionaryRead = true;
        th3.join();
    }else{
        inputFileStream.close();
        thread th1(loadingDictionary,"Reading Dictionary");
        thread th2(readDictionaryFromFile);
        th2.join();
        dictionaryRead = true;
        th1.join();
    }
}
void homeScreen(){
    string query;
    while(true){
        cout << "\t\t"<<"Enter Your Query (0 to Exit): ";
        getline(cin,query);
        if(query == "0"){
            system("cls");
            cout << "\n\n\t\t "<<"Hoping that the Results were accurate..."<<endl;
            cout<<"\n\n\t\t Exiting"<<endl;
            exit(0);
        }
        vector<int> result = processQuery(query);
        cout<<endl<<endl;
        if(result.size() == 0){
            cout<<"\t\tThere Are no documents that match this query ";
        }else{
            cout<<"\t\tRetrieved Documents: ";
            for(int i=0;i<result.size();i++){
                cout<<result[i];
                if(i != result.size()-1){
                    cout<<", ";
                }
            }
        }
        cout<<endl;
        cout<<"\t\tEnter R to Refresh Page ... ";
        cin>>query;
        system("cls");
        cin.ignore();
    }
    
}
int main(){
    system("cls");
    loading_screen_start();
    LoadDictionary();
    homeScreen();
}