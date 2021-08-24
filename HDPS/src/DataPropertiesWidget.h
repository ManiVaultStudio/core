#ifndef HDPS_DATA_PROPERTIES_WIDGET_H
#define HDPS_DATA_PROPERTIES_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTreeWidget>

class QTreeWidgetItem;

namespace hdps
{

class Core;
class DataHierarchyItem;

namespace gui
{

class GroupAction;

/**
 * Section expand button class
 *
 * Represents a collapse/expand button
 *
 * @author Thomas Kroes
 */
class SectionExpandButton : public QPushButton {
public:

    /**
     * Constructor
     * @param treeWidgetItem Pointer to tree widget item
     * @param widgetActionGroup Pointer to widget action group
     * @param text Section name
     * @param parent Pointer to parent widget
     */
    SectionExpandButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, const QString& text, QWidget* parent = nullptr);

protected:
    GroupAction*        _widgetActionGroup;     /** Pointer to widget action group */
    QTreeWidgetItem*    _treeWidgetItem;        /** Pointer to widget action group */
};

/**
 * Data properties widget class
 *
 * Widget class for viewing/editing data
 *
 * @author Thomas Kroes
 */
class DataPropertiesWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param core Pointer to the core
     */
    DataPropertiesWidget(QWidget* parent, Core* core);

    /**
     * Set the name of the current dataset
     * @param datasetName Name of the dataset
     */
    void setDataset(const QString& datasetName);

protected:

    /**
     * Add a section collapse/expand button
     * @param groupAction Pointer to group action
     */
    QTreeWidgetItem* addButton(GroupAction* groupAction);

signals:

    /**
     * Signals that the dataset name changed (used to change the docking widget title)
     * @param datasetName Name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

protected:
    Core*                   _core;                  /** Pointer to the core */
    QTreeWidget*            _treeWidget;            /** Pointer to sections tree widget */
    QWidget*                _dataWidget;            /** Pointer to current data widget */
    DataHierarchyItem*      _dataHierarchyItem;     /** Pointer to current data hierarchy item */
};

}
}

#endif // HDPS_DATA_PROPERTIES_WIDGET_H
