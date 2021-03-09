#include "DropDataTypesWidget.h"
#include "Application.h"
#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

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

DropDataTypesWidget::DropDataTypesWidget(QWidget* parent, ScatterplotPlugin* scatterplotPlugin) :
    QWidget(parent),
    _scatterplotPlugin(scatterplotPlugin),
    _dropRegionWidgets()
{
    setAcceptDrops(true);
    setMouseTracking(true);

    _dropRegionWidgets["point"]     = new DropRegionWidget("Load as point data", this);
    _dropRegionWidgets["color"]     = new DropRegionWidget("Load as color data", this);
    _dropRegionWidgets["cluster"]   = new DropRegionWidget("Load as cluster data", this);
    _dropRegionWidgets["error"]     = new DropRegionWidget("Data cannot be loaded", this);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    layout->addWidget(_dropRegionWidgets["point"]);
    layout->addWidget(_dropRegionWidgets["color"]);
    layout->addWidget(_dropRegionWidgets["cluster"]);
    layout->addWidget(_dropRegionWidgets["error"]);

    for (auto dropRegionWidget : _dropRegionWidgets.values())
        dropRegionWidget->installEventFilter(this);

    // Install event filters for synchronizing widget size
    parent->installEventFilter(this);
    this->installEventFilter(this);
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
            if (dynamic_cast<QWidget*>(target) == this)
                event->accept();

            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);
            /*
            if (!_dropRegionWidgets.values().contains(dropRegionWidget))
                break;

            
            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);

            if (!_dropRegionWidgets.values().contains(dropRegionWidget))
                break;
            */

            for (auto dropRegionWidget : _dropRegionWidgets.values())
                dropRegionWidget->hide();

            const auto dragEnterEvent   = static_cast<QDragEnterEvent*>(event);
            const auto mimeData         = dragEnterEvent->mimeData();
            const auto mimeText         = mimeData->text();
            const auto tokens           = mimeText.split("\n");
            const auto datasetName      = tokens[0];
            const auto dataType         = DataType(tokens[1]);
            
            if (dataType == PointType) {
                const auto currentDatasetName = _scatterplotPlugin->getCurrentDataset();

                if (currentDatasetName.isEmpty()) {
                    _dropRegionWidgets["point"]->show();
                }
                else {
                    const auto currentDataset     = _scatterplotPlugin->getCore()->requestData<Points>(currentDatasetName);
                    const auto candidateDataset   = _scatterplotPlugin->getCore()->requestData<Points>(datasetName);

                    if (currentDataset.getNumPoints() != candidateDataset.getNumPoints()) {
                        _dropRegionWidgets["point"]->show();
                    } else {
                        _dropRegionWidgets["point"]->show();
                        _dropRegionWidgets["color"]->show();
                    }
                }
            }

            if (dropRegionWidget)
                dragEnterEvent->acceptProposedAction();

            break;
        }

        case QEvent::DragLeave:
        {
            /*
            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);

            if (!_dropRegionWidgets.values().contains(dropRegionWidget))
                break;

            for (auto dropRegionWidget : _dropRegionWidgets.values())
                dropRegionWidget->deactivate();

            const auto dragLeaveEvent = static_cast<QDragLeaveEvent*>(event);

            dropRegionWidget->setHighLight(false);
            */

            for (auto dropRegionWidget : _dropRegionWidgets.values())
                dropRegionWidget->hide();

            break;
        }

        case QEvent::Drop:
        {
            auto dropRegionWidget = dynamic_cast<DropRegionWidget*>(target);

            if (!_dropRegionWidgets.values().contains(dropRegionWidget))
                break;

            const auto dropEvent        = static_cast<QDragEnterEvent*>(event);
            const auto mimeData         = dropEvent->mimeData();
            const auto mimeText         = mimeData->text();
            const auto tokens           = mimeText.split("\n");
            const auto datasetName      = tokens[0];

            if (!dropRegionWidget)
                break;

            if (_dropRegionWidgets.key(dropRegionWidget) == "point")
                _scatterplotPlugin->loadPointData(datasetName);

            if (_dropRegionWidgets.key(dropRegionWidget) == "color")
                _scatterplotPlugin->loadColorData(datasetName);
            
            for (auto dropRegionWidget : _dropRegionWidgets.values())
                dropRegionWidget->hide();

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

DropDataTypesWidget::DropRegionWidget::DropRegionWidget(const QString& title, QWidget* parent) :
    QWidget(parent),
    _labelsWidget(),
    _iconLabel(),
    _dataTypeLabel()
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    _dataTypeLabel.setText(title);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

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

    mainLayout->addWidget(&_labelsWidget);

    hide();

    parent->installEventFilter(this);
}

bool DropDataTypesWidget::DropRegionWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::MouseMove) {
        const auto mouseEvent = static_cast<QMouseEvent*>(event);

        setHighLight(rect().contains(mouseEvent->pos()));
    }

    return QWidget::eventFilter(target, event);
}

void DropDataTypesWidget::DropRegionWidget::setActive(const bool& active)
{
    _labelsWidget.setVisible(active);
}

void DropDataTypesWidget::DropRegionWidget::activate()
{
    setActive(true);
}

void DropDataTypesWidget::DropRegionWidget::deactivate()
{
    setActive(false);
}

void DropDataTypesWidget::DropRegionWidget::setHighLight(const bool& highlight /*= false*/)
{
    const auto backgroundColorString    = QString("rgba(150, 150, 150, %1)").arg(highlight ? "50" : "10");
    const auto borderString             = QString("%1 solid rgba(0, 0, 0, %2)").arg(highlight ? "1px" : "1px", highlight ? "50" : "10");

    _labelsWidget.setStyleSheet(QString("QWidget#Labels { background-color: %2; border: %3; }").arg(backgroundColorString, borderString));

    const auto textColorString = QString("rgba(0, 0, 0, %1)").arg(highlight ? "200" : "100");

    _iconLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
    _dataTypeLabel.setStyleSheet(QString("color: %1;").arg(textColorString));
}