/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
using std::endl;
using std::fstream;
using std::string;
using std::stringstream;
using std::ios_base;
using std::runtime_error;
using std::cout;
using std::vector;

class TimeSeries {
  /**
   * table hold vector of vectors of float.
   * colNames hold vector of column names, the name of the i column is colNames[i].
   * fin is an in-stream to raed csv file to table.
   */
  vector<vector<float>> table;
  vector<std::string> colNames;
  fstream fin;
  void loadByColumns();
 public:

  /**
   * initialize file-in stream
   * @param CSVfileName name of csv file to read values from
   */
  explicit TimeSeries(const char *CSVfileName) noexcept(false) {
      initialize(CSVfileName);
  }
  void initialize(const char *CSVfileName);
  /**
   * get row from table.
   * @param i index of row
   * @return row in index i
   */
  std::vector<float> getRow(int i) const;
  /**
   * get column from table.
   * @param i index of column.
   * @return column in index i.
   */
  std::vector<float> getCol(int i) const;
  /**
   *
   * @return table
   */
  const vector<vector<float>> &getTable() const;
  /**
   * return the value of table at (row,col)
   * @param col index if column
   * @param row index of row
   * @return element (row,col) in table
   */
  float getValByIndex(int col, int row) const;
  /**
   * return the value of table by column name.
   * @param colName column name
   * @param row index of row
   * @return element (row, colName) in table
   */
  float getValByColName(const std::string &colName, int row) const;
  /**
   *
   * @return return colNames vector
   */
  const vector<std::string> &getColNames() const;
  /**
   * push new element to table at the specify index
   * @param row index of rox
   * @param col index of column
   * @param val value to insert
   */
  void pushToTableByIndex(int row, int col, float val);
  fstream &getFin();
  TimeSeries();

};

#endif /* TIMESERIES_H_ */
