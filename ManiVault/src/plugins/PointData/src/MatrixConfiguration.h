#pragma once
#include "pointdata_export.h"

#include <QMetaType>
#include <biovault_bfloat16/biovault_bfloat16.h>
#include "MatrixUtils.h"

namespace mv_matrix
{
    


    class POINTDATA_EXPORT  Configuration
    {
        std::string _storageType;
        std::string _dataType;

       

    public:
        Configuration();

        Configuration(const Configuration& other);

        Configuration(const std::string& storageTypeName, const std::string& dataTypeName);
        Configuration(const char* storageTypeName, const char* dataTypeName);

        Configuration(const std::string& dataTypeName);

        Configuration(const char *dataTypeName);

        template<typename T>
        Configuration(const std::string& storageTypeName, T)
            : _storageType(storageTypeName)
            , _dataType(mv_matrix::type_name<T>())
        {
            static_assert(!std::is_pointer_v<T>);
        }

        template<typename T>
        Configuration(std::nullptr_t, T)
            : Configuration()
        {
            setDataTypeTo<T>();
        }

        void set(const std::string& storageTypeName, const std::string& dataTypeName);

        const std::string &storageType() const;

        const std::string &dataType() const;
        
        void setDataType(const std::string name);

        template<typename T>
        void setDataTypeTo()
        {
            _dataType = mv_matrix::type_name<T>();
        }

        bool operator==(const Configuration& other) const;
      
        

        template<typename Matrix>
        bool matches() const
        {
            return (_storageType == Matrix::matrix_type_name()) && (_dataType == Matrix::data_type_name());
        }
    };

    bool operator==(const Configuration& c1, const Configuration& c2);
    
}