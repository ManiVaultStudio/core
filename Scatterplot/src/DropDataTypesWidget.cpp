#include "DropDataTypesWidget.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QLabel>
#include <QVBoxLayout>

using namespace hdps;
using namespace hdps::gui;

DropDataTypesWidget::DropDataTypesWidget(QWidget* parent, const hdps::DataTypes& dataTypes) :
    QWidget(parent),
    _dropDataTypeWidgets()
{
    for (auto dataType : dataTypes)
        _dropDataTypeWidgets << new DropDataTypeWidget(dataType);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    for (auto dropDataTypeWidget : _dropDataTypeWidgets) {
        layout->addWidget(dropDataTypeWidget);
        dropDataTypeWidget->installEventFilter(this);
    }

    parent->installEventFilter(this);

    setLayout(layout);
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
            for (auto dropDataTypeWidget : _dropDataTypeWidgets)
                dropDataTypeWidget->setActive(true);

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