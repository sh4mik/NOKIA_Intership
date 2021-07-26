//
// Created by mi on 26.07.2021.
//

#include "csv_data.h"

vector<csv_data::Cell> csv_data::parseLine(const string &line) {
    vector<Cell> res;

    char *pch = strtok(const_cast<char *>(line.data()), ",");
    while (pch != nullptr) {
        res.push_back({string(pch), 0, false, false});
        pch = strtok(nullptr, " ,");
    }
    string lastStr = res.back().exp;
    res.back().exp = lastStr.substr(0, lastStr.size() - 1);
    return res;
}

void csv_data::read(const string &filename) {
    std::ifstream fin;
    fin.open(filename);

    if (!fin.is_open()) {
        throw std::invalid_argument("read: Can't find file!");
    }

    // Read first line and add words to {ColumnName -> ColumnNumber} map
    string line;
    getline(fin, line);
    if (line[0] != ',') {
        throw std::invalid_argument("read: wrong table format: missing first comma");
    }
    columnNames = parseLine(line);
    for (int i = 0; i < columnNames.size(); i++) {
        columnDictionary.emplace(columnNames[i].exp, i + 1);
    }

    // Read other lines and add to the columns
    while (getline(fin, line)) {
        vector<Cell> parsedLine = parseLine(line);
        if (parsedLine.size() != columnNames.size() + 1) {
            throw std::invalid_argument("read: Wrong count of commas in line: " + line);
        }

        int lineNumber;
        if (isInteger(parsedLine[0].exp)) {
            lineNumber = std::stoi(parsedLine[0].exp);
            parsedLine[0].value = lineNumber;
            rowDictionary.emplace(lineNumber, data.size());
        } else {
            throw std::invalid_argument("read: Number of line is not Integer");
        }

        data.emplace_back(parsedLine);
    }
}

int csv_data::getValue(csv_data::Cell &cell) {
    if (cell.isOnCycle) {
        throw std::invalid_argument("getValue: Cycle on table");
    }

    if (cell.isValueParsed) {
        return cell.value;
    }
    if (isInteger(cell.exp)) {
        cell.value = std::stoi(cell.exp);
        cell.isValueParsed = true;
        return cell.value;
    }

    cell.isOnCycle = true;
    int res;
    if (isOperation(cell.exp)) {
        res = operation(cell.exp);
        cell.value = res;
        cell.isValueParsed = true;
    } else {
        throw std::invalid_argument("getValue: Not correct expression on table");
    }
    cell.isOnCycle = false;
    return cell.value;
}

int csv_data::operation(const string &line) {
    static const std::regex r(R"(^=([A-Za-z]+)?(\d+)([\+\-\/\*])([A-Za-z]+)?(\d+)$)");
    std::smatch m;
    if (regex_search(line, m, r)) {

        // for readability
        string &&column1 = m[1];
        int number1 = std::stoi(m[2]);
        char sign = m[3].str().front();
        string &&column2 = m[4];
        int number2 = std::stoi(m[5]);

        int arg1 = column1.empty() ? number1 : get(number1, column1);
        int arg2 = column2.empty() ? number2 : get(number2, column2);

        switch (sign) {
            case '+':
                return arg1 + arg2;
            case '-':
                return arg1 - arg2;
            case '*':
                return arg1 * arg2;
            case '/':
                if (arg2 == 0) {
                    throw std::invalid_argument("Operation: division by zero");
                }
                return arg1 / arg2;
            default:
                throw std::invalid_argument("Operation: Wrong operation sign format");;
        }
    } else {
        throw std::invalid_argument("Operation: Wrong csv-table column format");
    }
}
