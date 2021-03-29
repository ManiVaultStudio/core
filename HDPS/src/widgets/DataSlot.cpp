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
            const auto mimeData = event->mimeData();

            event->acceptProposedAction();
        }

        void DataSlot::dropEvent(QDropEvent* event)
        {
            const auto mimeData = event->mimeData();
            const auto mimeText = mimeData->text();
            const auto tokens   = mimeText.split("\n");

            if (tokens.size() < 2)
                return;

            const auto dataType = DataType(tokens[1]);

            if (_supportedTypes.contains(dataType))
            {
                emit onDataInput(tokens[0], dataType);

                event->accept();
            }
            else
                event->ignore();
        }
    }
}
