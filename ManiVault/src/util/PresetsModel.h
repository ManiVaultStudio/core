// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Preset.h"

#include <QAbstractListModel>

namespace mv {

namespace gui {
    class WidgetAction;
}

namespace util {

    
/**
 * Presets model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PresetsModel : public QAbstractListModel
{
public:

    /** Presets model columns */
    enum class Column {
        Name,           /** Name column */
        Description     /** Description column */
    };

public:

    /** Constructor
     * @param parent Pointer to parent object
     */
    PresetsModel(QObject* parent = nullptr);

public: // Inherited MVC

    /**
     * Returns the the number of model columns
     * @param parent Parent index
     */
    int columnCount(const QModelIndex& parent) const override;

    /**
     * Returns the number of color maps in the model
     * @param parent Parent index
     */
    int rowCount(const QModelIndex& parent /* = QModelIndex() */) const override;

    /**
     * Returns model data for the given index
     * @param index Index
     * @param role The data role
     */
    QVariant data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const override;

public: // Miscellaneous

    /**
     * Set widget action
     * @param widgetAction Pointer to widget action to retrieve the presets for
     */
    void setWidgetAction(mv::gui::WidgetAction* widgetAction);

private:
    QVector<Preset>     _presets;     /** Preset names */
};

}
}
