#include "DataSlot.h"

#include <QMimeData>
#include <QDebug>

namespace hdps
{
    namespace gui
    {
        DataSlot::DataSlot(QStringList supportedKinds)
        {
            _supportedKinds = supportedKinds;

            setAcceptDrops(true);
        }
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

            if (tokens.size() < 2)
                return;

            QString kind = tokens[1];
            if (_supportedKinds.contains(kind))
            {
                emit onDataInput(tokens[0]);

                event->accept();
            }
            else
            {
                event->ignore();
            }
        }
        }
    }
}
