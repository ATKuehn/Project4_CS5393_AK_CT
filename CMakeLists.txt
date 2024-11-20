# Specify the minimum version of CMake required to build the project
cmake_minimum_required(VERSION 3.15)

# Define the name of the project
project(FinalProjExamples)

# Set the required C++ standard for the project. 
# The "filesystem.h" header and other modern features require C++17.
set(CMAKE_CXX_STANDARD 17)

# Copy the contents of the `sample_data` directory to the build directory.
# This ensures that data files are accessible when the program runs.
file(COPY sample_data/ DESTINATION sample_data/)

# Copy the contents of the `Trees` directory to the build directory.
# These files likely contain necessary resources or data for testing and execution.
file(COPY Trees/ DESTINATION Trees/)

# Configure the `stopWords.txt` file to be included in the build directory as a read-only file.
# This file is likely used for filtering common words in the program logic.
configure_file(stopWords.txt stopWords.txt COPYONLY)

# Enable verbose output for compiler and build commands during the build process.
# This can help developers debug the build process and view detailed output.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add compiler options to display warnings and enforce strict code quality checks.
# -Wall: Enables all standard warnings.
# -Wextra: Enables extra warnings.
# -pedantic: Enforces strict compliance with the C++ standard.
add_compile_options(-Wall -Wextra -pedantic)

# Define a target executable named `AvlTest` that uses `AvlTreeTest.cpp` and `AvlTree.h`.
# This target is likely for testing the AVL tree implementation.
add_executable(AvlTest AvlTreeTest.cpp AvlTree.h)

# Define a target executable named `rapidJSONExample` that uses `rapidJSONExample.cpp`.
# This target demonstrates the usage of the RapidJSON library.
add_executable(rapidJSONExample rapidJSONExample.cpp)

# Define a target executable named `supersearch` that uses `main.cpp` and `stopWords.txt`.
# This target is the main application of the project, which includes functionality like searching or processing text.
add_executable(supersearch main.cpp stopWords.txt)

# Ensure that the `rapidJSONExample` target has access to the RapidJSON library.
# The library files are included privately, meaning they're only visible to this target.
target_include_directories(rapidJSONExample PRIVATE rapidjson/)

# Turn off verbose output after all the configuration commands are complete.
# This ensures that subsequent builds are less cluttered unless debugging the build process.
set(CMAKE_VERBOSE_MAKEFILE OFF)
