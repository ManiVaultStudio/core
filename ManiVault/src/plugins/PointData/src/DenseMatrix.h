#pragma once

#include "MatrixUtils.h"
#include "graphics/Vector2f.h"
#include "DatasetTask.h"
#include <util/Serialization.h>
#include <utility>
#include <vector>

namespace mv_matrix
{
    template<typename T>
    class DenseMatrix
    {
       
    public:
        using Container = std::vector<T>;
        using difference_type = typename Container::difference_type;
        using value_type = typename Container::value_type;
      

    private:
        inline std::size_t index(row_size_type row, column_size_type column) const
        {
            return ((row * _numCols) + column);
        }
    public:

        static constexpr const char* data_type_name()
        {
            return mv_matrix::type_name<value_type>();
        }

        static constexpr const char* matrix_type_name()
        {
            return "Full";
        }

        


        DenseMatrix()
            :_numRows(0), _numCols(0)
        {}

        DenseMatrix(row_size_type rows, column_size_type columns)
            :_values(rows* columns), _numRows(rows), _numCols(columns)
        {
        }

        DenseMatrix(row_size_type rows, column_size_type columns, const T& value)
            :_values(rows* columns, value), _numRows(rows), _numCols(columns)
        {
        }

        DenseMatrix(row_size_type rows, column_size_type columns, std::vector<T>& values)
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
        row_size_type rows() const
        {
            return _numRows;
        }


        column_size_type columns() const
        {
            return _numCols;
        }

        value_type operator()(row_size_type row, column_size_type column) const
        {
            return _values[index(row, column)];
        }

        value_type &operator()(row_size_type row, column_size_type column)
        {
            return _values[index(row, column)];
        }

        std::size_t getNumNonZeros() const
        {
            return _numCols * _numCols;
        }

    protected:
        template <typename FunctionObject, bool DenseVisit>
        void visitElements(FunctionObject functionObject, mv::DatasetTask *task = nullptr) const
        {
            typename Container::size_type idx = 0;
            if (task)
                task->setSubtasks(_numRows);
            for (std::ptrdiff_t r = 0; r < _numRows; ++r, ++idx)
            {
                for (std::size_t c = 0; c < _numCols; ++c, ++idx)
                {
                    const auto value = _values[idx];
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(r);
            }
        }


        template <typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(r);
            }
        }

        template <typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(c);
            }
        }

        template <typename RowRange, typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(i);
            }
        }

        


        template <typename RowRange, typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(c);
            }
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
                    {
                        functionObject(r, c, value);
                    }
                }
                if (task)
                    task->subtaskFinished(i);
            }
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    if (DenseVisit || (value != 0))
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
        void sparseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void denseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename FunctionObject>
        void rowParallelSparseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void rowParallelDenseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename FunctionObject>
        void columnParallelSparseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void columnParallelDenseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange,FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, FunctionObject, true>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, true>(rows,functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void rowParallelSparseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, ColumnRange, FunctionObject, false>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, ColumnRange, FunctionObject, true>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, ColumnRange, FunctionObject, false>(rows, columns, functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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

        void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex, mv::DatasetTask* task = nullptr) const
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

        void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, mv::DatasetTask* task = nullptr) const
        {
            result.resize(_numRows);
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; row++)
            {
                auto row_offset = index(row, 0);
                result[row].set(_values[row_offset + dimensionIndex1], _values[row_offset + dimensionIndex2]);
                if (task)
                    task->subtaskFinished(row);
            }
        }

        template<typename RowRange>
        void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const RowRange& indices, mv::DatasetTask* task = nullptr) const
        {
            const auto indicesSize = indices.size();
            result.resize(indicesSize);
            if (task)
                task->setSubtasks(indicesSize);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t i = 0; i < indicesSize; i++)
            {
                auto row_offset = index(indices[i], 0);
                result[i].set(_values[row_offset + dimensionIndex1], _values[row_offset+ dimensionIndex2]);
                if (task)
                    task->subtaskFinished(i);
            }
        }

        template <typename ResultContainer, typename ColumnRange>
        void populateFullDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, mv::DatasetTask* task = nullptr) const
        {
            const std::size_t numDims = dimensionIndices.size();
            if (task)
                task->setSubtasks(_numRows);
            #pragma omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t row = 0; row < _numRows; ++row)
            {
                const auto row_offset = index(row, 0);
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
        void populateDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, const RowRange& indices, mv::DatasetTask* task = nullptr) const
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
        void convertData(const U* const data, const row_size_type numPoints, const column_size_type numDimensions)
        {
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values.resize(numElements);
            #pragma  omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t e = 0; e < numElements; ++e)
            {
                _values[e] = static_cast<T>(data[e]);
            }
        }

        /// Converts the specified data to the internal data, using static_cast for each data element.
        /// Convenience overload, allowing an std::vector or an std::array as
        /// input data container.
        template <typename U>
        void convertData(const U& inputDataContainer, const column_size_type numDimensions)
        {
            convertData(inputDataContainer.data(), inputDataContainer.size() / numDimensions, numDimensions);
        }

        /// Copies the specified data into the internal data, sets the number of
        /// dimensions as specified, and sets the selected internal data type
        /// according to the specified data type T.
        template <typename U>
        void setData(const U* const data, const row_size_type numPoints, const column_size_type numDimensions)
        {
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values = Container(data, data + numElements);
        }


        /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
        void setData(std::nullptr_t data, row_size_type numPoints, column_size_type numDimensions)
        {
            _numRows = numPoints;
            _numCols = numDimensions;
            const auto numElements = numPoints * numDimensions;
            _values.assign(numElements, T());
        }


        

        /// Efficiently sets the data from the specified vector into the internal
        /// data, sets the number of dimensions as specified.
        template <typename U>
        void setData(std::vector<U>& data, const column_size_type numDimensions)
        {
            _numRows = data.size() / numDimensions;
            _numCols = numDimensions;
            assert((_numRows * _numCols) == data.size());
            if constexpr (std::is_same_v<U,T>)
                _values =std::move(data);
            else
            {
                _values.resize(data.size());
                std::copy(data.cbegin(), data.cend(), _values.begin());
            }
        }

        /// Efficiently sets the data from the specified vector into the internal
       /// data, sets the number of dimensions as specified.
        template<typename RowSizeType, typename ColumnSizeType, typename RowType, typename ColumnType, typename ValueType>
        void setData(RowSizeType numRows, ColumnSizeType numCols, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
        {
            _numRows = mv_matrix::safe_numeric_cast<decltype(_numRows)>(numRows);
            _numCols = mv_matrix::safe_numeric_cast<decltype(_numCols)>(numCols);
            // to make sure computation of number of values goes ok we do it in 64 bit unsigned.
            std::uint64_t numValues = numRows;
            numValues *= numCols;
            _values.assign(numValues, 0);

            #pragma omp parallel for schedule(dynamic,1)
            for (int64_t r = 0; r < _numRows; ++r)
            {
                auto start = rowPointers[r];
                auto end = rowPointers[r+1];

                const std::size_t offset = r * _numCols;
                for (auto i = start; i < end; ++i)
                {
                    auto column = colIndices[i];
                    if (column < _numCols)
                    {
                        double value = values[i];
                        _values[offset + column] = value;
                    }
                }
            }
        }
     
   
        /**
        * Get amount of data occupied by the raw data
        * @return Size of the raw data in bytes
        */
        std::size_t bytes() const
        {
            return mv_matrix::vectorSizeInBytes(_values);
        }

        const Container & values() const
        {
            return _values;
        }

        Container& values()
        {
            return _values;
        }

        
        void resize(row_size_type numRows, column_size_type numCols, ...)
        {
            _numRows = numRows;
            _numCols = numCols;
            _values.resize(_numRows * _numCols);
        }

        void fromRawDataBuffer(const std::vector<char>& buffer)
        {
            assert(buffer.size() == bytes());
            std::memcpy(_values.data(), buffer.data(), mv_matrix::vectorSizeInBytes(_values));
        }

        QVariantMap toVariantMap() const
        {
            QVariantMap rawData = mv::util::rawDataToVariantMap((const char*)_values.data(),mv_matrix::vectorSizeInBytes(_values), true);

            return {
                { "Values", QVariant::fromValue(rawData) },
                { "NumberOfDimensions", QVariant::fromValue(_numCols) },
                { "NumberOfPoints", QVariant::fromValue(_numRows) }
            };
        }

        void fromVariantMap(const QVariantMap& variantMap)
        {
            mv::util::variantMapMustContain(variantMap, "Values");
            mv::util::variantMapMustContain(variantMap, "NumberOfDimensions");
            mv::util::variantMapMustContain(variantMap, "NumberOfPoints");
            
            _numRows = static_cast<row_size_type>(variantMap["NumberOfPoints"].toULongLong());
            _numCols = static_cast<column_size_type>(variantMap["NumberOfDimensions"].toULongLong());
            const auto valuesData = variantMap["Values"].toMap();

            _values.resize(_numRows * _numCols);
            mv::util::populateDataBufferFromVariantMap(valuesData, (char*)_values.data());
        }



    private:
        Container _values;
        row_size_type _numRows;
        column_size_type _numCols;
    };
}
