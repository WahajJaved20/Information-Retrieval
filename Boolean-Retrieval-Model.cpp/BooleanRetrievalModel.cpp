// WAHAJ JAVED ALAM (20K-0208) BCS-6A
// ADDED A PREFIX FILE BY MYSELF FOR STEMMING
// USED THREADS JUST FOR THE SAKE OF GUI
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<ctype.h>
#include<string>
#include<stdlib.h>
#include<algorithm>
#include<stack>
#include<set>
#include<stdlib.h>
#include<windows.h>
#include<thread>
using namespace std;

// Class to Store the Document Frequency and Positional Indexes for each token
class InvertedIndexContents{
    private:
        int documentFrequency;
        int termFrequency;
        vector<pair<int,vector<int>>> positionalIndex;
        void incrementDocumentFrequency(){
            documentFrequency++;
        }
        void incrementTermFrequency(){
            termFrequency++;
        }
    public:
        InvertedIndexContents(){
            documentFrequency = 0;
            termFrequency = 0;
        }
        int getDocumentFrequency() const {
            return documentFrequency;
        }
        int getTermFrequency() const {
            return termFrequency;
        }
        void setDocumentFrequency(int frequency){
            documentFrequency = frequency;
        }
        void setTermFrequency(int frequency){
            termFrequency = frequency;
        }
        vector<pair<int,vector<int>>> getPositionalIndex() const {
            return positionalIndex;
        }
        void setPositionalIndex(vector<pair<int,vector<int>>> pi){
            positionalIndex = pi;
        }
        void appendDocumentPosition(int docID,int position){
            incrementTermFrequency();
            for(int i=0;i<positionalIndex.size();i++){
                if(positionalIndex[i].first == docID){
                    positionalIndex[i].second.push_back(position);
                    return;
                }
            }
            positionalIndex.push_back(pair<int,vector<int>>(docID,vector<int>()));
            incrementDocumentFrequency();
            positionalIndex[positionalIndex.size()-1].second.push_back(position);
        }
};
vector<char> vowels = {'a','e','i','o','u'};
// file path to the data set directory
string inputFilePath="Dataset/";
string outputFilePath="Dictionary.txt";
// input and output file streams
ifstream inputFileStream;
ofstream outputFileStream;
// vector to store prefixes
vector<string> prefixes;
// map to determine if a token is a stopword
map<string,bool> stopwords;
// dictionary mapping from token to the inverted index
map<string,InvertedIndexContents> dictionary;
// list of tokens for storing the dictionary into a file in document processing
vector<string> tokens;
// a variable to stop the progress bar thread
bool dictionaryRead = false;
// read prefixes from the file
void readPrefixes(){
    inputFileStream.open("Prefix-List.txt");
    string line,fileContents;
    while(inputFileStream>>line){
       prefixes.push_back(line);
    }
    cout<<"Prefixes Processed"<<endl;
    inputFileStream.close();
}
// convert operators to lowercase and return them
string lowerCase(string opera){
    for(int i=0;i<opera.length();i++){
        opera[i] = tolower(opera[i]);
    }
    return opera;
}
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
// remove prefixes from the token
void removePrefix(string token){
    for(int i=0;i<prefixes.size();i++){
        if(token.substr(0,prefixes[i].length()) == prefixes[i]){
            if(token != prefixes[i]){
                token =  token.substr(prefixes[i].length());
            }
            break;
        }
    }
}
// helper function to check if a suffix exists and remove it from the token
int removeSuffix(string &token,string suffix){
    size_t exists = token.rfind(suffix);
    if(exists != string::npos && exists == token.length()-suffix.length()){
        if(token != suffix){
            token =  token.substr(0, exists);
            return 1;
        }
    }
    return 0;
}
// Porter Stemmer implemented by me
void stemmizeToken(string &token){
    removePrefix(token);
    //Suffix Trim 1
    size_t exists = token.rfind("sses");
    if(exists != string::npos && exists == token.length() - 4){
        removeSuffix(token,"es");
    } 
    exists = token.rfind("ies");
    if(exists != string::npos && exists == token.length() - 3){
        removeSuffix(token,"es");
    }
    if(token.length() > 1 && token[token.length()-2] != 's'){
        removeSuffix(token,"s");
    }
    exists = token.rfind("eed");
    bool vowelFound = false;
    if(exists != string::npos && exists == token.length() - 3){
        for(int i=0;i<token.size();i++){
            for(int j=0;j<vowels.size();j++){
                if(token[i] == vowels[j]){
                    removeSuffix(token,"ed");
                    vowelFound = true;
                    break;
                }
            }
            if(vowelFound){
                break;
            }
        }
    }
    vowelFound = false;
    if(token.length() != 1 && token[token.length()-1] == 'y'){
        for(int i=0;i<token.size();i++){
            for(int j=0;j<vowels.size();j++){
                if(token[i] == vowels[j]){
                    vowelFound = true;
                    break;
                }
            }
            if(vowelFound){
                break;
            }
        }
        if(vowelFound){
            token[token.length()-1] = 'i';
        }
        vowelFound = false;
    }
    if(token.length() > 4){
        for(int i=0;i<vowels.size();i++){
            if(token[token.length()-4]== vowels[i]){
                vowelFound = true;
            }
        }
        if(!vowelFound){
            removeSuffix(token,"ing");
        }
        vowelFound = false;
    }
    //Suffix Trim 2

    if(removeSuffix(token,"tional")){
        token += "tion";
    }
    if(removeSuffix(token,"ization")){
        token += "ize";
    }
    if(removeSuffix(token,"fulness")){
        token += "ful";
    }
    if(removeSuffix(token,"iveness")){
        token += "ive";
    }
    if(removeSuffix(token,"ousness")){
        token += "ous";
    }
    if(removeSuffix(token,"ousli")){
        token += "ous";
    }
    if(removeSuffix(token,"entli")){
        token += "ent";
    }
    if(removeSuffix(token,"biliti")){
        token += "ble";
    }
    // Suffix Trim 3
    if(removeSuffix(token,"icate")){
        token += "ic";
    }
    removeSuffix(token,"ative");
    if(removeSuffix(token,"alize")){
        token += "al";
    }
    if(removeSuffix(token,"iciti")){
        token += "ic";
    }
    if(removeSuffix(token,"ical")){
        token += "ic";
    }
    removeSuffix(token,"ful");
    if(removeSuffix(token,"iveness")){
        token += "ive";
    }
    removeSuffix(token,"ness");
    // Suffix Trim 4
    removeSuffix(token, "al");
    removeSuffix(token, "ance");
    removeSuffix(token, "ence");
    removeSuffix(token, "er");
    removeSuffix(token, "ic");
    removeSuffix(token,"ed");
    removeSuffix(token, "able");
    removeSuffix(token, "ible");
    removeSuffix(token, "ant");
    removeSuffix(token, "ement");
    removeSuffix(token, "ment");
    removeSuffix(token, "ent");
    removeSuffix(token, "sion");
    removeSuffix(token, "tion");
    removeSuffix(token, "ou");
    removeSuffix(token, "ism");
    if(token.length()> 4){
        removeSuffix(token, "ate");
    }
    
    removeSuffix(token, "iti");
    removeSuffix(token, "ous");
    removeSuffix(token, "ive");
    removeSuffix(token, "ize");
    removeSuffix(token, "ise");
    // Suffix Trim 5
    if(token.length() != 1 && token[token.length()-1] == 'e'){
        for(int j=0;j<vowels.size();j++){
            if(token[token.length()-2] == vowels[j]){
                vowelFound = true;
                break;
            }
        }
        if(!vowelFound){
            removeSuffix(token,"e");
        }
        vowelFound = false;
    }
}
// write the dictionary to a file in the format <term,documentFrequency,termFrequency;docID:positions;>
void writeDictionaryToFile(){
    outputFileStream.open(outputFilePath);
    for(int i=0;i<tokens.size();i++){
        vector<pair<int,vector<int>>> list = dictionary[tokens[i]].getPositionalIndex();
        outputFileStream<<"<"<<tokens[i]<<","<<dictionary[tokens[i]].getDocumentFrequency()<<",";
        outputFileStream<<dictionary[tokens[i]].getTermFrequency()<<";";
        for(int j=0;j<list.size();j++){
            outputFileStream<<list[j].first<<":";
            for(int k=0;k<list[j].second.size();k++){
                outputFileStream<<list[j].second[k];
                if(k != list[j].second.size()-1){
                    outputFileStream<<",";
                }
            }
            outputFileStream<<";";
            
        }
        outputFileStream<<">"<<endl;
    }
    outputFileStream.close();
}
// read the dictionary from the created file previously
void readDictionaryFromFile(){
    inputFileStream.open(outputFilePath);
    if(!inputFileStream){
        cout<<"Cannot Open File"<<endl;
    }
    string line;
    while(inputFileStream>>line){
        string token="";
        string documentFrequency="";
        string termFrequency="";
        int i=1;
        while(line[i] != ','){
            token += line[i];
            i++;
        }
        i++;
        while(line[i]!=','){
            documentFrequency+=line[i];
            i++;
        }
        i++;
        while(line[i]!=';'){
            termFrequency+=line[i];
            i++;
        }
        i++;
        tokens.push_back(token);
        string docID="";
        while(line[i] != '>'){
            string position="";
            while(line[i]!=':'){
                docID += line[i];
                i++;
            }
            i++;
            while(line[i] != ';'){
                position += line[i];
                i++;
                if(line[i] == ','){
                    i++;
                    dictionary[token].appendDocumentPosition(stoi(docID),stoi(position));
                    position = "";
                }
            }
            dictionary[token].appendDocumentPosition(stoi(docID),stoi(position));
            dictionary[token].setTermFrequency(stoi(termFrequency));
            docID="";
            i++;
        }
    }
    inputFileStream.close();
}
// function to process documents
void processDocuments(){
    for(int i=1;i<=30;i++){
        string path = inputFilePath + to_string(i) + ".txt";
        inputFileStream.open(path);
        string line;
        int position = 0;
        while(inputFileStream>>line){
            if(!isStopword(line)){
                normalizeToken(line);
                if(line == ""){
                    continue;
                }
                casefoldToken(line);
                stemmizeToken(line);
                position++;
                if(!dictionary[line].getDocumentFrequency()){
                    tokens.push_back(line);
                }
                dictionary[line].appendDocumentPosition(i,position);
            }else{
                continue;
            }
        }
        inputFileStream.close();
    }
    sort(tokens.begin(),tokens.end());
    for(int i=0;i<tokens.size();i++){
        vector<pair<int,vector<int>>> pi = dictionary[tokens[i]].getPositionalIndex();
        sort(pi.begin(),pi.end());
        for(int j=0;j<pi.size();j++){
            sort(pi[j].second.begin(),pi[j].second.end());
        }
        dictionary[tokens[i]].setPositionalIndex(pi);
    }
}
// print the whole dictionary and indexes
void printDictionary(){
    for(int i=0;i<tokens.size();i++){
        vector<pair<int,vector<int>>> list = dictionary[tokens[i]].getPositionalIndex();
        cout<<"<"<<tokens[i]<<","<<dictionary[tokens[i]].getDocumentFrequency()<<",";
        cout<<dictionary[tokens[i]].getTermFrequency()<<";";
        for(int j=0;j<list.size();j++){
            cout<<list[j].first<<":";
            for(int k=0;k<list[j].second.size();k++){
                cout<<list[j].second[k];
                if(k != list[j].second.size()-1){
                    cout<<",";
                }
            }
            cout<<";";
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
        stemmizeToken(token);
    }else{
        return;
    }
}

// extract document IDs from a posting list
vector<int> extractDocumentIDs(vector<pair<int,vector<int>>> postingList){
    vector<int> docIDs;
    for(int i=0;i<postingList.size();i++){
        docIDs.push_back(postingList[i].first);
    }
    return docIDs;
}
// function to intersect/AND the posting lists
vector<int> intersectPostingLists(vector<int> posting1,vector<int> posting2){
    vector<int> intersection;
    int i=0,j=0;
    while(i<posting1.size() && j<posting2.size()){
        if(posting1[i] == posting2[j]){
            intersection.push_back(posting1[i]);
            i++;
            j++;
        }else if(posting1[i] < posting2[j]){
            i++;
        }else{
            j++;
        }
    }
    return intersection;
}
// function to union/OR the posting lists
vector<int> unionPostingLists(vector<int> posting1,vector<int> posting2){
    set<int> unions;
    int i=0,j=0;
    while (i<posting1.size()){
        unions.insert(posting1[i]);
        i++;
    }
    while(j<posting2.size()){
        unions.insert(posting2[j]);
        j++;
    }
    return vector<int>(unions.begin(),unions.end());
}
// function to negate/NOT the posting list
vector<int> negatePostingList(vector<int> posting){
    vector<int> negation;
    int j=0;
    for(int i=1;i<=30;i++){
        if(j<posting.size()){
            if(i != posting[j]){
                negation.push_back(i);
            }else{
                j++;
            }
        }else{
           negation.push_back(i);
        }
    }
    return negation;
}
// function to intersect/AND the posting lists by positional indexes under given proximity
vector<int> positionalIntersect(vector<pair<int,vector<int>>> pi1,vector<pair<int,vector<int>>> pi2,int proximity){
    set<int> positions;
    int i=0,j=0;
    while(i < pi1.size() && j < pi2.size()){
        if(pi1[i].first == pi2[j].first){
            vector<int> result;
            vector<int> pos1=pi1[i].second;
            vector<int> pos2=pi2[j].second;
            int k=0,l=0;
            while(k < pos1.size()){
                while(l < pos2.size()){
                    if(abs(pos1[k] - pos2[l]) <= proximity){
                        result.push_back(pos2[l]);
                    }else if(pos2[l] > pos1[k]){
                        break;
                    }
                    l++;
                }
                while(!result.empty() && abs(result[0]-pos1[k]) > proximity){
                    result.erase(result.begin());
                }
                if(!result.empty()){
                    positions.insert(pi1[i].first);
                }
                k++;
            }
            i++;
            j++;
        }else if(pi1[i].first < pi2[j].first){
            i++;
        }else{
            j++;
        }
    }
    return vector<int>(positions.begin(),positions.end());
}
// perform positional intersect but in terms of phrases i.e: all words are at a distance of 1
vector<pair<int,vector<int>>> phrasalIntersect(vector<pair<int,vector<int>>> pi1,vector<pair<int,vector<int>>> pi2){
    vector<pair<int,vector<int>>> positions;
    const int proximity = 1;
    int i=0,j=0;
    while(i < pi1.size() && j < pi2.size()){
        if(pi1[i].first == pi2[j].first){
            vector<int> result;
            vector<int> pos1=pi1[i].second;
            vector<int> pos2=pi2[j].second;
            int k=0,l=0;
            while(k < pos1.size()){
                while(l < pos2.size()){
                    if(abs(pos1[k] - pos2[l]) <= proximity){
                        result.push_back(pos2[l]);
                    }else if(pos2[l] > pos1[k]){
                        break;
                    }
                    l++;
                }
                while(!result.empty() && abs(result[0]-pos1[k]) > proximity){
                    result.erase(result.begin());
                }
                positions.push_back(pair<int,vector<int>>(pi1[i].first,result));
                k++;
            }
            i++;
            j++;
        }else if(pi1[i].first < pi2[j].first){
            i++;
        }else{
            j++;
        }
    }
    return positions;
}
// process proximity Tokens
vector<int> proximityToken(stack<string> &tokens,stack<vector<int>> &res,string token){
    string proximity="";
    for(int i=1;i<token.length();i++){
        proximity += token[i];
    }
    token = tokens.top();
    tokens.pop();
    processQueryToken(token);
    vector<pair<int,vector<int>>> positionalList1 = dictionary[token].getPositionalIndex();
    token = tokens.top();
    tokens.pop();
    processQueryToken(token);
    vector<pair<int,vector<int>>> positionalList2 = dictionary[token].getPositionalIndex();
    vector<int> result = positionalIntersect(positionalList1,positionalList2,stoi(proximity));
    return result;
}
// process NOT tokens
vector<int> NOTToken(stack<string> &tokens,stack<vector<int>> &res){
    vector<int> posting = res.top();
    res.pop();
    vector<int> result;
    if(!tokens.empty() && lowerCase(tokens.top()) == "and"){
        tokens.pop();
        string token = tokens.top();
        tokens.pop();
        processQueryToken(token);
        vector<int> posting2;
        if(!tokens.empty() && lowerCase(tokens.top()) == "not"){
            tokens.pop();
            posting2 = negatePostingList(extractDocumentIDs(dictionary[token].getPositionalIndex()));
        }else{
            posting2 = extractDocumentIDs(dictionary[token].getPositionalIndex());
        }
        posting = negatePostingList(posting);
        result = intersectPostingLists(posting,posting2);
    }else{
        result = negatePostingList(posting);
    }
    for(int i=0;i<result.size();i++){
        cout<<result[i]<<endl;
    }
    return result;
}
// process AND/OR Tokens
vector<int> ANDORTokens(stack<string> &tokens,stack<vector<int>> &res,bool isAND){
    vector<int> result,posting2;
    vector<int> posting1 = res.top();
    res.pop();
    string token = tokens.top();
    tokens.pop();
    if(token[0] == '/'){
        posting2 = proximityToken(tokens,res,token);
    }else{
        processQueryToken(token);
        if(!tokens.empty() && lowerCase(tokens.top()) == "not"){
            tokens.pop();
            res.push(extractDocumentIDs(dictionary[token].getPositionalIndex()));
            posting2 = NOTToken(tokens,res);
        }else{
            posting2 = extractDocumentIDs(dictionary[token].getPositionalIndex());
        }
    }
    if(isAND){
        result = intersectPostingLists(posting1,posting2);
    }else{
        result = unionPostingLists(posting1,posting2);
    }
    return result;
}
// extract phrases from inverted commas formatted tokens
vector<string> extractPhraseVector(stack<string> &tokens,string &token){
    vector<string> phrases;
    phrases.push_back(token);
    while(!tokens.empty() && (tokens.top())[0] != '"'){
        token = tokens.top();
        tokens.pop();
        phrases.insert(phrases.begin(),token);
    }
    phrases.insert(phrases.begin(),tokens.top());
    tokens.pop();
    return phrases;
}
// extract phrase tokens to work with proximity queries
vector<int> phraseToken(stack<string> &tokens,string &token){
    vector<string> phrases = extractPhraseVector(tokens,token);
    processQueryToken(phrases[0]);
    stack<vector<pair<int,vector<int>>>> positionalLists;
    positionalLists.push(dictionary[phrases[0]].getPositionalIndex());
    for(int i=1;i<phrases.size();i++){
        vector<pair<int,vector<int>>> posting1 = positionalLists.top();
        positionalLists.pop();
        processQueryToken(phrases[i]);
        vector<pair<int,vector<int>>> posting2 = dictionary[phrases[i]].getPositionalIndex();
        vector<pair<int,vector<int>>> res = phrasalIntersect(posting1,posting2);
        positionalLists.push(res);
    }
    vector<int> temp = extractDocumentIDs(positionalLists.top());
    set<int> result;
    for(int i=0;i<temp.size();i++){
        result.insert(temp[i]);
    }
    
    return vector<int>(result.begin(),result.end());
}
// process the whole query and return results
vector<int> processQuery(string query){
    stack<string> tokens;
    stack<vector<int>> res;
    string token="";
    for(int i=0;i<query.length();i++){
        if(query[i] == ' '){
            tokens.push(token);
            token="";
            continue;
        }
        token += query[i];
    }
    tokens.push(token);
    do{
        token = tokens.top();
        tokens.pop();
        if(lowerCase(token) == "and" || lowerCase(token) == "or"){
            bool isAND = false;
            if(lowerCase(token) == "and"){
                isAND = true;
            }
            vector<int> result = ANDORTokens(tokens,res,isAND);
            res.push(result);
        }else if(lowerCase(token) == "not"){
            vector<int> result = NOTToken(tokens,res);
            res.push(result);
        }else if(token[0] == '/'){
            vector<int> result = proximityToken(tokens,res,token);
            res.push(result);
        }else if(token[token.length()-1] == '"'){
            vector<int> result = phraseToken(tokens,token);
            res.push(result);
        }else{
            processQueryToken(token);
            vector<int> docIDs = extractDocumentIDs(dictionary[token].getPositionalIndex());
            res.push(docIDs);
        }
    }while(!tokens.empty());
    vector<int> finalResult = res.top();
    res.pop();
    return finalResult;
}
void loading_screen_start(){
    cout << "\n\n\t\t\t\t Boolean Retrieval Model\n";
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
        while (!dictionaryRead)
        {
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
    readPrefixes();
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
        cout << "\t\tQuery Formats: \n\t\tLogical Queries: t1 AND t2, t1 OR t2, NOT t1"<<endl;
        cout << "\t\tProximity Queries: t1 t2 /k"<<endl;
        cout << "\t\tPhrasal Queries: \"t1 t2 t3 t4 \" "<<endl;
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