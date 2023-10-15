// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "models/ActionsFilterModel.h"
#include "models/ActionsListModel.h"

#include <QMimeData>

namespace mv::gui {

/**
 * Widget action mime data class
 * 
 * Class for drag-and-drop of a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionMimeData final : public QMimeData
{
public:

    /**
     * Construct with \p action
     * @param action Pointer to action for dropping
     */
    WidgetActionMimeData(WidgetAction* action);

    /** Destructor */
    ~WidgetActionMimeData();

    /**
     * Get supported mime types
     * @return List of string of supported mime types
     */
    QStringList formats() const override;

    /**
     * Get stored action
     * @return Pointer to stored widget action
     */
    WidgetAction* getAction() const { return _action; }

    /** Supported mime type */
    static QString format() {
        return "application/action";
    }

private:
    WidgetAction*       _action;                /** Pointer to mime data action */
    ActionsListModel    _actionsListModel;      /** Actions list model */
    ActionsFilterModel  _actionsFilterModel;    /** Filtered actions model */
    WidgetActions       _highlightActions;      /** Actions to be highlighted/un-highlighted */
};

}