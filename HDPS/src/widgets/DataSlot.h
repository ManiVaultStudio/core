#pragma once

#include <QFrame>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QString>
#include <QStringList>

namespace hdps
{
    namespace gui
    {
        class DataSlot : public QWidget
        {
            Q_OBJECT
        public:
            DataSlot();

            void dragEnterEvent(QDragEnterEvent* event) override;
            void dropEvent(QDropEvent* event) override;

        signals:
            void onDataInput(QString dataName);

        private:

            QStringList _supportedKinds;
        };
    }
}
