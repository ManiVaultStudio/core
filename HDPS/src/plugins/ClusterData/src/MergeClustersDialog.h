#pragma once

#include <QDialog>

#include "actions/StringAction.h"
#include "actions/ColorAction.h"

class MergeClustersDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    MergeClustersDialog(QWidget* parent = nullptr);

    /** Destructor */
    ~MergeClustersDialog() override = default;

    QSize sizeHint() const override {
        return QSize(400, 0);
    }
};
