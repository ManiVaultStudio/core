// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QSortFilterProxyModel>

#include "ManiVaultGlobals.h"

#include "util/Serializable.h"

#include "NumberOfRowsAction.h"

#include "actions/StringAction.h"
#include "actions/VerticalGroupAction.h"
#include "actions/OptionAction.h"
#include "actions/ToggleAction.h"

namespace mv
{

class DatasetImpl;

/**
 * Filter model class
 *
 * Base MV filter model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT SortFilterProxyModel : public QSortFilterProxyModel, public mv::util::Serializable
{
    Q_OBJECT

public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    SortFilterProxyModel(QObject* parent = nullptr);

    /**
     * Set source model to \p sourceModel
     * @param sourceModel Pointer to source model
     */
    void setSourceModel(QAbstractItemModel* sourceModel) override;

    /**
     * Set row type name to \p rowTypeName
     * @param rowTypeName Type name by which to identify a row
     */
    void setRowTypeName(const QString& rowTypeName);

    /**
     * Get row type name
     * @return Type name by which to identify a row
     */
    QString getRowTypeName() const;

    /**
     * Set filter column to \p column
     * @param column Column index
     */
    void setFilterColumn(int column);

private:

    /** Force users to use SortFilterProxyModel::setFilterColumn(...), because we need to be notified of filter column changes */
    using QSortFilterProxyModel::setFilterKeyColumn;

private:

    /** Updates the text filter settings and the configures the model */
    void updateTextFilterSettings();

    /** Re-populates the filter column action */
    void updateFilterColumnAction();

signals:

    /**
     * Signals that the row type name changed to \p rowTypeName
     * @param rowTypeName Type name by which to identify a row
     */
    void rowTypeNameChanged(const QString& rowTypeName);

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::NumberOfRowsAction& getNumberOfRowsAction() { return _numberOfRowsAction; }
    mv::gui::StringAction& getTextFilterAction() { return _textFilterAction; }
    mv::gui::VerticalGroupAction& getTextFilterSettingsAction() { return _textFilterSettingsAction; }
    mv::gui::OptionAction& getTextFilterColumnAction() { return _textFilterColumnAction; }
    mv::gui::ToggleAction& getTextFilterCaseSensitiveAction() { return _textFilterCaseSensitiveAction; }
    mv::gui::ToggleAction& getTextFilterRegularExpressionAction() { return _textFilterRegularExpressionAction; }

private:
    QString                     _rowTypeName;                           /** Type name by which to identify a row */
    gui::NumberOfRowsAction     _numberOfRowsAction;                    /** String action for displaying the number of rows */
    gui::StringAction           _textFilterAction;                      /** Filter based on text */
    gui::VerticalGroupAction    _textFilterSettingsAction;              /** Text filter settings */
    gui::OptionAction           _textFilterColumnAction;                /** Option action for choosing the filtering column */
    gui::ToggleAction           _textFilterCaseSensitiveAction;         /** Whether to filter case-sensitive or not */
    gui::ToggleAction           _textFilterRegularExpressionAction;     /** Enable filter with regular expression action */
};

}
