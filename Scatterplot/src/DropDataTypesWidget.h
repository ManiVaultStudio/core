#pragma once

#include "DataType.h"

#include "widgets/DataSlot.h"

#include <QWidget>
#include <QLabel>

class ScatterplotPlugin;

class DropDataTypesWidget : public QWidget
{
public:
    class DropDataTypeWidget : public hdps::gui::DataSlot {
    public:
        DropDataTypeWidget(const hdps::DataType& dataType, QWidget* parent = nullptr);

        void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;
        void dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent) override;
        void dropEvent(QDropEvent* dropEvent) override;

        void setActive(const bool& active);
        void activate();
        void deactivate();

    protected:
        void setHighLight(const bool& highlight = false);

    protected:
        QWidget     _labelsWidget;
        QLabel      _iconLabel;
        QLabel      _dataTypeLabel;
    };

public:
    DropDataTypesWidget(ScatterplotPlugin* scatterplotPlugin);

    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    ScatterplotPlugin*                  _scatterplotPlugin;
    QMap<QString, DropDataTypeWidget*>  _dropDataTypeWidgets;
};