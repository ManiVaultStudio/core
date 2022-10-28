#pragma once

#include <QDialog>

/**
 * Action hierarchy dialog
 * 
 * Dialog class for configuring action hierarchy
 *
 * @author Thomas Kroes
 */
class ActionHierarchyDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ActionHierarchyDialog(QWidget* parent);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(350, 150);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }
};
