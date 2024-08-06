// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"

#include <QAbstractItemModel>
#include <QTimer>

namespace mv::gui {

/**
 * Number of rows action class
 *
 * Action class for keeping track of the number of rows in a QAbstractItemModel (derived) class
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT NumberOfRowsAction final : public StringAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE NumberOfRowsAction(QObject* parent, const QString& title);

    /** Ensure to disconnect from model to avoid receiving signals when the actions is already destructed */
    ~NumberOfRowsAction() override;

    /**
     * Initialize the with a pointer to a \p model
     * @param model Pointer to model to synchronize with
     */
    void initialize(QAbstractItemModel* model);

private:

    /** Determine number of rows and update the string value */
    void updateString();

    /** Invoked when the number of rows changed */
    void numberOfRowsChanged();

protected:
    QAbstractItemModel*     _model;         /** Pointer to model to synchronize with */
    QTimer                  _updateTimer;   /** Timer to prevent unnecessary updates */
};

}

Q_DECLARE_METATYPE(mv::gui::NumberOfRowsAction)

inline const auto numberOfRowsActionMetaTypeId = qRegisterMetaType<mv::gui::NumberOfRowsAction*>("NumberOfRowsAction");
