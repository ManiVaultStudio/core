// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"

namespace mv::gui {

/**
 * Window/level settings action class
 *
 * Action class for window/level settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WindowLevelAction : public GroupAction
{
    Q_OBJECT

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE WindowLevelAction(QObject* parent, const QString& title);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: /** Action getters */

    DecimalAction& getWindowAction() { return _windowAction; }
    DecimalAction& getLevelAction() { return _levelAction; }

signals:

    /** Signals the window/level changed */
    void changed(WindowLevelAction& windowLevelAction);

protected:
    DecimalAction   _windowAction;      /** Window action */
    DecimalAction   _levelAction;       /** Level action */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::WindowLevelAction)

inline const auto windowLevelActionMetaTypeId = qRegisterMetaType<mv::gui::WindowLevelAction*>("mv::gui::WindowLevelAction");
