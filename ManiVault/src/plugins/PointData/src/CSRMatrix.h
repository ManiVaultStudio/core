#pragma once

#include "MatrixUtils.h"
#include <util/Serialization.h>
#include <DatasetTask.h>
#include "graphics/Vector2f.h"

#include <vector>
#include <iterator>

#include <QDebug>


#include <omp.h>

namespace mv_matrix
{

    class omp_scope_lock
    {
        omp_nest_lock_t& _lock;
    public:
        omp_scope_lock(omp_nest_lock_t& lock)
            :_lock(lock)
        {
            omp_set_nest_lock(&_lock);
        }

        ~omp_scope_lock()
        {
            omp_unset_nest_lock(&_lock);
        }
    };
    class omp_lock_manager
    {
        omp_nest_lock_t _lock;

        omp_lock_manager(const omp_lock_manager&) = delete;
    public:
        explicit omp_lock_manager()
        {
            omp_init_nest_lock(&_lock);
        }

        ~omp_lock_manager()
        {
            omp_destroy_nest_lock(&_lock);
        }

        omp_scope_lock lock_scope()
        {
            return omp_scope_lock(_lock);
        }
    };
    template<typename T >
    class CSRMatrix
    {
        
        // proxy class for matrix elements since we have to deal with sparsity.
        class element_type
        {
            CSRMatrix<T> &_matrix;
            row_size_type _row;
            column_size_type _column;
            element_type() = delete;
        public:
            element_type(const element_type& _rhs) = default;
            element_type(CSRMatrix<T>& matrix, row_size_type row, column_size_type column)
                : _matrix(matrix)
                , _row(row)
                , _column(column)
            { }

            explicit element_type(const CSRMatrix<T>& matrix, index_size_type index)
                : _matrix(const_cast<CSRMatrix<T>&>(matrix))
                , _row(index/matrix.rows())
                , _column(index%matrix.columns())
            {
            }

            template<typename U>
            operator U() const
            {
                return _matrix(_row, _column);
            }

            template<typename U>
            element_type& operator=(const U& value)
            {
                _matrix.set(_row, _column, value);
                return *this;
            }
        };

        friend class element_type;

    public:
       
        

        class const_iterator 
        {
        public:
            using iterator_tag = std::random_access_iterator_tag;
            using value_type = CSRMatrix<T>::element_type;
            using element_type = CSRMatrix<T>::element_type;
            using reference = element_type&;
            using difference_type = std::ptrdiff_t;

        protected:

            const CSRMatrix& _matrix;
            index_size_type _index;

        public:
            const_iterator(const CSRMatrix& matrix, std::size_t index = 0)
                : _matrix(matrix)
                , _index(index)
            {

            }

            const_iterator(const const_iterator& other) = default;
            
            /**  
            */
            auto operator*() const
            {
                
                return element_type(_matrix, _index);
            }

            /** Prefix increment ('++it').
            */
            auto& operator++()
            {
                ++_index;

                return *this;
            }

            /** Postfix increment ('it++').
            * \note Usually prefix increment ('++it') is preferable.
            */
            auto operator++(int)
            {
                auto result = *this;
                ++(*this);
                return result;
            }
            /** Prefix decrement ('--it').
            */
            auto& operator--()
            {
                --_index;
                return *this;
            }

            /** Postfix increment ('it--').
            * \note  Usually prefix increment ('--it') is preferable.
            */
            auto operator--(int)
            {
                auto result = *this;
                --(*this);
                return result;
            }

            /** Does (it += n) for iterator 'it' and integer value 'n'.
            */
            friend auto& operator+=(const_iterator& it, const difference_type n)
            {
                it._index += n;
                return it;
            }


            /** Returns (it1 - it2) for iterators it1 and it2.
            */
            friend difference_type operator-(const const_iterator& it1, const const_iterator& it2)
            {
                assert(&(it1._matrix) == &(it2._matrix));
                return it1._index - it2._index;
            }

            /** Returns (it + n) for iterator 'it' and integer value 'n'.
        */
            friend auto operator+(const_iterator it, const difference_type n)
            {
                return it += n;
            }


            /** Returns (n + it) for iterator 'it' and integer value 'n'.
            */
            friend auto operator+(const difference_type n, const_iterator it)
            {
                return it += n;
            }


            /** Returns (it - n) for iterator 'it' and integer value 'n'.
            */
            friend auto operator-(const_iterator it, const difference_type n)
            {
                return it += (-n);
            }


            /** Returns it[n] for iterator 'it' and integer value 'n'.
            */
            auto operator[](const difference_type n) const
            {
                return *(*this + n);
            }


            /** Returns (it1 == it2) for iterators it1 and it2.
            */
            friend bool operator==(const const_iterator& it1, const const_iterator& it2)
            {
                assert(&(it1._matrix) == &(it2._matrix));
                return it1._index == it2._index;
            }


            /** Returns (it1 != it2) for iterators it1 and it2.
            */
            friend bool operator!=(const const_iterator& it1, const const_iterator& it2)
            {
                return !(it1 == it2);
            }


            /** Returns (it1 < it2) for iterators it1 and it2.
            */
            friend bool operator<(const const_iterator& it1, const const_iterator& it2)
            {
                return it1._index < it2._index;
            }


            /** Returns (it1 > it2) for iterators it1 and it2.
            */
            friend bool operator>(const const_iterator& it1, const const_iterator& it2)
            {
                // Implemented just like the corresponding std::rel_ops operator.
                return it2 < it1;
            }


            /** Returns (it1 <= it2) for iterators it1 and it2.
            */
            friend bool operator<=(const const_iterator& it1, const const_iterator& it2)
            {
                // Implemented just like the corresponding std::rel_ops operator.
                return !(it2 < it1);
            }


            /** Returns (it1 >= it2) for iterators it1 and it2.
            */
            friend bool operator>=(const const_iterator& it1, const const_iterator& it2)
            {
                // Implemented just like the corresponding std::rel_ops operator.
                return !(it1 < it2);
            }


            /** Does (it -= n) for iterator 'it' and integer value 'n'.
            */
            friend auto& operator-=(const_iterator& it, const difference_type n)
            {
                it += (-n);
                return it;
            }
            
        };

        class iterator : public const_iterator
        {

            using difference_type = const_iterator::difference_type;

        public:
            iterator(CSRMatrix& matrix, std::size_t offset = 0)
                : const_iterator(matrix,offset)
            {

            }

            iterator() = default;

            iterator(const iterator& other)
                :const_iterator(other)
            {
            }

           
            T& operator*()
            {
                return element_type(this->_matrix, this->_index);
            }

        };

        // using iterators like this is not optimal, better to use visit functions
        __declspec(deprecated) iterator begin() 
        {
            return iterator(*this, 0);
        }

        __declspec(deprecated) iterator end()
        {
            return iterator(*this, this->rows() * this->columns());
        }

        __declspec(deprecated) const_iterator  begin() const
        {
            return const_iterator(*this, 0);
        }

        __declspec(deprecated) const_iterator end() const
        {
            return const_iterator(*this, this->rows() * this->columns());
        }

        __declspec(deprecated) const_iterator  cbegin() const
        {
            return const_iterator(*this, 0);
        }

        __declspec(deprecated) const_iterator cend() const
        {
            return const_iterator(*this, this->rows() * this->columns());
        }

    public:
       
        using Container = std::vector<T>;
        using difference_type = typename Container::difference_type;
        using value_type = typename Container::value_type;
        
    private:
        
        auto search_lower_bound(row_size_type row, column_size_type column) const
        {
            auto current = _colIndices.data() + _rowPointers[row];
            const auto last = _colIndices.data() + _rowPointers[row + 1];
            auto length = last - current;
            while (length > 0)
            {
                auto rem = length % 2;
                length /= 2;
                if (*current < column)
                    current += length + rem;
                
            }
            auto absolute_pos = current - _colIndices.data();
            return std::make_pair( (current != last) && (*current == column), absolute_pos);
        }

        index_size_type insert(row_size_type row, column_size_type column)
        {
            auto scope_lock = _lock_manager.lock_scope();
            const auto pos = search_lower_bound(row, column);
            if (!pos.first)
            {
                _colIndices.insert(_colIndices.begin()+pos.second, column);
                _values.insert(_values.begin()+pos.second, T());
                #pragma omp paralllel for
                for (std::ptrdiff_t i = row + 1; i < _rowPointers.size(); ++i) {
                    _rowPointers[i]++;
                }
               
            }
            return pos.second;
        }

        enum Index : std::ptrdiff_t { NoColumnIndex = -1 };
        inline std::ptrdiff_t column_index(size_t row, size_t column) const
        {
            auto found = search_lower_bound(row, column);
            if (found.first)
                return found.second;
            return NoColumnIndex;
        }

        inline value_type value(row_size_type row, column_size_type column) const
        {
            const auto idx = column_index(row, column);
            if (idx == NoColumnIndex)
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

        template<typename U>
        void set(row_size_type row, column_size_type column, const U& value)
        {
            auto scope_lock = _lock_manager.lock_scope();
            const auto idx = insert(row, column);
            _values[idx] = value;
        }

       

    public:

        static constexpr const char* data_type_name()
        {
            return mv_matrix::type_name<value_type>();
        }

        static constexpr const char* matrix_type_name()
        {
            return "Sparse";
        }



        CSRMatrix()
            :_numRows(0)
            ,_numCols(0)
        {
            
        }
        CSRMatrix(row_size_type numRows, column_size_type numCols, std::size_t numNonZero)
            : _numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(numRows + 1)
            , _colIndices(numNonZero)
            , _values(numNonZero)
        {
            
        }
        CSRMatrix(row_size_type numRows, column_size_type numCols, std::vector<row_size_type>& rowPointers, std::vector<column_size_type>& colIndices, std::vector<value_type>& values)
            :_numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(std::move(rowPointers))
            , _colIndices(std::move(colIndices))
            , _values(std::move(values))
        {
           
            qDebug() << "Num non zero: " << nonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }

        template<typename RowSizeType, typename ColumnSizeType, typename RowType, typename ColumnType, typename ValueType>
        CSRMatrix(RowSizeType numRows, ColumnSizeType numCols, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
            :_numRows(numRows)
            , _numCols(numCols)
        {
            setData(numRows, numCols, rowPointers, colIndices, values);
        }



        CSRMatrix(row_size_type numRows, column_size_type numCols, std::vector<row_size_type>& rowPointers, std::vector<column_size_type>& colIndices)
            :_numRows(numRows)
            , _numCols(numCols)
            , _rowPointers(std::move(rowPointers))
            , _colIndices(std::move(colIndices))
        {
            _values.resize(_colIndices.size());
            qDebug() << "Num non zero: " << nonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }

        CSRMatrix(const CSRMatrix &_other)
            :_values(_other._values)
            , _rowPointers(_other._rowPointers)
            , _colIndices(_other._colIndices)
            , _numRows(_other._numRows)
            , _numCols(_other._numCols)
        {
            
        }

        CSRMatrix(CSRMatrix&& _other)
            :_values(std::move(_other._values)), _rowPointers(std::move(_other._rowPointers)), _colIndices(std::move(_other._colIndices)), _numRows(std::move(_other._numRows)), _numCols(std::move(_other._numCols))
        {
            
        }

        CSRMatrix &operator=(CSRMatrix &&_other)
        {
            auto scope_lock = _lock_manager.lock_scope();
            _values = std::move(_other._values);
            _rowPointers = std::move(_other._rowPointers);
            _colIndices = std::move(_other._colIndices);
            _numRows = std::move(_other._numRows);
            _numCols = std::move(_other._numCols);
            return *this;
        }
        CSRMatrix& operator=(const CSRMatrix& _other)
        {
            auto scope_lock = _lock_manager.lock_scope();
            _values =_other._values;
            _rowPointers = _other._rowPointers;
            _colIndices = _other._colIndices;
            _numRows = _other._numRows;
            _numCols = _other._numCols;
            return *this;
        }

        row_size_type rows() const
        {
            return _numRows;
        }

        column_size_type columns() const
        {
            return _numCols;
        }

        std::size_t nonZeros() const 
        { 
            return _values.size(); 
        }

        value_type operator()(row_size_type row, column_size_type column) const
        {
            return value(row, column);
        }

        element_type operator()(row_size_type row, column_size_type column)
        {
            auto scope_lock = _lock_manager.lock_scope();
            return element_type(*this, row, column);
            
          
        }

    protected:

        template <typename FunctionObject, bool DenseVisit>
        void visitElements(FunctionObject functionObject, mv::DatasetTask *task = nullptr) const
        {
           
            if (task)
                task->setSubtasks(_numRows);
            for (std::size_t r = 0; r < _numRows; ++r)
            {
                const size_t nzEnd = _rowPointers[r + 1];
                if constexpr (DenseVisit)
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

        template <typename RowRange, typename FunctionObject, bool DenseVisit>
        void visitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
           
           if(task)
               task->setSubtasks(rows.size());
            for (const auto r : rows)
            {
                const size_t nzEnd = _rowPointers[r + 1];
                if constexpr (DenseVisit)
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

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool DenseVisit>
        void visitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask *task = nullptr) const
        {
            if (task)
                task->setSubtasks(rows.size());
            for (const auto r : rows)
            {
                for (auto col : columns)
                {
                    auto idx = column_index(r, col);
                    if ((idx == NoColumnIndex) && DenseVisit)
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


        template <typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            if (task)
                task->setSubtasks(_numRows);
            #pragma  omp parallel for schedule(dynamic,1)
            for (std::ptrdiff_t r = 0; r < _numRows; ++r)
            {
                const auto nzEnd = _rowPointers[r + 1];
                if constexpr (DenseVisit)
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
        
        template <typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                        auto idx = column_index(r, c);
                        if (idx == NoColumnIndex)
                        {
                            if constexpr (DenseVisit)
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
                visitElements<FunctionObject, DenseVisit>(functionObject, task);
          
        }

        template <typename RowRange, typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            const auto numRows = rows.size();
            if (task)
                task->setSubtasks(numRows);
            #pragma  omp parallel for
            for (std::ptrdiff_t i = 0; i < numRows; ++i)
            {
                const auto r = rows[i];
                const auto nzEnd = _rowPointers[r + 1];
                if constexpr (DenseVisit)
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

        template <typename RowRange, typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                       
                        auto idx = column_index(r, c);
                        if (idx == NoColumnIndex)
                        {
                            if constexpr (DenseVisit)
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
                visitElements<RowRange, FunctionObject, DenseVisit>(rows, functionObject,task);
            }
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool DenseVisit>
        void rowParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    auto idx = column_index(r, col);
                    if ((idx == NoColumnIndex) && DenseVisit)
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

        template <typename RowRange, typename ColumnRange, typename FunctionObject, bool DenseVisit>
        void columnParallelVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
                    auto idx = column_index(r, col);
                    if ((idx == NoColumnIndex) && DenseVisit)
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
        void sparseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, false>(functionObject, task);
        }

        template <typename FunctionObject>
        void denseVisitElements(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<FunctionObject, true>(functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void sparseVisitElements(const RowRange &rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,FunctionObject, false>(rows,functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void denseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,FunctionObject, true>(rows,functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void sparseVisitElements(const RowRange& rows, const ColumnRange &columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            visitElements<RowRange,ColumnRange,FunctionObject, false>(rows,columns,functionObject, task);
        }

        template <typename RowRange, typename ColumnRange, typename FunctionObject>
        void denseVisitElements(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
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
            rowParallelVisitElements<RowRange, FunctionObject, false>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void rowParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            rowParallelVisitElements<RowRange, FunctionObject, true>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelSparseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, false>(rows, functionObject, task);
        }

        template <typename RowRange, typename FunctionObject>
        void columnParallelDenseVisitElements(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
        {
            columnParallelVisitElements<RowRange, FunctionObject, true>(rows, functionObject, task);
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
            columnParallelVisitElements<RowRange, ColumnRange, FunctionObject, true>(rows, columns, functionObject, task);
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
        {
            auto begin = cbegin();
            auto end = cend();
            return functionObject(begin,end);
        }

        template <typename ReturnType = void, typename FunctionObject>
        ReturnType visitFromBeginToEnd(FunctionObject functionObject)
        {
            auto xbegin = begin();
            auto xend = end();
            return functionObject(xbegin, xend);
        }

        void extractFullDataForDimension(std::vector<float>& result, const column_size_type dimensionIndex, mv::DatasetTask *task=nullptr) const
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

        void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const column_size_type dimensionIndex1, const column_size_type dimensionIndex2, mv::DatasetTask *task=nullptr) const
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
        void extractDataForDimensions(std::vector<mv::Vector2f>& result, const column_size_type dimensionIndex1, const column_size_type dimensionIndex2, const RowRange& indices, mv::DatasetTask *task = nullptr) const
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
        void populateFullDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, mv::DatasetTask* task = nullptr) const
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
        void populateDataForDimensions(ResultContainer& resultContainer, const ColumnRange& dimensionIndices, const RowRange& indices, mv::DatasetTask* task = nullptr) const
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
        void convertData(const U* const data, const row_size_type numPoints, const column_size_type numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Converts the specified data to the internal data, using static_cast for each data element.
            /// Convenience overload, allowing an std::vector or an std::array as
            /// input data container.
        template <typename T>
        void convertData(const T& inputDataContainer, const row_size_type numDimensions)
        {
            convertData(inputDataContainer.data(), inputDataContainer.size() / numDimensions, numDimensions);
        }

        /// Copies the specified data into the internal data, sets the number of
        /// dimensions as specified, and sets the selected internal data type
        /// according to the specified data type T.
        template <typename T>
        void setData(const T* const data, const row_size_type numPoints, const column_size_type numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
        void setData(std::nullptr_t data, row_size_type numPoints, column_size_type numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }


        /// Copies the data from the specified vector into the internal data, sets
        /// the number of dimensions as specified, and sets the selected internal
        /// data type according to the specified data type T.
        template <typename U>
        void setData(const std::vector<U>& data, const column_size_type numDimensions)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }

        /// Efficiently "moves" the data from the specified vector into the internal
        /// data if possible, sets the number of dimensions as specified.
        template <typename U>
        void setData(std::vector<U>& data, const column_size_type numDimensions)
        {
            _numRows = data.size() / numDimensions;
            _numCols = numDimensions;
            // resize _rowPointers vector
            _rowPointers.assign(_numRows + 1,0);
            // count the number of non zero values per row
            #pragma omp parallel for schedule(dynamic,1)
            for (long long r = 0; r < _numRows; ++r)
            {
                decltype(_rowPointers)::value_type nonZeroCount = 0;
                std::size_t start = r * numDimensions;
                std::size_t end = start + _numCols;
                for (std::size_t index = start; index < end; ++index)
                {
                    nonZeroCount += (data[index] != 0);
                }
                _rowPointers[r + 1] = nonZeroCount;
            }

            // compute the total number of non zero values and set the _rowPointer offsets so they are cumulative
            std::size_t nonZeroes = _rowPointers[1];
            for (long long r = 2; r < (_numRows+1); ++r)
            {
                nonZeroes += _rowPointers[r];
                _rowPointers[r] += _rowPointers[r - 1];
            }
            
            // resize values and colIndices vectors
            _values.resize(nonZeroes);
            _colIndices.resize(nonZeroes);
            
            // copy the values and colIndices
            #pragma omp parallel for schedule(dynamic,1)
            for (long long r = 0; r < _numRows; ++r)
            {
                std::size_t start = r * numDimensions;
                std::size_t end = start + _numCols;
                std::size_t c = 0;
                std::size_t j = _rowPointers[r];
                for (std::size_t index = start; index < end; ++index, ++c)
                {
                    auto value = data[index];
                    if (value != 0)
                    {
                        _values[j] = value;
                        _colIndices[j] = c;
                        ++j;
                    }
                }
            }
        }
    
        template<typename RowSizeType, typename ColumnSizeType, typename RowType, typename ColumnType, typename ValueType>
        void setData(RowSizeType numRows, ColumnSizeType numCols, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
        {
            _numRows = numRows;
            _numCols = numCols;

            assert(_rowPointers.size() == (numRows + 1));
            if constexpr (std::is_same_v<decltype(rowPointers), decltype(_rowPointers)>)
            {
                _rowPointers = std::move(rowPointers);
            }
            else
            {
                _rowPointers = decltype(_rowPointers)(rowPointers.cbegin(), rowPointers.cend());
            }

            if constexpr (std::is_same_v<decltype(colIndices), decltype(_colIndices)>)
            {
                _colIndices = std::move(colIndices);
            }
            else
            {
                _colIndices = decltype(_colIndices)(colIndices.cbegin(), colIndices.cend());
            }

            if constexpr (std::is_same_v<decltype(values), decltype(_values)>)
            {
                _values = std::move(values);
            }
            else
            {
                _values = decltype(_values)(values.cbegin(), values.cend());
            }

            qDebug() << "Num non zero: " << nonZeros();
            qDebug() << "CSR vector sizes: " << _rowPointers.size() << _colIndices.size() << _values.size();
        }


        /**
           * Get amount of data occupied by the raw data
           * @return Size of the raw data in bytes
           */
        std::size_t bytes() const
        {
            return (mv_matrix::vectorSizeInBytes(_rowPointers) + mv_matrix::vectorSizeInBytes(_colIndices) + mv_matrix::vectorSizeInBytes(_values));
        }

        const Container& values() const
        {
            return _values;
        }

        Container& values()
        {
            return _values;
        }

        void resize(row_size_type rows, column_size_type columns)
        {
            const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
            throw MatrixException(mesg.c_str());
        }
        
        void resize(row_size_type numRows, column_size_type numCols, std::size_t numNonZeros, ...)
        {
            _numRows = numRows;
            _numCols = numCols;
            _colIndices.resize(numNonZeros);
            _values.resize(numNonZeros);
            _rowPointers.resize(numRows + 1);
        }

        

        const std::vector<column_size_type>& columnIndices() const
        {
            return columnIndices();
        }

        const std::vector<row_size_type> rowPointers() const
        {
            return _rowPointers;
        }


        QVariantMap toVariantMap() const
        {
            QVariantMap valuesData = mv::util::rawDataToVariantMap((const char*)_values.data(), mv_matrix::vectorSizeInBytes(_values), true);
            const auto valuesSize = _values.size();

            QVariantMap rowOffsetsData = mv::util::rawDataToVariantMap((const char*)_rowPointers.data(), mv_matrix::vectorSizeInBytes(_rowPointers), true);
            const auto rowOffsetsSize = _rowPointers.size();

            QVariantMap colIndicesData = mv::util::rawDataToVariantMap((const char*)_colIndices.data(), mv_matrix::vectorSizeInBytes(_colIndices), true);
            const auto colIndicesSize = _colIndices.size();
            


            return {
                { "Values", QVariant::fromValue(valuesData) },
                { "ValuesSize", QVariant::fromValue(valuesSize) },
                { "RowOffsets", QVariant::fromValue(rowOffsetsData) },
                { "RowOffsetsSize", QVariant::fromValue(rowOffsetsSize) },
                { "ColIndices", QVariant::fromValue(colIndicesData) },
                { "ColIndicesSize", QVariant::fromValue(colIndicesSize) },
                { "NumberOfDimensions", QVariant::fromValue(_numCols) },
                { "NumberOfPoints", QVariant::fromValue(_numRows)}
            };
        }

        void fromRawDataBuffer(const std::vector<char>& bytes)
        {
            size_t offset = 0;
            std::vector<size_t> rowPointers(_rowPointers.size());
            std::memcpy(rowPointers.data(), bytes.data() + offset, mv_matrix::vectorSizeInBytes(rowPointers));
            std::copy(rowPointers.cbegin(), rowPointers.cend(), _rowPointers.begin());


            offset += mv_matrix::vectorSizeInBytes(rowPointers);
            std::vector<size_t> colIndices(_colIndices.size());
            std::memcpy(colIndices.data(), bytes.data() + offset, mv_matrix::vectorSizeInBytes(colIndices));
            std::copy(colIndices.cbegin(), colIndices.cend(), _colIndices.begin());

            offset += mv_matrix::vectorSizeInBytes(colIndices);
            std::vector<float> values(_values.size());
            std::memcpy(values.data(), bytes.data() + offset, mv_matrix::vectorSizeInBytes(values));
            std::copy(values.cbegin(), values.cend(), _values.begin());
        }

        void fromVariantMap(const QVariantMap& variantMap)
        {
            mv::util::variantMapMustContain(variantMap, "Values");
            mv::util::variantMapMustContain(variantMap, "ValuesSize");
            mv::util::variantMapMustContain(variantMap, "RowOffsets");
            mv::util::variantMapMustContain(variantMap, "RowOffsetsSize");
            mv::util::variantMapMustContain(variantMap, "ColIndices");
            mv::util::variantMapMustContain(variantMap, "ColIndicesSize");
            mv::util::variantMapMustContain(variantMap, "NumberOfDimensions");
            mv::util::variantMapMustContain(variantMap, "NumberOfPoints");
            
            const auto valuesData = variantMap["Values"].toMap();
            const auto valuesSize = variantMap["ValuesSize"].toULongLong();
            _values.resize(valuesSize);
            mv::util::populateDataBufferFromVariantMap(valuesData, (char*)_values.data());

            const auto rowOffsetsData = variantMap["RowOffsets"].toMap();
            const auto rowOffsetsSize = variantMap["RowOffsetsSize"].toULongLong();
            _rowPointers.resize(rowOffsetsSize);
            mv::util::populateDataBufferFromVariantMap(rowOffsetsData, (char*)_rowPointers.data());

            const auto colIndicesData = variantMap["ColIndices"].toMap();
            const auto colIndicesSize = variantMap["ColIndicesSize"].toULongLong();
            _colIndices.resize(colIndicesSize);
            mv::util::populateDataBufferFromVariantMap(colIndicesData, (char*)_colIndices.data());


            _numRows = static_cast<row_size_type>(variantMap["NumberOfPoints"].toULongLong());
            _numCols = static_cast<column_size_type>(variantMap["NumberOfDimensions"].toULongLong());
        }



    private:
        row_size_type _numRows;
        column_size_type _numCols;
        omp_lock_manager _lock_manager;
        

        std::vector<row_size_type> _rowPointers;
        std::vector<column_size_type> _colIndices;
        std::vector<value_type> _values;
        static constexpr const value_type _sparseValue = {};
    };


   


}

namespace std
{
    /*
    template<typename T>
    auto begin(mv_matrix::CSRMatrix<T>& matrix)
    {
        return matrix.dense_begin();
    }

    template<typename T>
    auto begin(const mv_matrix::CSRMatrix<T>& matrix) const
    {
        return matrix.dense_begin();
    }

    template<typename T>
    auto end(mv_matrix::CSRMatrix<T>& matrix)
    {
        return matrix.dense_end();
    }

    template<typename T>
    auto end(const mv_matrix::CSRMatrix<T>& matrix) const
    {
        return matrix.dense_end();
    }

    */
}
//#include "SparseMatrix.cpp"
