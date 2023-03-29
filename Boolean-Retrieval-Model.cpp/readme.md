# BOOLEAN RETRIEVAL MODEL IN C++
# CODER: WAHAJ JAVED ALAM (20K-0208)
# COURSE: INFORMATION RETRIEVAL

## How to use the model
### compile the code
- g++ -o brm BooleanRetrievalModel.cpp
### run the code
- ./brm.exe
### Dictionary and Dataset
- If the Dictionary already exists, it will be loaded from the file
- If it doesnt exist, the program creates one dictionary

### Queries and their Results are shown here:
Query 1) cricket AND captain
Results => 1, 3, 6, 9, 14, 20, 21, 22, 25, 29
Query 2) goOd AND ChasE
Results => 2, 16, 27, 28, 30
Query 3) pCb OR Psl
Results => 4, 11, 29
Query 4) ground and statement and board
Results => 17  
Query 5) not ImPossiBle
Results => 1, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
Query 6) international cricket /1
Results => 11, 12, 14, 29
Query 7)  replacement players /9
Results => 10