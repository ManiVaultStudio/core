#pragma once

#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"

namespace hdps::gui {

/**
 * Window/level settings action class
 *
 * Action class for window/level settings
 *
 * @author Thomas Kroes
 */
class WindowLevelAction : public GroupAction
{
    Q_OBJECT

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE WindowLevelAction(QObject* parent, const QString& title);

public: // Linking

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
};

}

Q_DECLARE_METATYPE(hdps::gui::WindowLevelAction)

inline const auto windowLevelActionMetaTypeId = qRegisterMetaType<hdps::gui::WindowLevelAction*>("hdps::gui::WindowLevelAction");
