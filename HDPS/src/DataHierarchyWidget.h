#ifndef HDPS_DATA_HIERARCHY_WIDGET_H
#define HDPS_DATA_HIERARCHY_WIDGET_H

#include "DataHierarchyModel.h"
#include "DataImportAction.h"

#include <QWidget>
#include <QTreeView>
#include <QGraphicsOpacityEffect>

namespace hdps
{

namespace gui
{

/**
* Widget for displaying the data hierarchy
*/
class DataHierarchyWidget : public QTreeView
{
    Q_OBJECT

public:

    /**
     * No data overlay widget class
     *
     * @author Thomas Kroes
     */
    class NoDataOverlayWidget : public QWidget
    {
    public:
        /** Default constructor */
        NoDataOverlayWidget(QWidget* parent);

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    protected:
        QGraphicsOpacityEffect*     _opacityEffect;     /** Effect for modulating opacity */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    DataHierarchyWidget(QWidget* parent);

signals:

    /**
     * Invoked when the selected dataset changed
     * @param datasetId Globally unique identifier of the selected dataset
     */
    void selectedDatasetChanged(const QString& datasetId);

private:
    DataHierarchyModel      _model;                 /** Model containing data to be displayed in the hierarchy */
    QItemSelectionModel     _selectionModel;        /** Selection model */
    NoDataOverlayWidget*    _noDataOverlayWidget;   /** Overlay help widget which is shown when no data is loaded */
    DataImportAction        _dataImportAction;      /** Data import action */
};

}
}

#endif // HDPS_DATA_HIERARCHY_WIDGET_H
