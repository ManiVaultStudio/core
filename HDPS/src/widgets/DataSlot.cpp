#include "DataSlot.h"

#include <QMimeData>
#include <QDebug>

namespace hdps
{
    namespace gui
    {
        DataSlot::DataSlot()
        {
            setAcceptDrops(true);
        }

        void DataSlot::dragEnterEvent(QDragEnterEvent* event)
        {
            const QMimeData* mimeData = event->mimeData();

            event->accept();
        }

        void DataSlot::dropEvent(QDropEvent* event)
        {
            const QMimeData* mimeData = event->mimeData();

            QString mimeText = mimeData->text();
            QStringList tokens = mimeText.split("\n");

            if (tokens[1] == "SET")
            {
                emit onDataInput(tokens[0]);

                event->accept();
            }
        }
    }
}
