#include "DropDataTypesWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "PointData.h"
#include "ClusterData.h"
#include "ColorData.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QMimeData>

using namespace hdps;
using namespace hdps::gui;

DropDataTypesWidget::DropDataTypesWidget(ScatterplotPlugin* scatterplotPlugin) :
    QWidget(reinterpret_cast<QWidget*>(scatterplotPlugin)),
    _scatterplotPlugin(scatterplotPlugin),
    _dropDataTypeWidgets()
{
    _dropDataTypeWidgets["point"]   = new DropDataTypeWidget(PointType);
    _dropDataTypeWidgets["color"]   = new DropDataTypeWidget(PointType);
    _dropDataTypeWidgets["cluster"] = new DropDataTypeWidget(PointType);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(_dropDataTypeWidgets["point"]);
    layout->addWidget(_dropDataTypeWidgets["color"]);
    layout->addWidget(_dropDataTypeWidgets["cluster"]);

    for (auto dropDataTypeWidget : _dropDataTypeWidgets.values())
        dropDataTypeWidget->installEventFilter(this);

    setAcceptDrops(true);

    this->installEventFilter(this);
    _scatterplotPlugin->installEventFilter(this);

    setLayout(layout);

    setStyleSheet("{background-color: red;}");
}

bool DropDataTypesWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());
            break;
        }

        case QEvent::DragEnter:
        {
            if (dynamic_cast<QWidget*>(target) != this)
                break;

            const auto dragEnterEvent   = static_cast<QDragEnterEvent*>(event);
            const auto mimeData         = dragEnterEvent->mimeData();
            const auto mimeText         = mimeData->text();
            const auto tokens           = mimeText.split("\n");
            const auto datasetName      = tokens[0];
            const auto dataType         = DataType(tokens[1]);
            
            if (dataType == PointType) {
                const auto currentDatasetName = _scatterplotPlugin->getCurrentDataset();

                if (currentDatasetName.isEmpty()) {
                    _dropDataTypeWidgets["point"]->activate();
                }
                else {
                    const auto currentDataset     = _scatterplotPlugin->getCore()->requestData<Points>(currentDatasetName);
                    const auto candidateDataset   = _scatterplotPlugin->getCore()->requestData<Points>(datasetName);

                    if (currentDataset.getNumPoints() != candidateDataset.getNumPoints()) {
                        _dropDataTypeWidgets["point"]->activate();
                    } else {
                        _dropDataTypeWidgets["point"]->activate();
                        _dropDataTypeWidgets["color"]->activate();
                    }
                }

                //dragEnterEvent
            }

            /*
            for (auto dropDataTypeWidget : _dropDataTypeWidgets)
                dropDataTypeWidget->setActive(true);
            */

            break;
        }

        case QEvent::DragLeave:
        case QEvent::Drop:
        {
            for (auto dropDataTypeWidget : _dropDataTypeWidgets)
                dropDataTypeWidget->setActive(false);

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

DropDataTypesWidget::DropDataTypeWidget::DropDataTypeWidget(const DataType& dataType, QWidget* parent /*= nullptr*/) :
    hdps::gui::DataSlot(DataTypes({ dataType })),
    _labelsWidget(),
    _iconLabel(),
    _dataTypeLabel()
{
    //_dataTypeLabel.setText(QString("Load as %1").arg(dataType()));

    layout()->setMargin(0);
    layout()->setSpacing(0);

    setHighLight(false);

    auto layout = new QVBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    _labelsWidget.setObjectName("Labels");
    _labelsWidget.setLayout(layout);

    _iconLabel.setAlignment(Qt::AlignCenter);
    _dataTypeLabel.setAlignment(Qt::AlignCenter);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _iconLabel.setFont(fontAwesome.getFont(20));
    _iconLabel.setText(fontAwesome.getIconCharacter("plus"));

    layout->addStretch(1);
    layout->addWidget(&_iconLabel);
    layout->addWidget(&_dataTypeLabel);
    layout->addStretch(1);

    addWidget(&_labelsWidget);

    deactivate();
}

void DropDataTypesWidget::DropDataTypeWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    DataSlot::dragEnterEvent(dragEnterEvent);

    setHighLight(true);
}

void DropDataTypesWidget::DropDataTypeWidget::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
    setHighLight(false);
}

void DropDataTypesWidget::DropDataTypeWidget::dropEvent(QDropEvent* dropEvent)
{
    DataSlot::dropEvent(dropEvent);

    setHighLight(false);
}

void DropDataTypesWidget::DropDataTypeWidget::setActive(const bool& active)
{
    _labelsWidget.setVisible(active);
}

void DropDataTypesWidget::DropDataTypeWidget::activate()
{
    setActive(true);
}

void DropDataTypesWidget::DropDataTypeWidget::deactivate()
{
    setActive(false);
}

void DropDataTypesWidget::DropDataTypeWidget::setHighLight(const bool& highlight /*= false*/)
{
    const auto backgroundColorString    = QString("rgba(150, 150, 150, %1)").arg(highlight ? "50" : "10");
    const auto borderString             = QString("%1 solid rgba(0, 0, 0, %2)").arg(highlight ? "1px" : "1px", highlight ? "50" : "10");

    _labelsWidget.setStyleSheet(QString("QWidget#Labels { background-color: %2; border: %3; }").arg(backgroundColorString, borderString));

    const auto textColorString = QString("rgba(0, 0, 0, %1)").arg(highlight ? "200" : "100");

    _iconLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
    _dataTypeLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
}