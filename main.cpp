#include <chrono>
#include <iostream>
#include <filesystem>
#include "AvlTree.h"
#include "DocumentParser.h"
#include "QueryProcessor.h"

// Function to index all files in a given directory and output performance statistics.
void indexDirectory(DocumentParser& docParse, AvlTree<std::string>& PersonTree, 
                    AvlTree<std::string>& OrganizationTree, AvlTree<std::string>& WordsTree) {
    std::cout << "Enter the path to the directory to index: ";
    std::string input;
    std::cin >> input;

    auto start = std::chrono::high_resolution_clock::now();

    // Iterate over all files in the specified directory and process them.
    for (const auto& entry : std::filesystem::recursive_directory_iterator(input)) {
        if (entry.is_regular_file()) {
            docParse.runDocument(entry.path().string());
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Output indexing statistics.
    std::cout << "Indexing took " << duration.count() << " seconds.\n";
    std::cout << "Indexing completed.\n";
    std::cout << "Unique names: " << PersonTree.getSize() << "\n";
    std::cout << "Unique organizations: " << OrganizationTree.getSize() << "\n";
    std::cout << "Unique words: " << WordsTree.getSize() << "\n";
    std::cout << "Files indexed: " << docParse.getFilesIndexed() << "\n";
}

// Function to handle query execution and display a query results menu.
void performQuery(QueryProcessor& queryProc, DocumentParser& docParse) {
    std::cout << "Enter the search query: ";
    std::cin.ignore();  // Clear the input buffer.
    std::string input;
    std::getline(std::cin, input);

    auto start = std::chrono::high_resolution_clock::now();
    queryProc.runQueryProcessor(input);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Query took " << duration.count() << " seconds.\n";
    char userChoice;

    // Loop to provide options for query results.
    while (true) {
        std::cout << "\nQuery Results Menu:\n";
        std::cout << "Press 'n' to print 5 more documents.\n";
        std::cout << "Press 'q' to start a new query.\n";
        std::cout << "Press 'd' and enter a document number to print its text.\n";
        std::cout << "Press 'e' to return to the main menu.\n";
        std::cout << "Enter your choice: ";
        std::cin >> userChoice;

        switch (userChoice) {
            case 'n':
                queryProc.outputDocuments(5);  // Output 5 additional documents.
                break;

            case 'q':
                std::cout << "Enter the new search query: ";
                std::cin.ignore();
                std::getline(std::cin, input);
                start = std::chrono::high_resolution_clock::now();
                queryProc.runQueryProcessor(input);
                end = std::chrono::high_resolution_clock::now();
                duration = end - start;
                std::cout << "Query took " << duration.count() << " seconds.\n";
                break;

            case 'd': {
                std::cout << "Enter the document number to print: ";
                int docNumber;
                std::cin >> docNumber;
                std::string docName = queryProc.getDocumentName(docNumber - 1);
                docParse.printDocumentText(docName);
                break;
            }

            case 'e':
                return;  // Return to the main menu.

            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

// Main menu for the application.
static void startUI() {
    // Create AVL trees and associated objects for processing.
    AvlTree<std::string> PersonTree;
    AvlTree<std::string> OrganizationTree;
    AvlTree<std::string> WordsTree;
    DocumentParser documentParser(PersonTree, OrganizationTree, WordsTree);
    QueryProcessor queryProcessor(PersonTree, OrganizationTree, WordsTree);

    char userChoice;

    // Main menu loop.
    while (true) {
        std::cout << "\nSuperSearch Menu:\n";
        std::cout << "Press 'i' to index a directory.\n";
        std::cout << "Press 'q' to perform a query.\n";
        std::cout << "Press 'w' to write the index to a file directory.\n";
        std::cout << "Press 'r' to read an index from a file directory.\n";
        std::cout << "Press 'e' to exit.\n";
        std::cout << "Enter your choice: ";
        std::cin >> userChoice;

        switch (userChoice) {
            case 'i':
                indexDirectory(documentParser, PersonTree, OrganizationTree, WordsTree);
                break;

            case 'q':
                performQuery(queryProcessor, documentParser);
                break;

            case 'w': {
                std::string folderName;
                std::cout << "Enter the directory to save the index: ";
                std::cin >> folderName;

                std::filesystem::create_directories(folderName);
                documentParser.toFile(folderName + "/personTree.txt", 
                                      folderName + "/organizationTree.txt", 
                                      folderName + "/wordsTree.txt");
                break;
            }

            case 'r': {
                std::string folderName;
                std::cout << "Enter the directory to load the index from: ";
                std::cin >> folderName;

                queryProcessor.getTreesfromFile(folderName + "/personTree.txt",
                                                folderName + "/organizationTree.txt",
                                                folderName + "/wordsTree.txt");
                break;
            }

            case 'e':
                std::cout << "Exiting program.\n";
                return;

            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    // Validate command-line arguments and initialize components.
    if (argc < 2) {
        std::cerr << "Usage:\n"
                  << argv[0] << " index <directory>\n"
                  << argv[0] << " query <query-string>\n"
                  << argv[0] << " ui\n";
        return 1;
    }

    AvlTree<std::string> PersonTree;
    AvlTree<std::string> OrganizationTree;
    AvlTree<std::string> WordsTree;
    DocumentParser documentParser(PersonTree, OrganizationTree, WordsTree);
    QueryProcessor queryProcessor(PersonTree, OrganizationTree, WordsTree);

    std::string command = argv[1];

    // Handle different modes of operation.
    if (command == "index" && argc == 3) {
        std::string directory = argv[2];
        indexDirectory(documentParser, PersonTree, OrganizationTree, WordsTree);
        documentParser.toFile("Trees/personTree.txt", "Trees/organizationTree.txt", "Trees/wordsTree.txt");

    } else if (command == "query" && argc == 3) {
        std::string query = argv[2];
        queryProcessor.getTreesfromFile("Trees/personTree.txt", "Trees/organizationTree.txt", "Trees/wordsTree.txt");
        queryProcessor.runQueryProcessor(query);

    } else if (command == "ui" && argc == 2) {
        startUI();

    } else {
        std::cerr << "Invalid command or arguments. See usage:\n"
                  << argv[0] << " index <directory>\n"
                  << argv[0] << " query <query-string>\n"
                  << argv[0] << " ui\n";
        return 1;
    }

    return 0;
}
