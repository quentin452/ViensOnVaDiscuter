#include <cstdlib>
#include <iostream>

int main() {
    std::cout << "Starting Desktop Companion..." << std::endl;
    
    // Try to run the Python script from the project root
    int result = system("python src/main.py");
    
    if (result != 0) {
        std::cout << "Failed to run from src/main.py, trying alternative paths..." << std::endl;
        
        // Try other possible locations
        result = system("python main.py");
        if (result != 0) {
            result = system("python ../src/main.py");
        }
    }
    
    if (result != 0) {
        std::cerr << "Could not find or run main.py file!" << std::endl;
        std::cerr << "Make sure Python is installed and main.py exists." << std::endl;
    }
    
    return result;
}