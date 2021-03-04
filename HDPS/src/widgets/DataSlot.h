#pragma once

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QString>
#include <QStringList>

#include <QHBoxLayout>

#include "../DataType.h"

namespace hdps
{
    namespace gui
    {
        class DataSlot : public QWidget
        {
            Q_OBJECT
        public:
            DataSlot(hdps::DataTypes supportedTypes);

            void addWidget(QWidget* widget);

            void dragEnterEvent(QDragEnterEvent* event) override;
            void dropEvent(QDropEvent* event) override;

        signals:
            void onDataInput(const QString& datasetName, const DataType& dataType);

        private:
            QHBoxLayout* _layout;

            hdps::DataTypes _supportedTypes;
        };
    }
}
