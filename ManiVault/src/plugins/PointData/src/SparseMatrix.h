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

template<typename RowIndexType, typename ColIndexType, typename ValueType>
class SparseMatrix
{
public:
    SparseMatrix();
    SparseMatrix(size_t numRows, size_t numCols, size_t numNonZero);

    size_t getNumRows() const { return _numRows; }
    size_t getNumCols() const { return _numCols; }
    size_t getNumNonZeros() const { return _numNonZero; }

    const std::vector<RowIndexType>& getIndexPointers() const { return _rowPointers; }
    const std::vector<ColIndexType>& getColIndices() const { return _colIndices; }
    const std::vector<ValueType>& getValues() const { return _values; }

    void setData(size_t numRows, size_t numCols, const std::vector<RowIndexType>& rowPointers, const std::vector<ColIndexType>& colIndices, const std::vector<ValueType>& values);
    void setData(size_t numRows, size_t numCols, std::vector<RowIndexType>&& rowPointers, std::vector<ColIndexType>&& colIndices, std::vector<ValueType>&& values);

    SparseRow<ColIndexType, ValueType> getSparseRow(size_t rowIndex);
    std::vector<ValueType> getDenseRow(size_t rowIndex) const;

    //SparseRow<ColIndexType, ValueType> getSparseCol(ColIndexType colIndex);
    std::vector<ValueType> getDenseCol(ColIndexType colIndex) const;

private:
    size_t _numRows = 0;
    size_t _numCols = 0;
    size_t _numNonZero = 0;

    std::vector<RowIndexType> _rowPointers = {};
    std::vector<ColIndexType> _colIndices = {};
    std::vector<ValueType> _values = {};
};

template<typename RowIndexType, typename ColIndexType, typename ValueType>
SparseMatrix<RowIndexType, ColIndexType, ValueType>::SparseMatrix()
{

}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
SparseMatrix<RowIndexType, ColIndexType, ValueType>::SparseMatrix(size_t numRows, size_t numCols, size_t numNonZero) :
    _numRows(numRows),
    _numCols(numCols),
    _numNonZero(numNonZero)
{
    _rowPointers.resize(numRows + 1);
    _colIndices.resize(numNonZero);
    _values.resize(numNonZero);
}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
void SparseMatrix<RowIndexType, ColIndexType, ValueType>::setData(size_t numRows, size_t numCols, const std::vector<RowIndexType>& rowPointers, const std::vector<ColIndexType>& colIndices, const std::vector<ValueType>& values)
{
    _numRows = numRows;
    _numCols = numCols;
    _numNonZero = colIndices.size();
    _rowPointers = rowPointers;
    _colIndices = colIndices;
    _values = values;

    qDebug() << "Num non zero: " << _numNonZero;
    qDebug() << "CSR vector sizes: " << _rowPointers.size() << ", " << _colIndices.size() << ", " << _values.size();
}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
void SparseMatrix<RowIndexType, ColIndexType, ValueType>::setData(size_t numRows, size_t numCols, std::vector<RowIndexType>&& rowPointers, std::vector<ColIndexType>&& colIndices, std::vector<ValueType>&& values)
{
    _numRows = numRows;
    _numCols = numCols;
    _numNonZero = colIndices.size();
    _rowPointers = std::move(rowPointers);
    _colIndices = std::move(colIndices);
    _values = std::move(values);

    qDebug() << "Num non zero: " << _numNonZero;
    qDebug() << "CSR vector sizes: " << _rowPointers.size() << ", " << _colIndices.size() << ", " << _values.size();
}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
SparseRow<ColIndexType, ValueType> SparseMatrix<RowIndexType, ColIndexType, ValueType>::getSparseRow(size_t rowIndex)
{
    size_t nzStart = _rowPointers[rowIndex];
    size_t nzEnd = _rowPointers[rowIndex + 1];

    SparseRow<RowIndexType, ColIndexType, ValueType> row;
    row._colIndices = std::vector<ColIndexType>(_colIndices.begin() + nzStart, _colIndices.begin() + nzEnd);
    row._values = std::vector<ValueType>(_values.begin() + nzStart, _values.begin() + nzEnd);

    return row;
}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
std::vector<ValueType> SparseMatrix<RowIndexType, ColIndexType, ValueType>::getDenseRow(size_t rowIndex) const
{
    size_t nzStart = _rowPointers[rowIndex];
    size_t nzEnd = _rowPointers[rowIndex + 1];

    std::vector<ValueType> row(_numCols, 0);

    for (size_t nzIndex = nzStart; nzIndex < nzEnd; nzIndex++)
    {
        ColIndexType col = _colIndices[nzIndex];
        ValueType val = _values[nzIndex];
        row[col] = val;
    }

    return row;
}

//SparseRow<RowIndexType, ColIndexType, ValueType> SparseMatrix<RowIndexType, ColIndexType, ValueType>::getSparseCol(ColIndexType colIndex)
//{
//
//}

template<typename RowIndexType, typename ColIndexType, typename ValueType>
std::vector<ValueType> SparseMatrix<RowIndexType, ColIndexType, ValueType>::getDenseCol(ColIndexType colIndex) const
{
    std::vector<ValueType> col(_numRows, 0);

    for (size_t rowIndex = 0; rowIndex < _numRows; rowIndex++)
    {
        size_t nzStart = _rowPointers[rowIndex];
        size_t nzEnd = _rowPointers[rowIndex + 1];

        for (size_t nzIndex = nzStart; nzIndex < nzEnd; nzIndex++)
        {
            if (_colIndices[nzIndex] == colIndex)
                col[rowIndex] = _values[nzIndex];
        }
    }

    return col;
}
