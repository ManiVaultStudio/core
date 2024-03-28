// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "StringAction.h"
#include "OptionAction.h"
#include "ToggleAction.h"

class QSortFilterProxyModel;

namespace mv::gui {

/**
 * Model filter action class
 *
 * Action class for configuring filtering parameters
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ModelFilterAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ModelFilterAction(QObject* parent, const QString& title);

    /**
     * Initializes the table action with a pointer to a \p model, possibly a \p filterModel and \p itemTypeName
     * @param model Pointer to model
     * @param filterModel Pointer to filter model (maybe nullptr)
     * @param itemTypeName Item type name string
     */
    void initialize(QAbstractItemModel* model, QSortFilterProxyModel* filterModel, const QString& itemTypeName);

private:

    /** Update the filter model and related actions */
    void updateFilterModel();

private:
    QAbstractItemModel*         _model;                             /** Pointer to input model */
    QSortFilterProxyModel*      _filterModel;                       /** Pointer to filter model */
    QString                     _itemTypeName;                      /** String that describes an individual model item type */
    StringAction                _filterNameAction;                  /** String action for filtering by name */
    OptionAction                _filterColumnAction;                /** Option action for choosing the filtering column */
    GroupAction                 _filterGroupAction;                 /** Filter group action */
    ToggleAction                _filterCaseSensitiveAction;         /** Filter case-sensitive action */
    ToggleAction                _filterRegularExpressionAction;     /** Enable filter with regular expression action */
};

}

Q_DECLARE_METATYPE(mv::gui::ModelFilterAction)

inline const auto modelFilterActionMetaTypeId = qRegisterMetaType<mv::gui::ModelFilterAction*>("mv::gui::ModelFilterAction");
