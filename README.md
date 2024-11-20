# Assignment 4: Search Engine 
Collaborators: Andrew Kuehn - Chiara Tesoriero

  The program implements an AVL tree data structure for storing, organizing, and querying data efficiently. It focuses on maintaining balance in the tree through rotations, 
ensuring optimal performance for insertions, deletions, and lookups. The AVLTree class is designed with various methods for managing key-document-frequency mappings and 
supporting operations like file I/O, visualization, and bulk processing. The program also provides utilities for text preprocessing, including tokenization, stop-word 
filtering, and stemming. Specialized trees (PersonTree, OrganizationTree, and WordsTree) allow categorization of data. The accompanying QueryProcessor class handles user 
queries, leveraging these AVL trees to search, sort, and present results efficiently. 
We started the project by psuedo coding the search engine inorder to feed it to AI. The following is the Psuedocode we created, used by Chatgpt to create the program.

### AVLTree Class Pseudocode

**Purpose**: Implements a balanced AVL (Adelson-Velsky and Landis) tree for efficient data storage and retrieval.

#### Properties
- `root`: Root node of the AVL tree.
- `uniqueTokens`: Tracks the count of unique keys.

#### Constructor
- Initialize `root` to `None` and `uniqueTokens` to `0`.

#### Destructor
- Call `makeEmpty()` to clear the tree.

#### Public Methods
1. `insert(x, documentID, frequency)`:
   - Add a new key or update an existing key.
   - Balance the tree if necessary.

2. `contains(x)`:
   - Check if a key exists in the tree.

3. `findNode(x)`:
   - Return the node containing the key, if found.

4. `getWordMapAtKey(key)`:
   - Retrieve the document-frequency map for a given key.

5. `isEmpty()`:
   - Return `true` if the tree is empty.

6. `prettyPrintTree()`:
   - Print the structure of the tree visually.

7. `makeEmpty()`:
   - Clear all nodes in the tree.

8. `writeToTextFile(filename)`:
   - Save tree contents to a file.

9. `readFromTextFile(filename)`:
   - Load tree contents from a file.

10. `getSize()`:
   - Return the number of unique keys.

#### Private Methods
- `balance(t)`:
  - Balance the tree at node `t`.

- Rotation Methods:
  - `rotateWithLeftChild(k2)`: Perform a single right rotation.
  - `rotateWithRightChild(k1)`: Perform a single left rotation.
  - `doubleWithLeftChild(k3)`: Perform a right-left double rotation.
  - `doubleWithRightChild(k1)`: Perform a left-right double rotation.

- Utility Methods:
  - `height(t)`: Return the height of node `t`.
  - `max(lhs, rhs)`: Return the larger of two values.
  - `writeHelper(node, outFile)`: Recursively save tree data to a file.

---

### Utility Methods

1. `loadStopWords(filePath)`:
   - Read stop words from the file at `filePath`.

2. `stemWord(word)`:
   - Apply a stemming algorithm to reduce the word to its base form.

3. `toLower(text)`:
   - Convert text to lowercase.

4. `containsStopWords(word)`:
   - Check if the word is in the stop words set.

5. `tokenizer(textLine)`:
   - Split a text line into individual words.

6. `removePunctuation(word)`:
   - Remove punctuation and non-alphabetic characters from a word.

---

### Document Parsing Workflow

1. `runDocument(documentName)`:
   - Open the document specified by `documentName`.
   - Load stop words if not already done.
   - Parse the document content (e.g., JSON format).
   - Tokenize, clean, and stem text content.
   - Add non-stop-word tokens to the `WordsTree`.
   - Extract and add person and organization entities to respective trees.

2. `pushToTreePerson(token, docName, frequency)`:
   - Insert the token into `PersonTree`.

3. `pushToTreeOrg(token, docName, frequency)`:
   - Insert the token into `OrganizationTree`.

4. `pushToTreeWord(token, docName, frequency)`:
   - Insert the token into `WordsTree`.

---

### QueryProcessor Class Pseudocode

#### Properties
- `PersonTree`, `OrganizationTree`, `WordsTree`: AVL trees for managing indexed data.
- `vectorOfMaps`: Holds results for valid keywords.
- `vectorOfBadMaps`: Holds results for excluded terms (e.g., prefixed with `-`).
- `documentFrequencyPairs`: A list of document-frequency pairs.
- `searchIndex`: Tracks the starting index for query session output.

#### Methods
1. `runQueryProcessor(search)`:
   - Process the input query.
   - Sort results by frequency and display top matches.

2. `processQuery(search)`:
   - Tokenize the query into keywords.
   - Retrieve matching documents from relevant trees.
   - Combine results and exclude documents from negative terms.
   - Return the final map of documents and frequencies.

3. `sortDocumentsByFrequency(documentFrequencyMap)`:
   - Sort documents based on frequency in descending order.

4. `outputDocuments(numDocuments)`:
   - Output a specified number of documents.

5. `SeperateString(search)`:
   - Parse the query string for categorization (e.g., "ORG:", "PERSON:", "-").

6. `intersectMaps(map1, map2)`:
   - Find common documents between two maps and sum their frequencies.

7. `excludeMaps(map, badMap)`:
   - Remove documents found in `badMap` from `map`.

---

### Main Function Workflow

1. **`indexDirectory`**:
   - Prompt the user for a directory path.
   - Iterate through files, process each file with `DocumentParser`, and calculate performance statistics.

2. **`performQuery`**:
   - Allow users to run queries interactively.
   - Provide options to display results, print documents, or exit.

3. **`startUI`**:
   - Start the user interface for indexing, querying, and file operations.

4. **`main(argc, argv)`**:
   - Parse command-line arguments.
   - Handle modes: `index`, `query`, or `ui`.


