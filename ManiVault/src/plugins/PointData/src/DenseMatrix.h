#pragma once
#include <utility>
#include <vector>
#include "graphics/Vector2f.h"
#include "PointData.h"
namespace mv
{
    template<typename T>
    class DenseMatrix
    {
       
    public:
        using Container = std::vector<T>;
        using size_type = typename Container::size_type;
        using difference_type = typename Container::difference_type;
        using value_type = T;

    private:
        inline size_type index(size_type row, size_type column) const
        {
            return ((row * _numCols) + column);
        }
    public:
        DenseMatrix()
            :_numRows(0), _numCols(0)
        {}

        DenseMatrix(size_type rows, size_type columns)
            :_values(rows* columns), _numRows(rows), _numCols(columns)
        {
        }

        DenseMatrix(size_type rows, size_type columns, const T& value)
            :_values(rows* columns, value), _numRows(rows), _numCols(columns)
        {
        }

        DenseMatrix(size_type rows, size_type columns, std::vector<T>&& values)
            :_values(std::move(values)), _numRows(rows), _numCols(columns)
        {
            assert(rows * columns == _values.size());
        }

        DenseMatrix(const DenseMatrix &_other)
            :_values(_other._values), _numRows(_other._numRows), _numCols(_other._numCols)
        {
        }

        DenseMatrix(DenseMatrix&& _other)
            :_values(std::move(_other._values)), _numRows(std::move(_other._numRows)), _numCols(std::move(_other._numCols))
        {
            
        }

        DenseMatrix& operator=(const DenseMatrix& _other)
        {
            _values = _other._values;
            _numRows = _other._numRows;
            _numCols = _other._numCols;
            return *this;
        }

        DenseMatrix &operator=(DenseMatrix &&_other)
        {
            _values = std::move(_other._values);
            _numRows = std::move(_other._numRows);
            _numCols = std::move(_other._numCols);
            return *this;
        }

        // generalized Matrix Interface
        size_type rows() const
        {
            return _numRows;
        }


        size_type columns() const
        {
            return _numCols;
        }

        value_type operator()(size_type row, size_type column) const
        {
            return _values[index(row, column)];
        }

        value_type &operator()(size_type row, size_type column) 
        {
            return _values[index(row, column)];
        }

        size_type getNumNonZeros() const
        {
            return _numCols * _numCols;
        }

    protected:
        template <typename FunctionObject, bool IsDense>
        void visitElements(FunctionObject functionObject, DatasetTask *task = nullptr) const
        {
            typename Container::size_type idx = 0;
            if (task)
                task->setSubtasks(_numRows);
            for (std::ptrdiff_t r = 0; r < _numRows; ++r, ++idx)
            {
                for (std::size_t c = 0; c < _numCols; ++c, ++idx)
                {
                    const auto value = _values[idx];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(r);
            }
        }


        template <typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t r = 0; r < _numRows; ++r)
            {
                std::size_t row_offset = index(r, 0);
                for (std::size_t c = 0; c < _numCols; ++c)
                {
                    const auto value = _values[row_offset + c];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(r);
            }
        }

        template <typename FunctionObject, bool IsDense>
        void columnParallelVisitElements(FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numCols);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t c = 0; c < _numCols; ++c)
            {
                std::size_t idx = c;
                for (std::size_t r = 0; r < _numRows; ++r, idx += _numCols)
                {
                    const auto value = _values[idx];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(c);
            }
        }

        template <typename RowRange, typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            const auto numRows = rows.size();
            if (task)
                task->setSubtasks(numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < numRows; ++i)
            {
                const auto r = rows[i];
                std::size_t idx = r * _numCols;
                for (std::size_t c = 0; c < _numCols; ++c, ++idx)
                {
                    const auto value = _values[idx];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(i);
            }
        }

        


        template <typename RowRange, typename FunctionObject, bool IsDense>
        void columnParallelVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numCols);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t c = 0; c < _numCols; ++c)
            {
                for (const auto r : rows)
                {
                    const std::size_t idx = index(r, c);
                    const auto value = _values[idx];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(c);
            }
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool IsDense>
        void rowParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            const auto numRows = rows.size();
            if (task)
                task->setSubtasks(numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < numRows; ++i)
            {
                const auto r = rows[i];
                std::size_t row_offset = r * _numCols;
                for (const auto c : columns)
                {
                    const auto value = _values[row_offset + c];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(i);
            }
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
                const auto c = columns[i];
                for (const auto r : rows)
                {
                    const auto value = _values[index(r,c)];
                    if (IsDense || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
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
            rowParallelVisitElements<RowRange,FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, FunctionObject, true>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, true>(rows,functionObject, task);
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
            columnParallelVisitElements<RowRange, ColumnRange, FunctionObject, true>(rows, columns, functionObject,task);
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
        {
            return functionObject(std::cbegin(_values), std::cend(_values));
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType visitFromBeginToEnd(FunctionObject functionObject)
        {
            return functionObject(std::begin(_values), std::end(_values));
        }

        void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex, DatasetTask* task = nullptr) const
        {
            result.resize(_numRows);
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; row++)
            {
                result[row] = _values[index(row, dimensionIndex)];
                if (task)
                    task->subtaskFinished(row);
            }
        }

        void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, DatasetTask* task = nullptr) const
        {
            result.resize(_numRows);
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; row++)
            {
                size_type row_offset = index(row, 0);
                result[row].set(_values[row_offset + dimensionIndex1], _values[row_offset + dimensionIndex2]);
                if (task)
                    task->subtaskFinished(row);
            }
        }

        template<typename RowRange>
        void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const RowRange& indices, DatasetTask* task = nullptr) const
        {
            const auto indicesSize = indices.size();
            result.resize(indicesSize);
            if (task)
                task->setSubtasks(indicesSize);
#pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < indicesSize; i++)
            {
                size_type row_offset = index(indices[i], 0);
                result[i].set(_values[row_offset + dimensionIndex1], _values[row_offset+ dimensionIndex2]);
                if (task)
                    task->subtaskFinished(i);
            }
        }

        template <typename ResultContainer, typename ColumnRange>
        void populateFullDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, DatasetTask* task = nullptr) const
        {
            const std::size_t numDims = dimensionIndices.size();
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; ++row)
            {
                const size_type row_offset = index(row, 0);
                const std::size_t container_offset = row * numDims;
                for (std::size_t d=0; d < numDims; ++d)
                {
                    resultContainer[container_offset + d] = _values[row_offset + dimensionIndices[d]];
                }
                if (task)
                    task->subtaskFinished(row);
            }
        }

        template <typename ResultContainer, typename ColumnRange, typename RowRange>
        void populateDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, const RowRange& indices, DatasetTask* task = nullptr) const
        {
            const std::ptrdiff_t numIndices{ static_cast<std::ptrdiff_t>(indices.size()) };
            const std::size_t numDims = dimensionIndices.size();
            if (task)
                task->setSubtasks(numIndices);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i=0; i < numIndices; ++i)
            {
                const auto row = indices[i];
                const auto row_offset = index(row, 0);
                const auto container_offset = (i * numDims);
                for (std::size_t d = 0; d < numDims; ++d)
                {
                    resultContainer[container_offset + d] = _values[row_offset + dimensionIndices[d]];
                }
                if (task)
                    task->subtaskFinished(i);
            }
        }

        /// Converts the specified data to the internal data, using static_cast for
        /// each data element. Sets the number of dimensions as specified. Ensures
        /// that the size of the internal data buffer corresponds to the number of
        /// points.
        /// \note This function does not affect the selected internal data type.
        template <typename U>
        void convertData(const U* const data, const std::size_t numPoints, const std::size_t numDimensions)
        {
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values.resize(numElements);
            #pragma  omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t e = 0; e < numElements; ++e)
            {
                _values[e] = static_cast<std::remove_reference_t<decltype(U)>>(data[e]);
            }
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
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values = Container(data, data + numElements);
        }


        /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
        void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions)
        {
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values.resize(numElements, T());
        }


        /// Copies the data from the specified vector into the internal data, sets
        /// the number of dimensions as specified, and sets the selected internal
        /// data type according to the specified data type T.
        template <typename T>
        void setData(const std::vector<T>& data, const std::size_t numDimensions)
        {
            _numRows = data.size() / numDimensions;
            _numCols = numDimensions;
            assert((_numRows * _numCols) == data.size());
            if constexpr (std::is_same_v<T, Container::value_type>)
                _values = data;
            else
            {
                _values.resize(data.size());
                std::copy(data.cbegin(), data.cend(), _values.begin());
            }
                
        }

        /// Efficiently "moves" the data from the specified vector into the internal
        /// data, sets the number of dimensions as specified, and sets the selected
        /// internal data type according to the specified data type T.
        template <typename T>
        void setData(std::vector<T>&& data, const std::size_t numDimensions)
        {
            _numRows = data.size() / numDimensions;
            _numCols = numDimensions;
            assert((_numRows * _numCols) == data.size());
            if constexpr (std::is_same_v<T, Container::value_type>)
                _values =std::move(data);
            else
            {
                _values.resize(data.size());
                std::copy(data.cbegin(), data.cend(), _values.begin());
            }
        }

     
   
        /**
        * Get amount of data occupied by the raw data
        * @return Size of the raw data in bytes
        */
        std::size_t bytes() const
        {
            return (_values.size() * sizeof(T));
        }

        const Container & values() const
        {
            return _values;
        }

        Container& values()
        {
            return _values;
        }

        void resize(size_type rows, size_type columns)
        {
            _numRows = rows;
            _numCols = columns;
            _values.resize(_numRows * _numCols);
        }

        template<typename ColIndexType>
        void sparse_resize(size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<ColIndexType>&& colIndices)
        {
            resize(numRows, numCols);
        }
    private:
        Container _values;
        size_type _numRows;
        size_type _numCols;
    };
}
