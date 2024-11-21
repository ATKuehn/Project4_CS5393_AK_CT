#ifndef DOCUMENT_PARSER_H
#define DOCUMENT_PARSER_H

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "AvlTree.h"
#include "Porter2/porter2Stemmer.cpp" // For stemming words
#include "rapidjson/document.h"       // For JSON parsing
#include "rapidjson/istreamwrapper.h" // For JSON stream handling

using namespace std;
using namespace rapidjson;

/**
 * @class DocumentParser
 * @brief A class for parsing and indexing documents. It processes text by
 * tokenizing, stemming, and filtering stop words, then stores the results in AVL trees.
 */
class DocumentParser {
   private:
    // References to AVL trees for indexing persons, organizations, and words
    AvlTree<string>& PersonTree;
    AvlTree<string>& OrganizationTree;
    AvlTree<string>& WordsTree;

    // Counter for the number of files indexed
    int filesIndexed = 0;

   public:
    /**
     * @brief Constructor that initializes the AVL trees for indexing.
     * @param person Reference to the AVL tree for persons.
     * @param org Reference to the AVL tree for organizations.
     * @param word Reference to the AVL tree for general words.
     */
    DocumentParser(AvlTree<string>& person, AvlTree<string>& org, AvlTree<string>& word)
        : PersonTree(person), OrganizationTree(org), WordsTree(word) {}

    // Set to store stop words for filtering
    static set<string> stopWords;

    /**
     * @brief Stems a given word using Porter2 stemming.
     * @param word The word to be stemmed.
     * @return The stemmed version of the word.
     */
    static string stemWord(const string& word) {
        string stemmedWord = word;
        Porter2Stemmer::trim(stemmedWord); // Trim whitespace
        Porter2Stemmer::stem(stemmedWord); // Apply stemming
        return stemmedWord;
    }

    /**
     * @brief Loads stop words from a file into a set.
     * @param filePath Path to the stop words file.
     */
    static void loadStopWords(const string& filePath) {
        ifstream file(filePath);
        string word;

        if (!file.is_open()) {
            cerr << "Unable to open stop words file: " << filePath << endl;
            return;
        }

        while (file >> word) {
            stopWords.insert(word);
        }

        file.close();
    }

    /**
     * @brief Converts a string to lowercase.
     * @param text The input string.
     * @return The lowercase version of the input string.
     */
    static string toLower(const string& text) {
        string result;
        result.reserve(text.size());
        for (char ch : text) {
            result += tolower(ch);
        }
        return result;
    }

    /**
     * @brief Checks if a word is a stop word.
     * @param word The word to check.
     * @return True if the word is a stop word, false otherwise.
     */
    static bool containsStopWords(const string& word) {
        string lowerToken = toLower(word);
        return stopWords.find(lowerToken) != stopWords.end();
    }

    /**
     * @brief Tokenizes a text line into individual words using spaces as delimiters.
     * @param textLine The line of text to tokenize.
     * @return A vector of tokens (words).
     */
    static vector<string> tokenizer(const string textLine) {
        string word;
        vector<string> tokens;
        size_t start = 0;

        for (size_t i = 0; i < textLine.length(); ++i) {
            if (textLine[i] == ' ') {
                word = textLine.substr(start, i - start);
                start = i + 1;
                tokens.push_back(word);
            }
        }

        // Add the last token
        word = textLine.substr(start);
        tokens.push_back(word);

        return tokens;
    }

    /**
     * @brief Removes punctuation from a word.
     * @param word The input word.
     * @return The word without punctuation.
     */
    static string removePunctuation(string word) {
        for (size_t i = 0; i < word.length(); ++i) {
            if (ispunct(word[i]) || !isalpha(word[i])) {
                word.erase(i, 1);
                --i;
            }
        }
        return word;
    }

    /**
     * @brief Processes and indexes a document.
     *        It filters stop words, removes punctuation, converts text to lowercase,
     *        stems words, and adds them to AVL trees for indexing.
     * @param documentName The path to the document file.
     */
    void runDocument(string documentName) {
        filesIndexed++;
        ifstream input(documentName);
        if (!input.is_open()) {
            cerr << "Cannot open file: " << documentName << endl;
            return;
        }

        if (stopWords.empty()) {
            loadStopWords("stopWords.txt");
        }

        IStreamWrapper isw(input);
        Document d;
        d.ParseStream(isw);

        string docText = d["text"].GetString();
        vector<string> tokens = tokenizer(docText);

        for (auto& token : tokens) {
            token = removePunctuation(token);
            token = toLower(token);
            token = stemWord(token);
        }

        for (const auto& token : tokens) {
            if (!containsStopWords(token) && !token.empty()) {
                pushToTreeWord(token, documentName, 1);
            }
        }

        // Index persons from the document
        auto docPersons = d["entities"]["persons"].GetArray();
        for (const auto& person : docPersons) {
            string personName = person["name"].GetString();
            for (const auto& name : tokenizer(personName)) {
                pushToTreePerson(name, documentName, 1);
            }
        }

        // Index organizations from the document
        auto docOrgs = d["entities"]["organizations"].GetArray();
        for (const auto& org : docOrgs) {
            string orgName = org["name"].GetString();
            for (const auto& org : tokenizer(orgName)) {
                pushToTreeOrg(org, documentName, 1);
            }
        }

        input.close();
    }

    // Functions to insert tokens into the respective AVL trees
    void pushToTreePerson(string token, string docName, int frequency) {
        PersonTree.insert(token, docName, frequency);
    }

    void pushToTreeOrg(string token, string docName, int frequency) {
        OrganizationTree.insert(token, docName, frequency);
    }

    void pushToTreeWord(string token, string docName, int frequency) {
        WordsTree.insert(token, docName, frequency);
    }

    /**
     * @brief Prints a document's title and publication date from a JSON file.
     * @param filename The path to the document file.
     */
    static void printDocument(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file: " << filename << endl;
            return;
        }

        IStreamWrapper isw(file);
        Document doc;
        doc.ParseStream(isw);

        cout << "Article Name: " << doc["title"].GetString()
             << " Publication Date: " << doc["published"].GetString() << endl;

        file.close();
    }

    /**
     * @brief Prints the main text content of a document from a JSON file.
     * @param filename The path to the document file.
     */
    void printDocumentText(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file: " << filename << endl;
            return;
        }

        IStreamWrapper isw(file);
        Document document;
        document.ParseStream(isw);

        if (!document.HasMember("text") || !document["text"].IsString()) {
            cerr << "Text not found in JSON file: " << filename << endl;
            return;
        }

        cout << document["text"].GetString() << endl;

        file.close();
    }

    /**
     * @brief Serializes and writes the trees to files.
     * @param personFile The file path for storing the persons tree.
     * @param orgFile The file path for storing the organizations tree.
     * @param wordFile The file path for storing the words tree.
     */
    void toFile(const string& personFile, const string& orgFile, const string& wordFile) {
        PersonTree.writeToTextFile(personFile);
        OrganizationTree.writeToTextFile(orgFile);
        WordsTree.writeToTextFile(wordFile);
    }

    /**
     * @brief Returns the number of files indexed so far.
     * @return The number of files indexed.
     */
    int getFilesIndexed() const {
        return filesIndexed;
    }
};

set<string> DocumentParser::stopWords;

#endif  // DOCUMENT_PARSER_H
