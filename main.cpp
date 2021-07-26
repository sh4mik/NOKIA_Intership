#include <iostream>
#include "csv_data.h"


int main(int argc, char *argv[]) {
    try {
        csv_data csvData(argv[1]);
        csvData.fit();
        csvData.printInOrder(std::cout);
    } catch (const std::exception &ex) {
        std::cout << "ERROR: ";
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
