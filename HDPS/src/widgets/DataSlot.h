#pragma once

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QString>
#include <QStringList>

#include <QHBoxLayout>

namespace hdps
{
    namespace gui
    {
        class DataSlot : public QWidget
        {
            Q_OBJECT
        public:
            DataSlot(QStringList supportedKinds);
            
            void addWidget(QWidget* widget);

            void dragEnterEvent(QDragEnterEvent* event) override;
            void dropEvent(QDropEvent* event) override;

        signals:
            void onDataInput(QString dataName);

        private:
            QHBoxLayout* _layout;

            QStringList _supportedKinds;
        };
    }
}
