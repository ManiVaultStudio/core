#pragma once

#include <QStringListModel>

namespace
{
    inline QStringListModel& createStringListModel(const unsigned int numDimensions, const std::vector<QString>& names, QObject& parent)
    {
        QStringList stringList;

        stringList.reserve(static_cast<int>(numDimensions));

        if (numDimensions == names.size())
        {
            for (const auto& name : names)
            {
                stringList.append(name);
            }
        }
        else
        {
            for (unsigned int i = 0; i < numDimensions; ++i)
            {
                stringList.append(QString::fromLatin1("Dim %1").arg(i));
            }
        }

        auto* stringListModel = new QStringListModel(stringList, &parent);
        return *stringListModel;
    }
}