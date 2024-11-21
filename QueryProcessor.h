#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include <cmath>
#include <map>
#include <vector>
#include "AvlTree.h"
#include "DocumentParser.h"

using namespace std;

// QueryProcessor handles search queries and outputs ranked documents based on frequency.
class QueryProcessor {
   private:
    // Reference to AVL trees containing person, organization, and word data
    AvlTree<string>& PersonTree;
    AvlTree<string>& OrganizationTree;
    AvlTree<string>& WordsTree;

    // Stores maps of query results and exclusion maps
    vector<map<string, int>> vectorOfMaps;
    vector<map<string, int>> vectorOfBadMaps;

    // Vector to hold document-frequency pairs for ranking
    vector<pair<string, int>> documentFrequencyPairs;

    // Index to track pagination during document output
    size_t searchIndex = 0;

   public:
    // Constructor initializes references to AVL trees
    QueryProcessor(AvlTree<string>& person, AvlTree<string>& org, AvlTree<string>& word)
        : PersonTree(person), OrganizationTree(org), WordsTree(word) {}

    // Processes a search query and outputs the top results
    void runQueryProcessor(const string& search) {
        // Clear previous data
        documentFrequencyPairs.clear();
        vectorOfMaps.clear();
        vectorOfBadMaps;
        searchIndex = 0;

        // Process the query
        map<string, int> result = processQuery(search);

        // Sort results by frequency and output top documents
        sortDocumentsByFrequency(result);
        outputDocuments(15); // Outputs the top 15 documents by default
    }

    // Processes a query string and returns the resulting map of document frequencies
    map<string, int> processQuery(string search) {
        SeperateString(search); // Tokenize and classify search terms

        if (vectorOfMaps.empty()) {
            return {}; // Return an empty map if there are no valid query terms
        }

        // Start with the first map and find intersections with other maps
        map<string, int> result = vectorOfMaps[0];
        for (size_t i = 1; i < vectorOfMaps.size(); ++i) {
            result = intersectMaps(result, vectorOfMaps[i]);
        }

        // Exclude results from bad maps
        for (const auto& badMap : vectorOfBadMaps) {
            result = excludeMaps(result, badMap);
        }
        return result;
    }

    // Sorts the document-frequency pairs in descending order by frequency
    void sortDocumentsByFrequency(const map<string, int>& documentFrequencyMap) {
        documentFrequencyPairs.clear();
        for (const auto& pair : documentFrequencyMap) {
            documentFrequencyPairs.push_back(pair);
        }
        sort(documentFrequencyPairs.begin(), documentFrequencyPairs.end(),
                  [](const pair<string, int>& a, const pair<string, int>& b) {
                      return a.second > b.second;
                  });
    }

    // Outputs the top `numDocuments` by relevance
    void outputDocuments(int numDocuments) {
        int count = 0;
        size_t startIndex = searchIndex;

        if (startIndex >= documentFrequencyPairs.size()) {
            cout << "No documents match the search criteria." << endl;
            return;
        }

        // Output documents starting from the current index
        while (startIndex < documentFrequencyPairs.size() && count < numDocuments) {
            const auto& pair = documentFrequencyPairs[startIndex];
            cout << count + 1 << ". ";
            DocumentParser::printDocument(pair.first);
            cout << endl;
            ++count;
            ++startIndex;
        }
        searchIndex = startIndex;
    }

    // Tokenizes and classifies search terms
    void SeperateString(string search) {
        string tempWord = "";
        vector<string> wordsToSearch = DocumentParser::tokenizer(search);

        for (auto& word : wordsToSearch) {
            word = removePunctuationExcept(word);

            if (word.substr(0, 4) == "ORG:") {
                tempWord = word.substr(4);
                vectorOfMaps.push_back(OrganizationTree.getWordMapAtKey(tempWord));
            } else if (word.substr(0, 7) == "PERSON:") {
                tempWord = DocumentParser::toLower(word.substr(7));
                vectorOfMaps.push_back(PersonTree.getWordMapAtKey(tempWord));
            } else if (word.substr(0, 1) == "-") {
                tempWord = DocumentParser::stemWord(word.substr(1));
                vectorOfBadMaps.push_back(WordsTree.getWordMapAtKey(tempWord));
            } else if (!DocumentParser::containsStopWords(word) && !word.empty()) {
                vectorOfMaps.push_back(WordsTree.getWordMapAtKey(DocumentParser::stemWord(word)));
            }
        }
    }

    // Finds the intersection of two maps
    map<string, int> intersectMaps(map<string, int>& map1, map<string, int>& map2) {
        map<string, int> intersectMap;

        for (auto it = map1.begin(); it != map1.end(); ++it) {
            auto findIter = map2.find(it->first);
            if (findIter != map2.end()) {
                intersectMap[it->first] = it->second + findIter->second;
                map2.erase(findIter); // Remove matched keys to optimize
            }
        }
        return intersectMap;
    }

    // Excludes keys from the map that exist in badMap
    map<string, int> excludeMaps(const map<string, int>& map, const map<string, int>& badMap) {
        map<string, int> excludeMap;
        for (const auto& pair : map) {
            if (badMap.find(pair.first) == badMap.end()) {
                excludeMap.insert(pair);
            }
        }
        return excludeMap;
    }

    // Removes punctuation from a string except for colons and dashes
    static string removePunctuationExcept(string word) {
        for (string::size_type i = 0; i < word.length(); ++i) {
            if (ispunct(word.at(i)) && word.at(i) != ':' && word.at(i) != '-') {
                word.erase(i, 1);
                --i;
            }
        }
        return word;
    }

    // Reads tree data from files
    void getTreesfromFile(const string& personFile, const string& orgFile, const string& wordFile) {
        PersonTree.readFromTextFile(personFile);
        OrganizationTree.readFromTextFile(orgFile);
        WordsTree.readFromTextFile(wordFile);
    }

    // Retrieves the document name at the specified index
    string getDocumentName(vector<pair<string, int>>::size_type index) {
        if (index >= documentFrequencyPairs.size()) {
            return "";
        }
        return documentFrequencyPairs[index].first;
    }
};

#endif // QUERY_PROCESSOR_H

#endif
