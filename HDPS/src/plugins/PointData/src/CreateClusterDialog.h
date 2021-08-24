#pragma once

#include "actions/Actions.h"

#include <QDialog>

using namespace hdps;
using namespace hdps::gui;

namespace hdps {
    class CoreInterface;
}

/**
 * Create cluster dialog class
 *
 * Dialog class for configuring a new cluster
 *
  * @author Thomas Kroes
 */
class CreateClusterDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param core Pointer to the core
     * @param inputDatasetName Name of the input dataset
     */
    CreateClusterDialog(CoreInterface* core, const QString& inputDatasetName);

    /** Destructor */
    ~CreateClusterDialog() override = default;

    QSize sizeHint() const override {
        return QSize(400, 0);
    }

    OptionAction& getTargetAction() { return _targetAction; }
    StringAction& getNameAction() { return _nameAction; }
    ColorAction& getColorAction() { return _colorAction; }

protected:
    OptionAction    _targetAction;      /** Target cluster set action */
    StringAction    _nameAction;        /** Cluster name action */
    ColorAction     _colorAction;       /** Cluster color action */
};
