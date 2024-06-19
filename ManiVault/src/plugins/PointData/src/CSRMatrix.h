#pragma once

#include <vector>
#include "graphics/Vector2f.h"
#include <QDebug>

#include <omp.h>
namespace mv
{

    template<typename ColIndexType, typename ValueType>
    class SparseRow
    {
    public:
        std::vector<ColIndexType> _colIndices;
        std::vector<ValueType> _values;
    };

    class MatrixException : public std::exception {
    private:
        const char* message;

    public:
        MatrixException(const char* msg) : message(msg) {}
        const char* what() {
            return message;
        }
    };

    template<typename ValueType, typename ColIndexType = std::uint16_t >
    class CSRMatrix
    {
       
    public:
       
        using Container = std::vector<ValueType>;
        using size_type = typename Container::size_type;
        using difference_type = typename Container::difference_type;
        using value_type = ValueType;

    private:
        enum Index : std::ptrdiff_t { NoIndex = -1 };
        inline std::ptrdiff_t index(size_t row, size_t column) const
        {
            auto current = _colIndices.data() + _rowPointers[row];
            auto last = _colIndices.data() + _rowPointers[row + 1];
            auto length = last - current;
            while (length > 0)
            {
                auto half = length / 2;
                current += current[half] < column ? length - half : 0;
                length = half;
            }
            if (*current == column)
                return column;
            return NoIndex;
        }

        inline value_type value(size_t row, size_t column) const
        {
            auto idx = index(row, column);
            if (idx == NoIndex)
                return _sparseValue;
            return _values[idx];
        }

        
        inline bool parallelVisitIsFaster() const
        {
            const auto totalNumElements = _numRows * _numCols;
            const int max_threads = omp_get_max_threads();
            if (omp_get_max_threads() == 0)
                return false;
            return (max_threads && (totalNumElements / max_threads) < _values.size());
        }

    public:
        CSRMatrix()
            :_numRows(0)
            ,_numCols(0)
        {
            
        }
        CSRMatrix(size_t numRows, size_t numCols, size_t numNonZero)
            :_numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(numRows + 1)
            , _colIndices(numNonZero)
            , _values(numNonZero)
        {
            
        }
        CSRMatrix(size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<ColIndexType>&& colIndices, std::vector<ValueType>&& values)
            :_numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(std::move(rowPointers))
            , _colIndices(std::move(colIndices))
            , _values(std::move(values))
        {
            qDebug() << "Num non zero: " << getNumNonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }

        CSRMatrix(size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<ColIndexType>&& colIndices)
            :_numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(std::move(rowPointers))
            , _colIndices(std::move(colIndices))
        {
            _values.resize(_colIndices.size());
            qDebug() << "Num non zero: " << getNumNonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }

        CSRMatrix(const CSRMatrix &_other)
            :_values(_other._values), _rowPointers(_other._rowPointers), _colIndices(_other._colIndices), _numRows(_other._numRows), _numCols(_other._numCols)
        {
            
        }

        CSRMatrix(CSRMatrix&& _other)
            :_values(std::move(_other._values)), _rowPointers(std::move(_other._rowPointers)), _colIndices(std::move(_other._colIndices)), _numRows(std::move(_other._numRows)), _numCols(std::move(_other._numCols))
        {

        }


        CSRMatrix &operator=(CSRMatrix &&_other)
        {
            _values = std::move(_other._values);
            _rowPointers = std::move(_other._rowPointers);
            _colIndices = std::move(_other._colIndices);
            _numRows = std::move(_other._numRows);
            _numCols = std::move(_other._numCols);
            return *this;
        }
        CSRMatrix& operator=(const CSRMatrix& _other)
        {
            _values =_other._values;
            _rowPointers = _other._rowPointers;
            _colIndices = _other._colIndices;
            _numRows = _other._numRows;
            _numCols = _other._numCols;
            return *this;
        }

        size_t getNumRows() const { return _numRows; }
        size_t getNumCols() const { return _numCols; }
        size_t getNumNonZeros() const { return _values.size(); }

        const std::vector<size_t>& getIndexPointers() const { return _rowPointers; }
        const std::vector<ColIndexType>& getColIndices() const { return _colIndices; }
        const std::vector<ValueType>& getValues() const { return _values; }

        
        SparseRow<ColIndexType, ValueType> getSparseRow(unsigned int rowIndex)
        {
            size_t nzStart = _rowPointers[rowIndex];
            size_t nzEnd = _rowPointers[rowIndex + 1];

            SparseRow<ColIndexType, ValueType> row;
            row._colIndices = std::vector<ColIndexType>(_colIndices.begin() + nzStart, _colIndices.begin() + nzEnd);
            row._values = std::vector<ValueType>(_values.begin() + nzStart, _values.begin() + nzEnd);

            return row;
        }
        std::vector<float> getDenseRow(unsigned int rowIndex) const
        {
            size_t nzStart = _rowPointers[rowIndex];
            size_t nzEnd = _rowPointers[rowIndex + 1];

            std::vector<float> row(_numCols, 0);
#pragma omp parallel for
            for (std::ptrdiff_t nzIndex = nzStart; nzIndex < nzEnd; nzIndex++)
            {
                ColIndexType col = _colIndices[nzIndex];
                ValueType val = _values[nzIndex];
                row[col] = static_cast<float>(val);
            }

            return row;
        }

        //SparseRow<ColIndexType, ValueType> getSparseCol(unsigned int colIndex);
        std::vector<float> getDenseCol(unsigned int colIndex) const
        {
            std::vector<float> col(_numRows, 0);

#pragma omp parallel for
            for (std::ptrdiff_t rowIndex = 0; rowIndex < _numRows; rowIndex++)
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


        size_type rows() const
        {
            return _numRows;
        }

        size_type columns() const
        {
            return _numCols;
        }

        value_type operator()(size_t row, size_t column) const
        {
            return value(row, column);
        }

        value_type &operator()(size_t row, size_t column)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
            /*
            const auto idx = index(row, column);
            if(idx >=0)
                return value(row, column);
            else
            {
               
            }
            */
        }

    protected:

        template <typename FunctionObject, bool IsDense>
        void visitElements(FunctionObject functionObject, DatasetTask *task = nullptr) const
        {
           
            if (task)
                task->setSubtasks(_numRows);
            for (std::size_t r = 0; r < _numRows; ++r)
            {
                const size_t nzEnd = _rowPointers[r + 1];
                if constexpr (IsDense)
                {
                    std::size_t col = 0;
                    for (std::ptrdiff_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        auto currentCol = _colIndices[nzIndex];
                        
                        for (; col < currentCol; ++col)
                            functionObject(r, col, _sparseValue);
                        
                        functionObject(r, currentCol, _values[nzIndex]);
                        ++col;
                    }
                    for (; col < _numCols; ++col)
                        functionObject(r, col, _sparseValue);

                  
                }
                else
                {
                    for (std::ptrdiff_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto value = _values[nzIndex];
                        const auto column = _colIndices[nzIndex];
                        functionObject(r,column, value);
                    }
                }
                if(task)
                    task->subtaskFinished(r);
            }
            if (task)
                task->setFinished();
        }

        template <typename RowRange, typename FunctionObject, bool IsDense>
        void visitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
           
           if(task)
               task->setSubtasks(rows.size());
            for (const auto r : rows)
            {
                const size_t nzEnd = _rowPointers[r + 1];
                if constexpr (IsDense)
                {
                    std::size_t col = 0;
                    for (std::ptrdiff_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        auto currentCol = _colIndices[nzIndex];

                        for (; col < currentCol; ++col)
                            functionObject(r, col, _sparseValue);

                        functionObject(r, currentCol, _values[nzIndex]);
                        ++col;
                    }
                    for (; col < _numCols; ++col)
                        functionObject(r, col, _sparseValue);
                }
                else
                {
                    for (std::ptrdiff_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto value = _values[nzIndex];
                        const auto column = _colIndices[nzIndex];
                        functionObject(r, column, value);
                    }
                }

                if (task)
                    task->subtaskFinished(r);
            }
            if (task)
                task->setFinished();
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool IsDense>
        void visitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask *task = nullptr) const
        {
            if (task)
                task->setSubtasks(rows.size());
            for (const auto r : rows)
            {
                for (auto col : columns)
                {
                    auto idx = index(r, col);
                    if ((idx == NoIndex) && IsDense)
                        functionObject(r, col, _sparseValue);
                    else
                        functionObject(r, col, _values[idx]);
                }

                if (task)
                    task->subtaskFinished(r);
            }
            if (task)
                task->setFinished();
        }


        template <typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numRows);
            #pragma  omp parallel for
            for (std::ptrdiff_t r = 0; r < _numRows; ++r)
            {
                const auto nzEnd = _rowPointers[r + 1];
                if constexpr (IsDense)
                {
                    std::size_t col = 0;
                    for (std::size_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto currentCol = _colIndices[nzIndex];
                        for (; col < currentCol; ++col)
                            functionObject(r, col, _sparseValue);
                        functionObject(r, currentCol, _values[nzIndex]);
                        ++col;
                    }
                    for (; col < _numCols; ++col)
                        functionObject(r, col, _sparseValue);
                }
                else
                {
                    for (std::size_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto value = _values[nzIndex];
                        const auto currentCol = _colIndices[nzIndex];
                        functionObject(r, currentCol, value);
                    }
                }

                if (task)
                    task->subtaskFinished(r);
            }
            if (task)
                task->setFinished();
        }
        
        template <typename FunctionObject, bool IsDense>
        void columnParallelVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if (parallelVisitIsFaster())
            {
                if (task)
                    task->setSubtasks(_numCols);
                #pragma omp parallel for schedule(dynamic,1)
                for (std::ptrdiff_t c = 0; c < _numCols; ++c)
                {
                    for (std::size_t r = 0; r < _numRows; ++r)
                    {
                        auto idx = index(r, c);
                        if (idx == NoIndex)
                        {
                            if constexpr (IsDense)
                                functionObject(r, c, _sparseValue);
                        }
                        else
                            functionObject(r, c, _values[idx]);
                    }
                    task->subtaskFinished(c);
                }
                if (task)
                    task->setFinished();
            }
            else
                visitElements<FunctionObject, IsDense>(functionObject, task);
          
        }

        template <typename RowRange, typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            const auto numRows = rows.size();
            if (task)
                task->setSubtasks(numRows);
            #pragma  omp parallel for
            for (std::ptrdiff_t i = 0; i < numRows; ++i)
            {
                const auto r = rows[i];
                const auto nzEnd = _rowPointers[r + 1];
                if constexpr (IsDense)
                {
                    std::size_t col = 0;
                    for (std::size_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto currentCol = _colIndices[nzIndex];
                        for (; col < currentCol; ++col)
                            functionObject(r, col, _sparseValue);
                        functionObject(r, currentCol, _values[nzIndex]);
                        ++col;
                    }
                    for (; col < _numCols; ++col)
                        functionObject(r, col, _sparseValue);
                }
                else
                {
                    for (std::size_t nzIndex = _rowPointers[r]; nzIndex < nzEnd; nzIndex++)
                    {
                        const auto value = _values[nzIndex];
                        const auto currentCol = _colIndices[nzIndex];
                        functionObject(r, currentCol, value);
                    }
                }

                task->subtaskFinished(i);
            }
            if (task)
                task->setFinished();
        }

        template <typename RowRange, typename FunctionObject, bool IsDense>
        void columnParallelVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if(parallelVisitIsFaster())
            {
                if (task)
                    task->setSubtasks(_numCols);
                #pragma omp parallel for schedule(dynamic,1)
                for (std::ptrdiff_t c = 0; c < _numCols; ++c)
                {
                    for (const auto r : rows)
                    {
                       
                        auto idx = index(r, c);
                        if (idx == NoIndex)
                        {
                            if constexpr (IsDense)
                                functionObject(r, c, _sparseValue);
                        }
                        else
                            functionObject(r, c, _values[idx]);
                    }

                    task->subtaskFinished(c);
                }
                if (task)
                    task->setFinished();
            }
            else
            {
                visitElements<RowRange, FunctionObject, IsDense>(rows, functionObject,task);
            }
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            const auto numRows = rows.size();
            if (task)
                task->setSubtasks(numRows);
            #pragma  omp parallel for
            for (std::ptrdiff_t i = 0; i < numRows; ++i)
            {
                const auto r = rows[i];
                for (auto col : columns)
                {
                    auto idx = index(r, col);
                    if ((idx == NoIndex) && IsDense)
                        functionObject(r, col, _sparseValue);
                    else
                        functionObject(r, col, _values[idx]);
                }

                if (task)
                    task->subtaskFinished(i);
            }
            if (task)
                task->setFinished();
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool IsDense>
        void columnParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            const auto numCols = columns.size();
            if (task)
                task->setSubtasks(numCols);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < numCols; ++i)
            {
                const auto col = columns[i];
                for (const auto r : rows)
                {
                    auto idx = index(r, col);
                    if ((idx == NoIndex) && IsDense)
                        functionObject(r, col, _sparseValue);
                    else
                        functionObject(r, col, _values[idx]);
                }

                if (task)
                    task->subtaskFinished(i);
            }
        }

    public:

        template <typename FunctionObject>
        void sparseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void denseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void sparseVisitElements(const RowRange &rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void denseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,FunctionObject, true>(rows,functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void sparseVisitElements(const RowRange& rows, const ColumnRange &columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,ColumnRange,FunctionObject, false>(rows,columns,functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void denseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename FunctionObject>
        void rowParallelSparseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void rowParallelDenseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename FunctionObject>
        void columnParallelSparseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void columnParallelDenseVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, FunctionObject, false>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, FunctionObject, true>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, false>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, true>(rows, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void rowParallelSparseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, ColumnRange, FunctionObject, false>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, ColumnRange, FunctionObject, true>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, ColumnRange, FunctionObject, false>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, ColumnRange, FunctionObject, true>(rows, columns, functionObject, task);
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
        {
            assert(false);
            return functionObject(_values.cbegin(), _values.cend());
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType visitFromBeginToEnd(FunctionObject functionObject)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex, DatasetTask *task=nullptr) const
        {
            
            result.resize(_numRows);
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; row++)
            {
                result[row] = value(row, dimensionIndex);
                if (task)
                    task->subtaskFinished(row);
            }
            if (task)
                task->setFinished();
        }

        void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, DatasetTask *task=nullptr) const
        {
            result.resize(_numRows);
            if (task)
                task->setSubtasks(_numRows);
#pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; row++)
            {

                result[row].set(value(row, dimensionIndex1), value(row, dimensionIndex2));
                if (task)
                    task->subtaskFinished(row);
            }
            if (task)
                task->setFinished();
        }

        template<typename RowRange>
        void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const RowRange& indices, DatasetTask *task = nullptr) const
        {
            const auto size = indices.size();
            result.resize(size);
            if (task)
                task->setSubtasks(size);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < size; i++)
            {
                const auto row = indices[i];
                result[i].set(value(row, dimensionIndex1), value(row, dimensionIndex2));
                if (task)
                    task->subtaskFinished(i);
            }
            if (task)
                task->setFinished();
        }

        template <typename ResultContainer, typename ColumnRange>
        void populateFullDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numRows);
            const std::size_t numDims = dimensionIndices.size();
#pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; ++row)
            {
                const std::size_t container_offset = row * numDims;
                for (std::size_t d = 0; d < numDims; ++d)
                {
                    resultContainer[container_offset + d] = value(row, dimensionIndices[d]);
                }

                if (task)
                    task->subtaskFinished(row);
            }
            if (task)
                task->setFinished();
        }

        template <typename ResultContainer, typename ColumnRange, typename RowRange>
        void populateDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, const RowRange& indices, DatasetTask* task = nullptr) const
        {
            const std::ptrdiff_t numIndices{ static_cast<std::ptrdiff_t>(indices.size()) };
            const std::size_t numDims = dimensionIndices.size();
            if (task)
                task->setSubtasks(numIndices);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < numIndices; ++i)
            {
                const auto row = indices[i];
                const auto container_offset = (i * numDims);
                for (std::size_t d = 0; d < numDims; ++d)
                {
                    resultContainer[container_offset + d] = value(row, dimensionIndices[d]);
                }
                if (task)
                    task->subtaskFinished(i);
            }
            if (task)
                task->setFinished();
        }

        /// Converts the specified data to the internal data, using static_cast for
        /// each data element. Sets the number of dimensions as specified. Ensures
        /// that the size of the internal data buffer corresponds to the number of
        /// points.
        /// \note This function does not affect the selected internal data type.
        template <typename U>
        void convertData(const U* const data, const std::size_t numPoints, const std::size_t numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Converts the specified data to the internal data, using static_cast for each data element.
            /// Convenience overload, allowing an std::vector or an std::array as
            /// input data container.
        template <typename T>
        void convertData(const T& inputDataContainer, const std::size_t numDimensions)
        {
            convertData(inputDataContainer.data(), inputDataContainer.size() / numDimensions, numDimensions);
        }

        /// Copies the specified data into the internal data, sets the number of
        /// dimensions as specified, and sets the selected internal data type
        /// according to the specified data type T.
        template <typename T>
        void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
        void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }


        /// Copies the data from the specified vector into the internal data, sets
        /// the number of dimensions as specified, and sets the selected internal
        /// data type according to the specified data type T.
        template <typename T>
        void setData(const std::vector<T>& data, const std::size_t numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Efficiently "moves" the data from the specified vector into the internal
        /// data, sets the number of dimensions as specified, and sets the selected
        /// internal data type according to the specified data type T.
        template <typename T>
        void setData(std::vector<T>&& data, const std::size_t numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }
    

        /**
           * Get amount of data occupied by the raw data
           * @return Size of the raw data in bytes
           */
        std::size_t bytes() const
        {
            if (_rowPointers.empty())
                return 0;
            return (_values.size() * sizeof(value_type)) + (_colIndices.size() * sizeof(ColIndexType)) + (_rowPointers.size() * sizeof(_rowPointers[0]));
        }

        const Container& values() const
        {
            return _values;
        }

        Container& values()
        {
            return _values;
        }

        void resize(size_type rows, size_type columns)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        void sparse_resize(size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<ColIndexType>&& colIndices)
            
        {
            if((_rowPointers.size() *_colIndices.size()) ==0)
            {
                _numRows = 0;
                _numCols = 0;
                _rowPointers.clear();
                _colIndices.clear();
                _values.clear();
            }
            else
            {
                assert(numRows = (_rowPointers.size() - 1));
                _numRows = numRows;
                _numCols = numCols;
                _rowPointers = std::move(rowPointers);
                _colIndices = std::move(colIndices);
                _values.resize(_colIndices.size());
            }
            
            qDebug() << "Num non zero: " << getNumNonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }
        

        const std::vector<ColIndexType>& columnIndices() const
        {
            return columnIndices();
        }

        const std::vector<std::size_t> rowPointers() const
        {
            return _rowPointers;
        }

    private:
        size_t _numRows;
        size_t _numCols;
        

        std::vector<size_t> _rowPointers;
        std::vector<ColIndexType> _colIndices;
        std::vector<ValueType> _values;
        const ValueType _sparseValue{};
    };


}
//#include "SparseMatrix.cpp"
