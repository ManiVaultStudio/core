#include "LinkedSelection.h"
#include "Set.h"

#include "event/Event.h"

#include <QDebug>

namespace hdps
{
    LinkedSelection::LinkedSelection(const Dataset<DatasetImpl>& sourceDataSet, const Dataset<DatasetImpl>& targetDataSet) :
        _sourceDataSet(sourceDataSet),
        _targetDataSet(targetDataSet)
    {
        Q_ASSERT(_sourceDataSet.isValid());
        Q_ASSERT(_targetDataSet.isValid());

        init();
    }

    void LinkedSelection::init()
    {
        _eventListener.setEventCore(Application::core());
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
        _eventListener.registerDataEvent([this](DataEvent* dataEvent) {
            qDebug() << "sdads";
            if (dataEvent->getDataset() == _sourceDataSet)
                _targetDataSet->resolveLinkedSelectionFromSourceDataset(_sourceDataSet);

            if (dataEvent->getDataset() == _targetDataSet)
                _sourceDataSet->resolveLinkedSelectionFromTargetDataset(_targetDataSet);
        });
    }

    const SelectionMap& LinkedSelection::getMapping()
    {
        return _mapping;
    }

    void LinkedSelection::setMapping(SelectionMap& mapping)
    {
        _mapping = mapping;
    }

}
