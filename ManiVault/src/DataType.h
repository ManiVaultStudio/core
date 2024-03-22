// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QList>

namespace mv
{
    namespace Plugin {
        class RawData;
    }

    class CORE_EXPORT DataType
    {
    public:
        DataType() = delete;

        DataType(QString type) :
            _type(type)
        { }

        inline friend bool operator==(const DataType& lhs, const DataType& rhs) { return lhs._type == rhs._type; }
        inline friend bool operator!=(const DataType& lhs, const DataType& rhs) { return !(lhs == rhs); }
        inline friend QString operator+(const QString& lhs, const DataType& rhs) { return lhs + rhs._type; }

        /**
         * Get the type of raw data as string
         * @return String representation of the data type
         */
        QString getTypeString() const {
            return _type;
        }

    private:
        QString _type;

        friend class RawData;
        friend class DataManager;
        friend struct std::hash<DataType>;
    };

    using DataTypes = QVector<DataType>;
} // namespace mv

namespace std {
    template<>
    struct hash<mv::DataType>
    {
        std::size_t operator()(const mv::DataType& dataType) const
        {
            return std::hash<std::string>()(dataType._type.toStdString());
            //return qHash(dataType._type); // For some reason this doesn't work
        }
    };
}
