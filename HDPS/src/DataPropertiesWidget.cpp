#include "DataPropertiesWidget.h"
#include "Application.h"
#include "Core.h"

#include "actions/GroupAction.h"

#include <QDebug>
#include <QVBoxLayout>

namespace hdps
{

namespace gui
{

DataPropertiesWidget::DataPropertiesWidget(QWidget* parent) :
    QWidget(parent),
    _dataset(),
    _groupsAction(this)
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(_groupsAction.createWidget(this));

    connect(&_dataset, &DatasetRef<DataSet>::datasetNameChanged, this, [this](const QString& oldDatasetName, const QString& newDatasetName) {
        loadDataset();
    });

    connect(&_dataset, &DatasetRef<DataSet>::datasetNameChanged, this, [this](const QString& oldDatasetName, const QString& newDatasetName) {
        loadDataset();
    });

    emit datasetNameChanged("");
}

void DataPropertiesWidget::setDataset(const QString& datasetName)
{
    try
    {
        if (datasetName.isEmpty())
            throw std::runtime_error("data set name is empty");

        if (_dataset.isValid())
            disconnect(&_dataset->getHierarchyItem(), &DataHierarchyItem::actionAdded, this, nullptr);

        _dataset.setDatasetName(datasetName);

        if (!_dataset.isValid())
            return;

        connect(&_dataset->getHierarchyItem(), &DataHierarchyItem::actionAdded, this, [this](WidgetAction& widgetAction) {
            if (dynamic_cast<GroupAction*>(&widgetAction) == nullptr)
                return;

            loadDataset();
        });
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Cannot update data properties for %1: %2").arg(datasetName, e.what());
    }
}

void DataPropertiesWidget::loadDataset()
{
    emit datasetNameChanged(_dataset.getDatasetName());

    if (!_dataset.isValid()) {
        _groupsAction.set(QVector<GroupAction*>());
        return;
    }

    GroupsAction::GroupActions groupActions;

    for (auto action : _dataset->getActions()) {
        auto groupAction = dynamic_cast<GroupAction*>(action);

        if (groupAction == nullptr)
            continue;

        groupActions << groupAction;
    }

    _groupsAction.set(groupActions);
}

}
}
