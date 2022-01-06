/*
 *
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include "timeseries.h"

void TimeSeries::pushToTableByIndex(int col, int row, float val) {
    this->table[col][row] = val;
}
float TimeSeries::getValByIndex(int col, int row) const {
    return this->getTable()[col][row];
}
float TimeSeries::getValByColName(const std::string &colName, int row) const {
    int col = -1;
    for (int i = 0; i < getColNames().size(); ++i) {
        if (colName == getColNames().at(i)) {
            col = i;
            break;
        }
    }
    if (col == -1) {
        throw std::runtime_error("element not found");
    }
    return getTable()[col][row];
}
const vector<std::string> &TimeSeries::getColNames() const {
    return colNames;
}

std::vector<float> TimeSeries::getCol(int i) const {
    return table.at(i);
}
const vector<vector<float>> &TimeSeries::getTable() const {
    return table;
}
void TimeSeries::loadByColumns() {
    vector<float> row;
    string line, word;
    // extract the first line in file for column names
    getline(fin, line);
    stringstream s(line);
    while (getline(s, word, ',')) {
        colNames.push_back(word);
    }
    vector<vector<float>> table(colNames.size(), vector<float>());
    // copy each line in file to variable line.
    while (getline(fin, line)) {
        stringstream stringStream(line);
        // push to column vector each row element
        for (auto &i: table) {
            getline(stringStream, word, ',');
            i.push_back(stof(word));
        }
    }
    this->table = table;
}

std::vector<float> TimeSeries::getRow(int i) const {
    std::vector<float> row;
    for (const auto &j: getTable()) {
        row.push_back(j.at(i));
    }
    return row;
}
fstream& TimeSeries::getFin() {
    return this->fin;
}
TimeSeries::TimeSeries() {}
void TimeSeries::initialize(const char *CSVfileName) {
    fin.open(CSVfileName, ios_base::in);
    if (!fin.is_open()) {
        throw runtime_error("Could not open file");
    }
    // load file to table
    loadByColumns();
}
