#include <chrono>
#include <iostream>
#include <filesystem>
#include "AvlTree.h"
#include "DocumentParser.h"
#include "QueryProcessor.h"
//referenced from G4G, DigitalOceans

using namespace std;

// Function to index all files in a given directory and output performance statistics.
void indexDirectory(DocumentParser& docParse, AvlTree<string>& PersonTree, 
                    AvlTree<string>& OrganizationTree, AvlTree<string>& WordsTree) {
    cout << "Enter the path to the directory to index: ";
    string input;
    cin >> input;

    auto start = chrono::high_resolution_clock::now();

    // Iterate over all files in the specified directory and process them.
    for (const auto& entry : filesystem::recursive_directory_iterator(input)) {
        if (entry.is_regular_file()) {
            docParse.runDocument(entry.path().string());
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    // Output indexing statistics.
    cout << "Indexing took " << duration.count() << " seconds.\n";
    cout << "Indexing completed.\n";
    cout << "Unique names: " << PersonTree.getSize() << "\n";
    cout << "Unique organizations: " << OrganizationTree.getSize() << "\n";
    cout << "Unique words: " << WordsTree.getSize() << "\n";
    cout << "Files indexed: " << docParse.getFilesIndexed() << "\n";
}

// Function to handle query execution and display a query results menu.
void performQuery(QueryProcessor& queryProc, DocumentParser& docParse) {
    cout << "Enter the search query: ";
    cin.ignore();  // Clear the input buffer.
    string input;
    getline(cin, input);

    auto start = chrono::high_resolution_clock::now();
    queryProc.runQueryProcessor(input);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Query took " << duration.count() << " seconds.\n";
    char userChoice;

    // Loop to provide options for query results.
    while (true) {
        cout << "\nQuery Results Menu:\n";
        cout << "Press 'n' to print 5 more documents.\n";
        cout << "Press 'q' to start a new query.\n";
        cout << "Press 'd' and enter a document number to print its text.\n";
        cout << "Press 'e' to return to the main menu.\n";
        cout << "Enter your choice: ";
        cin >> userChoice;

        switch (userChoice) {
            case 'n':
                queryProc.outputDocuments(5);  // Output 5 additional documents.
                break;

            case 'q':
                cout << "Enter the new search query: ";
                cin.ignore();
                getline(cin, input);
                start = chrono::high_resolution_clock::now();
                queryProc.runQueryProcessor(input);
                end = chrono::high_resolution_clock::now();
                duration = end - start;
                cout << "Query took " << duration.count() << " seconds.\n";
                break;

            case 'd': {
                cout << "Enter the document number to print: ";
                int docNumber;
                cin >> docNumber;
                string docName = queryProc.getDocumentName(docNumber - 1);
                docParse.printDocumentText(docName);
                break;
            }

            case 'e':
                return;  // Return to the main menu.

            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

// Main menu for the application.
static void startUI() {
    // Create AVL trees and associated objects for processing.
    AvlTree<string> PersonTree;
    AvlTree<string> OrganizationTree;
    AvlTree<string> WordsTree;
    DocumentParser documentParser(PersonTree, OrganizationTree, WordsTree);
    QueryProcessor queryProcessor(PersonTree, OrganizationTree, WordsTree);

    char userChoice;

    // Main menu loop.
    while (true) {
        cout << "\nSuperSearch Menu:\n";
        cout << "Press 'i' to index a directory.\n";
        cout << "Press 'q' to perform a query.\n";
        cout << "Press 'w' to write the index to a file directory.\n";
        cout << "Press 'r' to read an index from a file directory.\n";
        cout << "Press 'e' to exit.\n";
        cout << "Enter your choice: ";
        cin >> userChoice;

        switch (userChoice) {
            case 'i':
                indexDirectory(documentParser, PersonTree, OrganizationTree, WordsTree);
                break;

            case 'q':
                performQuery(queryProcessor, documentParser);
                break;

            case 'w': {
                string folderName;
                cout << "Enter the directory to save the index: ";
                cin >> folderName;

                filesystem::create_directories(folderName);
                documentParser.toFile(folderName + "/personTree.txt", 
                                      folderName + "/organizationTree.txt", 
                                      folderName + "/wordsTree.txt");
                break;
            }

            case 'r': {
                string folderName;
                cout << "Enter the directory to load the index from: ";
                cin >> folderName;

                queryProcessor.getTreesfromFile(folderName + "/personTree.txt",
                                                folderName + "/organizationTree.txt",
                                                folderName + "/wordsTree.txt");
                break;
            }

            case 'e':
                cout << "Exiting program.\n";
                return;

            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    // Validate command-line arguments and initialize components.
    if (argc < 2) {
        cerr << "Usage:\n"
             << argv[0] << " index <directory>\n"
             << argv[0] << " query <query-string>\n"
             << argv[0] << " ui\n";
        return 1;
    }

    AvlTree<string> PersonTree;
    AvlTree<string> OrganizationTree;
    AvlTree<string> WordsTree;
    DocumentParser documentParser(PersonTree, OrganizationTree, WordsTree);
    QueryProcessor queryProcessor(PersonTree, OrganizationTree, WordsTree);

    string command = argv[1];

    // Handle different modes of operation.
    if (command == "index" && argc == 3) {
        string directory = argv[2];
        indexDirectory(documentParser, PersonTree, OrganizationTree, WordsTree);
        documentParser.toFile("Trees/personTree.txt", "Trees/organizationTree.txt", "Trees/wordsTree.txt");

    } else if (command == "query" && argc == 3) {
        string query = argv[2];
        queryProcessor.getTreesfromFile("Trees/personTree.txt", "Trees/organizationTree.txt", "Trees/wordsTree.txt");
        queryProcessor.runQueryProcessor(query);

    } else if (command == "ui" && argc == 2) {
        startUI();

    } else {
        cerr << "Invalid command or arguments. See usage:\n"
             << argv[0] << " index <directory>\n"
             << argv[0] << " query <query-string>\n"
             << argv[0] << " ui\n";
        return 1;
    }

    return 0;
}
