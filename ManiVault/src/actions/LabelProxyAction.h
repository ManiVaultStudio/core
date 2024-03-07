// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace mv::gui {

/**
 * Label proxy action class
 *
 * Sole purpose is to override the label text of the source action
 *
 * Note: This action is a WIP
 * 
 * @author Thomas Kroes
 */
class LabelProxyAction : public WidgetAction
{
    Q_OBJECT

protected:

    /**
     * Get widget representation of the label proxy action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return _sourceAction->createWidget(parent, _sourceAction->getDefaultWidgetFlags());
    }

public:

    /**
     * Construct with pointer to \p parent object, \p title and \p source action
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param sourceAction Pointer to source action
     */
    Q_INVOKABLE LabelProxyAction(QObject* parent, const QString& title, WidgetAction* sourceAction);

protected:
    WidgetAction*   _sourceAction;     /** Source action for which to override the label text */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::LabelProxyAction)

inline const auto labelProxyActionMetaTypeId = qRegisterMetaType<mv::gui::LabelProxyAction*>("mv::gui::LabelProxyAction");
