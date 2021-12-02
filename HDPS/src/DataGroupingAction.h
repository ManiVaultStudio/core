#pragma once

#include "actions/Actions.h"

using namespace hdps::util;

namespace hdps {

/**
 * Data export action class
 *
 * Data type dependent data export options
 *
 * @author Thomas Kroes
 */
class DataGroupingAction : public hdps::gui::WidgetAction
{
protected:

    class GroupingDialog : public QDialog
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dataset Dataset root to apply grouping to
         */
        GroupingDialog(QWidget* parent, const Dataset<DatasetImpl>& dataset);

        /** Get preferred size */
        QSize sizeHint() const override {
            return QSize(350, 150);
        }

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    protected:
        Dataset<DatasetImpl>    _dataset;               /** Smart pointer to the dataset */
        IntegralAction          _groupIndexAction;      /** Starting group index action */
        ToggleAction            _recursiveAction;       /** Apply group indices recursively action */
        TriggerAction           _applyAction;           /** Apply grouping action */
        TriggerAction           _cancelAction;          /** Cancel action */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to the dataset
     */
    DataGroupingAction(QObject* parent, const Dataset<DatasetImpl>& dataset);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    Dataset<DatasetImpl>    _dataset;               /** Smart pointer to the dataset */
    TriggerAction           _sequential;       /** Groups children recursively and sequentially */
};

}
