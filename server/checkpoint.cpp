// CheckPoint.cpp

#include "checkpoint.h"
#include <iostream>
#include <fstream>
#include <sstream>

void CheckPoint::createCheckPoint(const std::string& from, const std::string& to, const std::string& orderNumber, const std::string& request) {
    // Get the parent directory (you may need to adjust this depending on your file structure)
    std::string parentDirectory = "..";  // Go up two levels to reach 'bft-protocol-framework'

    // Create the log file path
    std::string logFilePath = parentDirectory + "/logs.txt";

    // Check if the log file exists
    std::ifstream file(logFilePath.c_str());
    if (file.good()) {
        // File exists, open for append
        std::ofstream outFile(logFilePath, std::ios_base::app);
        if (outFile.is_open()) {
            // Append a new line with three values
            outFile << from << "," << to << "," << orderNumber << "," << request << "\n";
            std::cout << "Log entry added to existing file.\n";
        } else {
            std::cerr << "Error opening file for append.\n";
        }
    } else {
        // File doesn't exist, create a new file
        std::ofstream outFile(logFilePath);
        if (outFile.is_open()) {
            // Write a header and the first entry
            outFile << "From,To,OrderNumber,Request\n";
            outFile << from << "," << to << "," << orderNumber << "," << request << "\n";
            std::cout << "Log file created with header and initial entry.\n";
        } else {
            std::cerr << "Error creating file.\n";
        }
    }
}


// int main() {
//     CheckPoint checkpoint;

//     // Example usage
//     checkpoint.createCheckPoint("123", "123" "456", "Sample Request");

//     return 0;
// } 
