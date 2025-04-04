#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

class FManager {
private:
    std::string directoryPath;

public:
    // Constructor to set the directory path
    explicit FManager(const std::string& path) : directoryPath(path) {
        if (!std::filesystem::exists(directoryPath)) {
            std::cerr << "Directory does not exist: " << directoryPath << std::endl;
        }
    }

    // Read the contents of a file into a string
	std::vector<std::string> readFile(const std::string& filename) const {
		std::ifstream file(directoryPath + "/" + filename);
		std::vector<std::string> lines;
		
		if (!file.is_open()) {
			std::cerr << "Error: Unable to open file " << filename << std::endl;
			return lines;  // Return an empty vector if file can't be opened
		}

		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		
		return lines;
	}

    // Write a string to a file (overwrites if file exists)
    bool writeFile(const std::string& filename, const std::string& content) const {
        std::ofstream file(directoryPath + "/" + filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to write to file " << filename << std::endl;
            return false;
        }
        file << content;
        return true;
    }

    // Append content to a file
    bool appendToFile(const std::string& filename, const std::string& content) const {
        std::ofstream file(directoryPath + "/" + filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to append to file " << filename << std::endl;
            return false;
        }
        file << content;
        return true;
    }

    // Check if a file exists in the directory
    bool fileExists(const std::string& filename) const {
        return std::filesystem::exists(directoryPath + "/" + filename);
    }

    // Delete a file from the directory
    bool deleteFile(const std::string& filename) const {
        return std::filesystem::remove(directoryPath + "/" + filename);
    }

    // List all files in the directory
    void listFiles() const {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            std::cout << entry.path().filename().string() << std::endl;
        }
    }
};
