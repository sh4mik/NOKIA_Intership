#ifndef NOKIA_INTERSHIP_CSV_DATA_H
#define NOKIA_INTERSHIP_CSV_DATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <variant>
#include <unordered_map>
#include <regex>
#include <cstring>

using std::string;
using std::vector;

class csv_data {
public:
    explicit csv_data(const std::string &filename) {
        read(filename);
    }

    int get(int lineNumber, const string &column) {
        string msg;
        int rowNumber;
        int columnNumber;
        try {
            msg = "out_of_bounds: get: rowNumber: " + std::to_string(lineNumber);
            rowNumber = rowDictionary.at(lineNumber);
            msg = "out_of_bounds: get: columnNumber: " + column;
            columnNumber = columnDictionary.at(column);
        } catch (const std::exception &ex) {
            throw std::invalid_argument(msg);
        }

        Cell &cell = data[rowNumber][columnNumber];
        return getValue(cell);
    }

    void fit() {
        for (auto &row : data) {
            for (int j = 0; j < columnNames.size() + 1; j++) {
                getValue(row[j]);
            }
        }
    }

    void printInOrder(std::ostream &fout) {
        // Print first row with column names
        fout << ",";
        for (int j = 0; j < columnNames.size(); j++) {
            fout << columnNames[j].exp << (j == columnNames.size() - 1 ? "" : ",");;
        }

        // Print other
        fout << std::endl;
        for (auto &row : data) {
            for (int j = 0; j < columnNames.size() + 1; j++) {
                fout << getValue(row[j]) << (j == columnNames.size() ? "" : ",");
            }
            fout << std::endl;
        }
    }


private:
    struct Cell {
        string exp;
        int value;
        bool isValueParsed = false;
        bool isOnCycle = false;
    };

    vector<vector<Cell>> data;
    vector<Cell> columnNames;

    std::unordered_map<string, int> columnDictionary;
    std::unordered_map<int, int> rowDictionary;

    static vector<Cell> parseLine(const string &line);

    void read(const string &filename);

    int getValue(Cell &cell);

    static bool isInteger(const string &line) {
        static const std::regex r(R"(^\-?\d+$)");
        return std::regex_match(line.data(), r);
    }

    static bool isOperation(const string &line) {
        static const std::regex r(R"(^=([A-Za-z]+)?(\d+)([\+\-\/\*])([A-Za-z]+)?(\d+)$)");
        return std::regex_match(line.data(), r);
    }

    int operation(const string &line);

};


#endif //NOKIA_INTERSHIP_CSV_DATA_H
