#include "MatrixConfiguration.h"
#include "DenseMatrix.h"

namespace mv_matrix
{
    Configuration::Configuration()
        : _storageType(mv_matrix::DenseMatrix<float>::matrix_type_name())
        , _dataType(mv_matrix::DenseMatrix<float>::data_type_name())
    {

    }

    Configuration::Configuration(const Configuration& other)
        : _storageType(other._storageType)
        , _dataType(other._dataType)
    {

    }

    Configuration::Configuration(const std::string& storageTypeName, const std::string& dataTypeName)
        : _storageType(storageTypeName)
        , _dataType(dataTypeName)
    {

    }

    Configuration::Configuration(const char* storageTypeName, const char* dataTypeName)
    {
        assert(storageTypeName != nullptr);
        assert(dataTypeName != nullptr);
        if (storageTypeName != nullptr)
        {
            _storageType = storageTypeName;
        }
        else
        {
            throw std::out_of_range("Configuration: parameter should not be a Null pointer");
        }
        if (dataTypeName != nullptr)
        {
            _dataType = dataTypeName;
        }
        else
        {
            throw std::out_of_range("Configuration: parameter should not be a Null pointer");
        }
    }

    Configuration::Configuration(const std::string& dataTypeName)
    : _storageType(mv_matrix::DenseMatrix<float>::matrix_type_name())
    , _dataType(dataTypeName)
        
    {

    }
    Configuration::Configuration(const char* dataTypeName)
        : _storageType(mv_matrix::DenseMatrix<float>::matrix_type_name())
    {
        if (dataTypeName == nullptr)
        {
            throw std::out_of_range("Configuration: parameter should not be a null pointer");
        }
        _dataType = dataTypeName;
    }

    void Configuration::set(const std::string& storageTypeName, const std::string& dataTypeName)
    {
        _storageType = storageTypeName;
        _dataType = dataTypeName;
    }


    const std::string& Configuration::storageType() const
    {
        return _storageType;
    }

    const std::string& Configuration::dataType() const
    {
        return _dataType;
    }

    void Configuration::setDataType(const std::string name)
    {
        _dataType = name;
    }

    bool Configuration::operator==(const Configuration& other) const
    {
        return (_storageType == other._storageType) && (_dataType == other._storageType);
    }
    
    bool operator==(const Configuration& c1, const Configuration& c2)
    {
        return (c1.operator==(c2));
    }

    
}
