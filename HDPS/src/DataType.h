#ifndef HDPS_DATATYPE_H
#define HDPS_DATATYPE_H

#include <QString>
#include <QList>

namespace hdps
{
    class DataType
    {
    public:
        DataType() = delete;

        DataType(QString type) :
            _type(type)
        { }

        inline friend bool operator==(const DataType& lhs, const DataType& rhs) { return lhs._type == rhs._type; }
        inline friend bool operator!=(const DataType& lhs, const DataType& rhs) { return !(lhs == rhs); }
        inline friend QString operator+(const QString& lhs, const DataType& rhs) { return lhs + rhs._type; }

    private:
        QString _type;

        friend class RawData;
        friend class Core;
        friend struct std::hash<DataType>;
    };

    using DataTypes = QList<DataType>;
} // namespace hdps

namespace std {
    template<>
    struct hash<hdps::DataType>
    {
        std::size_t operator()(const hdps::DataType& dataType) const
        {
            return std::hash<std::string>()(dataType._type.toStdString());
            //return qHash(dataType._type); // For some reason this doesn't work
        }
    };
}

#endif // HDPS_DATATYPE_H
