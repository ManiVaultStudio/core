#ifndef HDPS_DATA_EDITOR_WIDGET_H
#define HDPS_DATA_EDITOR_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTreeWidget>

class QTreeWidgetItem;

namespace hdps
{

class Core;

namespace gui
{

class WidgetActionGroup;

class SectionExpandButton : public QPushButton {
public:
    SectionExpandButton(QTreeWidgetItem* treeWidgetItem, WidgetActionGroup* widgetActionGroup, const QString& text, QWidget* parent = nullptr);

protected:
    WidgetActionGroup*  _widgetActionGroup;
    QTreeWidgetItem*    _treeWidgetItem;
};
    

/**
 * Data editor widget class
 * Widget class for viewing/editing data
 */
class DataPropertiesWidget : public QWidget
{
public:
    

public:
    DataPropertiesWidget(QWidget* parent, Core* core);
            
    void setDataset(const QString& datasetName);

protected:
    QTreeWidgetItem* addButton(WidgetActionGroup* widgetActionGroup);

protected:
    Core*           _core;          /** Pointer to core */
    QTreeWidget*    _treeWidget;    /** Sections tree widget */
    QWidget*        _dataWidget;    /** Current data widget */
};

}
}

#endif // HDPS_DATA_EDITOR_WIDGET_H
