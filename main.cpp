#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//

void print_Table(){
    
}




int main(std::string argc, char *argv[]) {
    // Create a vector to store the lines
    // Count the number of lines
    std::vector<std::string> lines;
    std::string line;
    unsigned int n_lines = 1;




    // Instructions for the user
    if (argc == "-h") {
        std::cout << " Instructions " << argv[0] << " <filename>" << '\n';
        return 1;
    }

    // Open the file
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    // Check if the file is open
    if (!file) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    
    // Read the file line by line
    while (getline(file, line)) {
        lines.push_back(line);
        n_lines++; // Count the number of lines
    }

    std::cout << "Number of lines: " << n_lines << std::endl;
    
    // Close the file
    file.close();
    return EXIT_SUCCESS;
}

