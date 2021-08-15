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

class WidgetActionGroup;

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
    SectionExpandButton(QTreeWidgetItem* treeWidgetItem, WidgetActionGroup* widgetActionGroup, const QString& text, QWidget* parent = nullptr);

protected:
    WidgetActionGroup*  _widgetActionGroup;     /** Pointer to widget action group */
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
     * @param core Pointer to the core
     * @param parent Pointer to parent widget
     */
    DataPropertiesWidget(QWidget* parent, Core* core);

    /**
     * Set the current dataset
     * @param datasetName Name of the dataset
     */
    void setDataset(const QString& datasetName);

protected:

    /**
     * Add a section collapse/expand button
     * @param widgetActionGroup Pointer to widget action group
     */
    QTreeWidgetItem* addButton(WidgetActionGroup* widgetActionGroup);

signals:

    /**
     * Signals that the dataset names changed
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

protected:
    Core*               _core;                  /** Pointer to core */
    QTreeWidget*        _treeWidget;            /** Sections tree widget */
    QWidget*            _dataWidget;            /** Current data widget */
    DataHierarchyItem*  _dataHierarchyItem;     /** Current data hierarchy item */
};

}
}

#endif // HDPS_DATA_PROPERTIES_WIDGET_H
