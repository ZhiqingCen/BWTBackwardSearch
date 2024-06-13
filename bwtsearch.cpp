#include <iostream>
#include <fstream>
#include <bitset>
#include <map>
#include <regex>
#include <string>
#include <cstring>
#include <utility>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

// using namespace std;
class BwtSearch {
    public:
        void readfile(int argc, char **argv);
        void search();
        void backwardSearch();
        void test();
    private:
        unsigned int occurance(char c, unsigned int rowNum);
        void substringSearch(unsigned int fr, unsigned int lr);
        void numberOfSearchResults(int fr, int lr, char searchChar);
        int getInteger(unsigned int a, unsigned int b = 0b10000000);
        std::ifstream infile;
        std::ofstream indexfile;
        std::string searchTerm;
        int searchTermLength;
        const int bufferLength = 5120;
        const int blockLength = 2048;
        const int numBit = 7;
        int mask = 0b10000000;
        int numInt = 0;
        unsigned int totalLength;
        std::string bwt;
        std::map <char, unsigned int> cTable;
        std::vector <std::vector<unsigned int>> lTable;
        std::unordered_map <char, unsigned int> lIndex;
        // std::vector <char> lastRow;
        std::map <unsigned int, std::string> searchResults;
        std::string inputfile = "";
        std::string outputfile = "";
};

// Read in the file
void BwtSearch::readfile(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Error: bwtsearch <input_file_path> <index_file_path> <search_term>" << std::endl;
        exit(0);
    }
    infile.open(argv[1]);
    if (!infile) {
        std::cerr << "Error: no such file: " + std::string(argv[1]) << std::endl;
        exit(0);
    }
    if (! std::regex_match(argv[1], std::regex("(.*)(.rlb)"))) {
        std::cerr << "Error: the input file " + std::string(argv[1]) + " is not in rlb format" << std::endl;
        exit(0);
    }
    indexfile.open(argv[2]);
    if (!indexfile) {
        std::cerr << "Error: no such file: " + std::string(argv[2]) << std::endl;
        exit(0);
    }
    searchTerm = argv[3];
    searchTermLength = searchTerm.length();
    if (searchTermLength > 512) {
        std::cerr << "Error: search term more than 512 characters" << std::endl;
        exit(0);
    }
}

void BwtSearch::search() {
    unsigned int countRow = 0; //TODO

    char* substring = new char [bufferLength];
    unsigned int charCount = 0;
    char previous;
    bool prevInt = false;
    // std::vector <unsigned int> uniqueAscii;

    // calculate number of substrings with file size
    infile.seekg(0, std::ios::end);
    std::streampos fileSize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    unsigned int filesize = static_cast<int>(fileSize);
    unsigned int substringCount;
    if (filesize % bufferLength == 0) {
        substringCount = filesize / bufferLength;
    } else {
        substringCount = (filesize / bufferLength) + 1;
    }

        for (unsigned int i = 0; i < substringCount; i++) {
        // get substring length
        infile.read(substring, bufferLength);
        std::streamsize substringLength = infile.gcount();

        // read substring char by char
        for (int i = 0; i < substringLength; i++) {
            if (((unsigned char)substring[i] >> 7) == 1) {
                // integer
                charCount = getInteger(((unsigned char)substring[i] ^ mask));
                while (++i < substringLength && ((unsigned char)substring[i] >> 7) == 1) {
                    charCount = getInteger(charCount, (unsigned char)substring[i]);
                }
                i--;
                prevInt = true;
            } else {
                if (charCount > 0 || prevInt) {
                    // previous char is integer, store in tables and arrays
                     // 3 runs are represented as 0
                    charCount += 2;
                    cTable[previous] += charCount;
                    for (int j = 0; j < charCount; j++) {
                        bwt.push_back(previous);
                    }
                    numInt = 0;
                }
                // char
                charCount = 0;
                prevInt = false;
                previous = substring[i];
                bwt.push_back(previous);
                cTable[previous]++;
            }
        }
        memset(substring, 0, bufferLength);
    }

    unsigned int cCounter = 0;
    totalLength = bwt.size();
    std::vector <unsigned int> uniqueAscii (cTable.size(), 0);
    // get each unique ascii character from cTable
    for (auto& i:cTable) {
        lIndex.insert({i.first, cCounter++});
    }
    
    // store only line == blocklength to save space
    for (unsigned int i = 0; i < totalLength; i++) {
        uniqueAscii[lIndex[bwt[i]]]++;
        if (i % blockLength == 0) {
            lTable.push_back(uniqueAscii);
        }
    }

    // store last line
    if ((totalLength - 1) % blockLength != 0) {
        lTable.push_back(uniqueAscii);
    }

    cCounter = 0;
    bool first = true;
    int pre = 0;
    for (auto& i: cTable) {
        if (first) {
            cCounter = i.second;
            i.second = 0;
            first = false;
        } else {
            pre = i.second;
            i.second = (unsigned int) (cCounter);
            cCounter += pre;
        }
    }

    indexfile.close();
    infile.close();
    delete []substring;
    totalLength = bwt.size();
}

unsigned int BwtSearch::occurance(char c, unsigned int rowNum) {
    if (rowNum % blockLength != 0) {
        unsigned int prevRow = rowNum / blockLength;
        unsigned int counter = lTable[prevRow][lIndex[c]];
        for (prevRow = (prevRow * blockLength) + 1; prevRow != rowNum + 1; prevRow++) {
            if (bwt[prevRow] == c) {
                ++counter;
            }
        }
        return counter;
    }
    return lTable[rowNum / blockLength][lIndex[c]];
}

void BwtSearch::backwardSearch() {
    unsigned int fr = 0;
    unsigned int lr = 0;

    std::map <char, unsigned int>::iterator cTableIterator = cTable.find(searchTerm[searchTermLength-1]);
    if (cTableIterator == cTable.end()) {
        // last char in searchTerm not found
        std::cout << searchTerm << " not found" << std::endl;
        return;
    }

    // last char in searchTerm
    fr = cTableIterator->second;
    if (cTableIterator != prev(cTable.end())) {
        lr = next(cTableIterator)->second - 1;
    } else {
        // last char in cTable
        lr = totalLength - 1;
    }

    // previous chars in searchTerm
    auto searchTermIterator = next(searchTerm.rbegin());
    for (; searchTermIterator != searchTerm.rend(); searchTermIterator++) {
        if (cTable.find(*searchTermIterator) == cTable.end()) {
            std::cout << searchTerm << " not found" << std::endl;
            return;
        }
        cTableIterator = cTable.find(*searchTermIterator);
        if (occurance (*searchTermIterator, fr) == 0) {
            // first row in lTable
            fr = cTableIterator->second + occurance(*searchTermIterator, fr);
        } else {
            fr = cTableIterator->second + occurance(*searchTermIterator, (fr-1));
        }
        lr = cTableIterator->second + occurance(*searchTermIterator, lr) - 1;
    }

    substringSearch(fr, lr);
}

void BwtSearch::substringSearch(unsigned int fr, unsigned int lr) {
    std::string retText = searchTerm;
    std::string retOffset;
    std::string rightText = "";
    std::string rightOffset = "";
    for (int iter = fr; iter <= lr; iter++) {
        retText = searchTerm;
        retOffset = "";
        unsigned int first = iter;
        // unsigned int last;
        while (retText[0] != '[') {
            if (bwt[first] == ']') {
                // found [<offset>]
                while (bwt[first] != '[') {
                    // get offset integer
                    retText = bwt[first] + retText;
                    retOffset = bwt[first] + retOffset;
                    first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
                }
                if (searchResults.find(static_cast<unsigned int>(stoi(retOffset))) != searchResults.end()) {
                    // text is already found and store in searchResults
                    retText = searchTerm;
                    retOffset = "";
                    break;
                }
            }
            retText = bwt[first] + retText;
            if (bwt[first] == '[') {
                // end of text
                break;
            }
            first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
        }

        if (retOffset.size() == 0) {
            // offset not found
            continue;
        }
        unsigned int last;
        // find next '['
        if (occurance('[', first) < (next(cTable.find('['))->second - cTable['['])) {
            // next bwt value is '['
            while (bwt[first+1] != '[') {
                first++;
            }
            first++;
            last = first;
            first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
        } else {
            // next bwt value not '[', get first '[' from cTable
            first = cTable['['];
            last = first;
        }
        rightText = bwt[first];
        rightOffset = "";

        while (first != iter) {
            first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
            rightText = bwt[first] + rightText;
            if ((first != iter) && (bwt[first] == ']')) {
                // find next offset integer
                while (bwt[first] != '[') {
                    first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
                    rightOffset = bwt[first] + rightOffset;
                }
                rightOffset = rightOffset.substr(1, rightOffset.size() - 1);

                if (static_cast<unsigned int>(stoi(rightOffset)) != static_cast<unsigned int>(stoi(retOffset))) { // TODO check here
                    rightOffset = "";
                    first = last;
                    if (occurance('[', first) < (next(cTable.find('['))->second - cTable['['])) {
                        // next bwt value is '['
                        while (bwt[first+1] != '[') {
                            first++;
                        }
                        first++;
                        last = first;
                        first = cTable[bwt[first]] + occurance(bwt[first], first) - 1;
                    } else {
                        // next bwt value not '[', get first '[' from cTable
                        first = cTable['['];
                        last = first;
                    }
                    rightText = bwt[first];
                }
            }
        }
        if (int(rightText.size() - searchTermLength - 1) >= 0) {
            retText = retText + rightText.substr((searchTermLength + 1), (rightText.size() - searchTermLength - 1));
        }
        searchResults.insert({static_cast<unsigned int>(stoi(retOffset)), retText});
        retText = searchTerm;
    }

    for (auto& i : searchResults) {
        std::cout << i.second << std::endl;
    }
}

int BwtSearch::getInteger(unsigned int a, unsigned int b) {
    return (((b ^ mask) << (numBit * (numInt++))) | a);
}

int main(int argc, char **argv) {
    BwtSearch BwtSearch;
    BwtSearch.readfile(argc, argv);
    BwtSearch.search();
    BwtSearch.backwardSearch();
}