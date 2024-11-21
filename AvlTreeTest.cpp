#define CATCH_CONFIG_MAIN
#include "AvlTree.h"
#include "catch2/catch.hpp"

using namespace std;

// Test case for operations on an empty AVL tree
TEST_CASE("Empty AVL Tree") {
    AvlTree<string> avlTree;

    SECTION("Tree is initially empty") {
        REQUIRE(avlTree.isEmpty());
    }

    SECTION("Checking containment in an empty tree") {
        REQUIRE_FALSE(avlTree.contains("example"));
    }
}

// Test case for insertion and checking containment in the AVL tree
TEST_CASE("AVL Tree Insertion and Containment Check") {
    AvlTree<string> avlTree;

    // Insert multiple words with associated document IDs and frequencies
    avlTree.insert("example", "doc1", 5);
    avlTree.insert("example", "doc2", 3);
    avlTree.insert("test", "doc1", 7);
    avlTree.insert("test", "doc3", 2);
    avlTree.insert("data", "doc1", 10);
    avlTree.insert("Aaron", "doc4", 69);

    SECTION("Checking if the tree contains inserted keys") {
        REQUIRE(avlTree.contains("example"));
        REQUIRE(avlTree.contains("test"));
        REQUIRE(avlTree.contains("data"));
        REQUIRE(avlTree.contains("Aaron"));

        REQUIRE_FALSE(avlTree.contains("unknown")); // Key not added
    }

    SECTION("Validating stored document IDs and frequencies") {
        // Verify correct mappings of document IDs and frequencies for each word
        auto exampleNode = avlTree.findNode("example");
        auto exampleMap = exampleNode->wordMap;
        REQUIRE(exampleMap.find("doc1") != exampleMap.end());
        REQUIRE(exampleMap.find("doc2") != exampleMap.end());
        REQUIRE(exampleMap["doc1"] == 5);
        REQUIRE(exampleMap["doc2"] == 3);

        auto testNode = avlTree.findNode("test");
        auto testMap = testNode->wordMap;
        REQUIRE(testMap.find("doc1") != testMap.end());
        REQUIRE(testMap.find("doc3") != testMap.end());
        REQUIRE(testMap["doc1"] == 7);
        REQUIRE(testMap["doc3"] == 2);

        auto aaronNode = avlTree.findNode("Aaron");
        auto aaronMap = aaronNode->wordMap;
        REQUIRE(aaronMap.find("doc4") != aaronMap.end());
        REQUIRE(aaronMap["doc4"] == 69);
    }

    SECTION("Displaying the tree structure") {
        avlTree.prettyPrintTree();
    }
}

// Test case for copy constructor and assignment operator in the AVL tree
TEST_CASE("AVL Tree Copy Constructor and Assignment Operator") {
    AvlTree<string> avlTree;

    avlTree.insert("example", "doc1", 5);
    avlTree.insert("example", "doc2", 3);
    avlTree.insert("test", "doc1", 7);
    avlTree.insert("test", "doc3", 2);
    avlTree.insert("data", "doc1", 10);

    SECTION("Testing copy constructor") {
        AvlTree<string> avlTreeCopy(avlTree);

        REQUIRE(avlTreeCopy.contains("example"));
        REQUIRE(avlTreeCopy.contains("test"));
        REQUIRE(avlTreeCopy.contains("data"));

        // Display the copied tree structure
        avlTreeCopy.prettyPrintTree();
    }

    SECTION("Testing assignment operator") {
        AvlTree<string> avlTreeAssigned;
        avlTreeAssigned = avlTree;

        REQUIRE(avlTreeAssigned.contains("example"));
        REQUIRE(avlTreeAssigned.contains("test"));
        REQUIRE(avlTreeAssigned.contains("data"));

        // Display the assigned tree structure
        avlTreeAssigned.prettyPrintTree();
    }
}

// Test case for clearing the contents of the AVL tree
TEST_CASE("AVL Tree Clearing") {
    AvlTree<string> avlTree;

    avlTree.insert("example", "doc1", 5);
    avlTree.insert("test", "doc2", 7);
    avlTree.insert("data", "doc1", 10);

    SECTION("Clearing all nodes in the tree") {
        REQUIRE_FALSE(avlTree.isEmpty());

        avlTree.makeEmpty();

        REQUIRE(avlTree.isEmpty());
    }
}

// Test case for persistence operations in the AVL tree
TEST_CASE("AVL Tree Persistence") {
    AvlTree<string> test1;
    AvlTree<string> test2;

    // Insert nodes and store the tree's state in a file
    test1.insert("example", "doc1", 5);
    test1.insert("example", "doc5", 9);
    test1.insert("test", "doc2", 7);
    test1.insert("data", "doc3", 10);

    test1.writeToTextFile("testPersistence.txt");
    test2.readFromTextFile("testPersistence.txt");

    // Verify the second tree has identical contents
    REQUIRE(test2.contains("example"));
    REQUIRE(test2.contains("test"));
    REQUIRE(test2.contains("data"));

    // Check that frequencies are properly restored
    auto exampleNode = test2.findNode("example");
    auto exampleMap = exampleNode->wordMap;
    REQUIRE(exampleMap["doc1"] == 5);
    REQUIRE(exampleMap["doc5"] == 9);
}
