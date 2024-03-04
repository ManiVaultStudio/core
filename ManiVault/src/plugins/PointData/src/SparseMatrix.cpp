#pragma once

#include "SparseMatrix.h"

template<typename ColIndexType, typename ValueType>
SparseMatrix<ColIndexType, ValueType>::SparseMatrix()
{

}

template<typename ColIndexType, typename ValueType>
SparseMatrix<ColIndexType, ValueType>::SparseMatrix(size_t numRows, size_t numCols, size_t numNonZero) :
    _numRows(numRows),
    _numCols(numCols),
    _numNonZero(numNonZero)
{
    _rowPointers.resize(numRows + 1);
    _colIndices.resize(numNonZero);
    _values.resize(numNonZero);
}

template<typename ColIndexType, typename ValueType>
void SparseMatrix<ColIndexType, ValueType>::setData(size_t numRows, size_t numCols, std::vector<size_t> rowPointers, std::vector<ColIndexType> colIndices, std::vector<ValueType> values)
{
    _numRows = numRows;
    _numCols = numCols;
    _numNonZero = colIndices.size();
    qDebug() << "Num non zero: " << _numNonZero;
    qDebug() << "CSR vector sizes: " << rowPointers.size() << colIndices.size() << values.size();
    _rowPointers = rowPointers;
    _colIndices = colIndices;
    _values = values;
}

template<typename ColIndexType, typename ValueType>
SparseRow<ColIndexType, ValueType> SparseMatrix<ColIndexType, ValueType>::getSparseRow(unsigned int rowIndex)
{
    size_t nzStart = _rowPointers[rowIndex];
    size_t nzEnd = _rowPointers[rowIndex + 1];

    SparseRow<ColIndexType, ValueType> row;
    row._colIndices = std::vector<ColIndexType>(_colIndices.begin() + nzStart, _colIndices.begin() + nzEnd);
    row._values = std::vector<ValueType>(_values.begin() + nzStart, _values.begin() + nzEnd);

    return row;
}

template<typename ColIndexType, typename ValueType>
std::vector<float> SparseMatrix<ColIndexType, ValueType>::getDenseRow(unsigned int rowIndex) const
{
    size_t nzStart = _rowPointers[rowIndex];
    size_t nzEnd = _rowPointers[rowIndex + 1];

    std::vector<float> row(_numCols, 0);

    for (size_t nzIndex = nzStart; nzIndex < nzEnd; nzIndex++)
    {
        ColIndexType col = _colIndices[nzIndex];
        ValueType val = _values[nzIndex];
        row[col] = static_cast<float>(val);
    }

    return row;
}

//SparseRow<ColIndexType, ValueType> SparseMatrix<ColIndexType, ValueType>::getSparseCol(unsigned int colIndex)
//{
//
//}

template<typename ColIndexType, typename ValueType>
std::vector<float> SparseMatrix<ColIndexType, ValueType>::getDenseCol(unsigned int colIndex) const
{
    std::vector<float> col(_numRows, 0);

    for (size_t rowIndex = 0; rowIndex < _numRows; rowIndex++)
    {
        size_t nzStart = _rowPointers[rowIndex];
        size_t nzEnd = _rowPointers[rowIndex + 1];

        for (size_t nzIndex = nzStart; nzIndex < nzEnd; nzIndex++)
        {
            if (_colIndices[nzIndex] == colIndex)
                col[rowIndex] = static_cast<float>(_values[nzIndex]);
        }
    }

    return col;
}
