#include "DataEditorWidget.h"
#include "Core.h"

#include <QDebug>

namespace hdps
{

namespace gui
{

DataEditorWidget::DataEditorWidget(QWidget* parent, Core* core) :
    QWidget(parent),
    _core(core),
    _dataWidget(nullptr)
{
    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setAlignment(Qt::AlignTop);
}

void DataEditorWidget::setDataset(const QString& datasetName)
{
    try
    {
        Q_ASSERT(!datasetName.isEmpty());

        if (datasetName.isEmpty())
            return;

        if (_dataWidget != nullptr)
            delete _dataWidget;

        auto& dataset = _core->requestData(datasetName);

        auto settingsAction = dataset.getActionByName("Settings");

        _dataWidget = settingsAction ? settingsAction->createWidget(this) : new QWidget();

        layout()->addWidget(_dataWidget);
    }
    catch (std::exception& e)
    {
    	
    }
}

}
}
