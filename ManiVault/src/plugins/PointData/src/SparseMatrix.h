#pragma once

#include <vector>

#include <QDebug>

template<typename ColIndexType, typename ValueType>
class SparseRow
{
public:
    std::vector<ColIndexType> _colIndices;
    std::vector<ValueType> _values;
};

//template<typename ColIndexType, typename ValueType>
//class SparseCol
//{
//public:
//    std::vector<ColIndexType> _colIndices;
//    std::vector<ValueType> _values;
//};

template<typename ColIndexType, typename ValueType>
class SparseMatrix
{
public:
    SparseMatrix();
    SparseMatrix(size_t numRows, size_t numCols, size_t numNonZero);

    size_t getNumRows() const { return _numRows; }
    size_t getNumCols() const { return _numCols; }
    size_t getNumNonZeros() const { return _numNonZero; }

    const std::vector<size_t>& getIndexPointers() const { return _rowPointers; }
    const std::vector<ColIndexType>& getColIndices() const { return _colIndices; }
    const std::vector<ValueType>& getValues() const { return _values; }

    void setData(size_t numRows, size_t numCols, std::vector<size_t> rowPointers, std::vector<ColIndexType> colIndices, std::vector<ValueType> values);

    SparseRow<ColIndexType, ValueType> getSparseRow(unsigned int rowIndex);
    std::vector<float> getDenseRow(unsigned int rowIndex) const;

    //SparseRow<ColIndexType, ValueType> getSparseCol(unsigned int colIndex);
    std::vector<float> getDenseCol(unsigned int colIndex) const;

private:
    size_t _numRows;
    size_t _numCols;
    size_t _numNonZero;

    std::vector<size_t> _rowPointers;
    std::vector<ColIndexType> _colIndices;
    std::vector<ValueType> _values;
};

#include "SparseMatrix.cpp"
