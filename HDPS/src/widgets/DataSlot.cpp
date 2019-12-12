#include "DataSlot.h"

#include <QMimeData>
#include <QDebug>

namespace hdps
{
    namespace gui
    {
        DataSlot::DataSlot(hdps::DataTypes supportedTypes)
        {
            _supportedTypes = supportedTypes;

            setAcceptDrops(true);

            _layout = new QHBoxLayout(this);
        }

        void DataSlot::addWidget(QWidget* widget)
        {
            _layout->addWidget(widget);
        }

        void DataSlot::dragEnterEvent(QDragEnterEvent* event)
        {
            const QMimeData* mimeData = event->mimeData();

            event->acceptProposedAction();
        }

        void DataSlot::dropEvent(QDropEvent* event)
        {
            const QMimeData* mimeData = event->mimeData();

            QString mimeText = mimeData->text();
            QStringList tokens = mimeText.split("\n");

            if (tokens.size() < 2)
                return;

            DataType dataType = DataType(tokens[1]);

            if (_supportedTypes.contains(dataType))
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
