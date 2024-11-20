#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

using namespace std;

/**
 * @class AvlTree
 * @brief A self-balancing binary search tree (AVL tree) implementation.
 * Each node stores additional height information to maintain balance, and
 * includes a map to store document IDs and their respective frequencies.
 */
template <typename Comparable>
class AvlTree {
   private:
    struct AvlNode {
        Comparable key;                      // The key stored in the node
        AvlNode *left;                       // Pointer to the left child
        AvlNode *right;                      // Pointer to the right child
        int height;                          // Height of the node
        std::map<std::string, int> wordMap;  // Map of document IDs to frequencies

        // Constructor for AvlNode
        AvlNode(const Comparable &theKey, AvlNode *lt = nullptr, AvlNode *rt = nullptr, int h = 0)
            : key{theKey}, left{lt}, right{rt}, height{h} {}
    };

    size_t uniqueTokens = 0;   // Tracks the number of unique keys in the tree
    AvlNode *root;             // Root node of the tree

    // The allowed imbalance factor for the AVL tree. A higher value reduces rebalancing but may affect search efficiency.
    static const int ALLOWED_IMBALANCE = 1;

   public:

    /**
     * @brief Retrieves the number of unique keys in the tree.
     * @return The number of unique keys.
     */
    size_t getSize() const {
        return uniqueTokens;
    }

    /**
     * @brief Default constructor. Initializes an empty AVL tree.
     */
    AvlTree() : root{nullptr} {}

    /**
     * @brief Copy constructor. Creates a deep copy of another AVL tree.
     * @param rhs The AVL tree to copy.
     */
    AvlTree(const AvlTree &rhs) : root{nullptr} {
        root = clone(rhs.root);
    }

    /**
     * @brief Destructor. Frees all memory associated with the tree.
     */
    ~AvlTree() {
        makeEmpty();
    }

    /**
     * @brief Copy assignment operator. Assigns one AVL tree to another.
     * @param rhs The AVL tree to copy.
     * @return A reference to this tree.
     */
    AvlTree &operator=(const AvlTree &rhs) {
        if (this != &rhs) {
            makeEmpty();
            root = clone(rhs.root);
        }
        return *this;
    }

    /**
     * @brief Checks if the tree contains a given key.
     * @param x The key to search for.
     * @return True if the key is found, false otherwise.
     */
    bool contains(const Comparable &x) const {
        return contains(x, root);
    }

    /**
     * @brief Finds a node with the specified key.
     * @param x The key to search for.
     * @return A pointer to the node, or nullptr if not found.
     */
    AvlNode *findNode(const Comparable &x) const {
        return findNode(x, root);
    }

    /**
     * @brief Retrieves the document-frequency map for a specific key.
     * @param key The key to search for.
     * @return A map of document IDs and their frequencies, or an empty map if the key is not found.
     */
    std::map<std::string, int> getWordMapAtKey(const Comparable &key) const {
        AvlNode *node = findNode(key);
        return node ? node->wordMap : std::map<std::string, int>();
    }

    /**
     * @brief Checks if the tree is empty.
     * @return True if the tree has no nodes, false otherwise.
     */
    bool isEmpty() const {
        return root == nullptr;
    }

    /**
     * @brief Prints the structure of the tree in a readable format.
     */
    void prettyPrintTree() const {
        prettyPrintTree("", root, false);
    }

    /**
     * @brief Clears all nodes from the tree.
     */
    void makeEmpty() {
        makeEmpty(root);
    }

    /**
     * @brief Inserts a key with document ID and frequency into the tree.
     *        Updates frequency if the key and document ID already exist.
     * @param x The key to insert.
     * @param documentID The document ID associated with the key.
     * @param frequency The frequency count for the document ID.
     */
    void insert(const Comparable &x, const std::string &documentID, int frequency) {
        insert(x, documentID, frequency, root);
    }

    /**
     * @brief Writes the tree structure to a text file in a readable format.
     * @param filename The name of the file to write to.
     */
    void writeToTextFile(const std::string &filename) const {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
            return;
        }
        writeHelper(root, outFile);
        outFile.close();
    }

    /**
     * @brief Reads tree data from a text file and reconstructs the tree.
     * @param filename The name of the file to read from.
     */
    void readFromTextFile(const std::string &filename) {
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            std::string key, docID;
            int frequency;

            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                std::cerr << "Error: Invalid file format. Colon not found." << std::endl;
                continue;
            }
            key = line.substr(0, colonPos);

            size_t openParenPos, commaPos, closeParenPos;
            while ((openParenPos = line.find('(', colonPos)) != std::string::npos) {
                commaPos = line.find(',', openParenPos);
                closeParenPos = line.find(')', commaPos);
                if (commaPos == std::string::npos || closeParenPos == std::string::npos) {
                    std::cerr << "Error: Invalid file format. Parentheses or comma not found." << std::endl;
                    break;
                }
                docID = line.substr(openParenPos + 1, commaPos - openParenPos - 1);
                frequency = std::stoi(line.substr(commaPos + 1, closeParenPos - commaPos - 1));
                insert(key, docID, frequency);
                colonPos = closeParenPos + 1;
            }
        }
        inFile.close();
    }

#ifdef DEBUG
    /**
     * @brief Validates the balance and height properties of the tree.
     * Throws an exception if any imbalance or incorrect height is detected.
     * Only compiled when DEBUG is defined.
     */
    int check_balance() {
        return check_balance(root);
    }
#endif
};
#endif
