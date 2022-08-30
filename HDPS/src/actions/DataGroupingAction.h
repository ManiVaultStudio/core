#pragma once

#include "actions/Actions.h"

using namespace hdps::util;

namespace hdps {

/**
 * Data grouping action class
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

    /** Establishes whether group indices may be removed recursively */
    bool mayRemoveRecursively() const;

protected:
    Dataset<DatasetImpl>    _dataset;                   /** Smart pointer to the dataset */
    TriggerAction           _assignAction;              /** Groups children recursively and sequentially */
    TriggerAction           _removeAction;              /** Remove action*/
    TriggerAction           _removeRecursivelyAction;   /** Remove recursively action */
};

}
