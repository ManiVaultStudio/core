#pragma once

#include "WidgetActionWidget.h"

#include <QWidgetAction>
#include <QJsonDocument>

class QLabel;
class QMenu;

namespace hdps {

class DataHierarchyItem;
class Application;

namespace gui {

class WidgetActionLabel;

/**
 * Widget action class
 *
 * Base class for custom widget actions
 * 
 * @author Thomas Kroes
 */
class WidgetAction : public QWidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    WidgetAction(QObject* parent = nullptr);

    /** Destructor */
    ~WidgetAction();

    /**
     * Get type string
     * @return Widget action type in string format
     */
    virtual QString getTypeString() const;

    /**
     * Get parent widget action
     * @return Pointer to parent widget action (if any)
     */
    WidgetAction* getParentWidgetAction();

    /**
     * Create standard widget
     * @param parent Parent widget
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent) override;

    /**
     * Create collapsed widget
     * @param parent Parent widget
     * @return Pointer to collapsed widget
     */
    QWidget* createCollapsedWidget(QWidget* parent);

    /**
     * Create label widget
     * @param parent Parent widget
     * @param widgetFlags Label widget configuration flags
     * @return Pointer to widget
     */
    QWidget* createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags = 0);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    virtual QMenu* getContextMenu(QWidget* parent = nullptr);

    /**
     * Create widget for the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags);

    /** Get the sort index */
    std::int32_t getSortIndex() const;

    /**
     * Set the sort index
     * @param sortIndex Sorting index
     */
    void setSortIndex(const std::int32_t& sortIndex);

    /** Gets the default widget flags */
    std::int32_t getDefaultWidgetFlags() const;

    /**
     * Set the widget flags
     * @param widgetFlags Widget flags
     */
    void setDefaultWidgetFlags(const std::int32_t& widgetFlags);

public: // Linking

    /**
     * Get whether the action may be published or not
     * @return Boolean indicating whether the action may be published or not
     */
    virtual bool mayPublish() const;

    /**
     * Get whether the action is public (visible to other actions)
     * @return Boolean indicating whether the action is public (visible to other actions)
     */
    virtual bool isPublic() const;

    /**
     * Get whether the action is published
     * @return Boolean indicating whether the action is published
     */
    virtual bool isPublished() const;

    /**
     * Get whether the action is connect to a public action
     * @return Boolean indicating whether the action is connect to a public action
     */
    virtual bool isConnected() const;

    /**
     * Publish this action so that other actions can connect to it
     * @param text Name of the published widget action
     */
    virtual void publish(const QString& name);

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    virtual void connectToPublicAction(WidgetAction* publicAction);

    /** Disconnect this action from a public action */
    virtual void disconnectFromPublicAction();

    /**
     * Connect a private action to this action
     * @param action Pointer to private action to connect to this action
     */
    void connectPrivateAction(WidgetAction* privateAction);

    /**
     * Disconnect a private action from this action
     * @param action Pointer to private action to disconnect from this action
     */
    void disconnectPrivateAction(WidgetAction* privateAction);

    /**
     * Get the public action (if any)
     * @return Pointer to the public action
     */
    WidgetAction* getPublicAction();

    /**
     * Get connected actions
     * @return Vector of pointers to connected actions
     */
    const QVector<WidgetAction*> getConnectedActions() const;

protected: // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const;

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its default
     */
    virtual bool isResettable()
    {
        return false;
    };

    /**
     * Reset to factory default
     * @param recursive Reset to factory default recursively
     */
    virtual void reset() {};

    /**
     * Get settings path
     * @return Path of the action w.r.t. to the top-level action
     */
    QString getSettingsPath() const;

    /**
     * Find child widget action of which the GUI name contains the search string
     * @param searchString The search string
     * @param recursive Whether to search recursively
     * @return Found vector of pointers to widget action(s)
     */
    QVector<WidgetAction*> findChildren(const QString& searchString, bool recursive = true) const;

public: // Serialization

    /**
     * Get serialization name in the action tree (returns text() by default and objectName() if it is not empty)
     * @return Serialization name
     */
    QString getSerializationName() const;

    /**
     * Get whether serialization is taking place
     * @return Boolean indicating whether serialization is taking place
     */
    bool isSerializing() const;

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    virtual QVariantMap toVariantMap() const;

    /**
     * Load widget action from variant map
     * @param widgetAction Pointer to target widget action
     * @param Variant map representation of the widget action and its children
     */
    static void fromVariantMap(WidgetAction* widgetAction, const QVariantMap& variantMap);

    /**
     * Save widget action to variant map
     * @param widgetAction Pointer to target widget action
     * @return Variant map representation of the widget action and its children
     */
    static QVariantMap toVariantMap(const WidgetAction* widgetAction);

    /**
     * Load widget action from JSON document
     * @param JSON document
     */
    virtual void fromJsonDocument(const QJsonDocument& jsonDocument) const final;

    /**
     * Save widget action to JSON document
     * @return JSON document
     */
    virtual QJsonDocument toJsonDocument() const final;

    /**
     * Load widget action from JSON file
     * @param filePath Path to the JSON file (if none/invalid a file open dialog is automatically opened)
     */
    virtual void fromJsonFile(const QString& filePath = "") final;

    /**
     * Save widget action from JSON file
     * @param filePath Path to the JSON file (if none/invalid a file save dialog is automatically opened)
     */
    virtual void toJsonFile(const QString& filePath = "") final;

protected:

    /**
     * Set whether serialization is taking place
     * @param isSerializing Whether serialization is taking place
     */
    void setIsSerializing(bool isSerializing);

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    virtual QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags);

signals:

    /**
     * Signals that serializing is currently being performed or not
     * @param isSerializing Whether serializing is currently being performed or not
     */
    void isSerializingChanged(bool isSerializing);

    /**
     * Signals that the published states changed
     * @param isPublished Whether the action is published or not
     */
    void isPublishedChanged(const bool& isPublished);

    /**
     * Signals that the connected state changed
     * @param isConnected Whether the action is connected or not
     */
    void isConnectedChanged(const bool& isConnected);

    /**
     * Signals that an action connected
     * @param action Action that connected
     */
    void actionConnected(const WidgetAction* action);

    /**
     * Signals that an action disconnected
     * @param action Action that disconnected
     */
    void actionDisconnected(const WidgetAction* action);

protected:
    std::int32_t                _defaultWidgetFlags;    /** Default widget flags */
    std::int32_t                _sortIndex;             /** Sort index (used in the group action to sort actions) */
    bool                        _isSerializing;         /** Whether the widget action is currently serializing */
    WidgetAction*               _publicAction;          /** Public action to which this action might be connected */
    QVector<WidgetAction*>      _connectedActions;      /** Pointers to widget action that are connected to this action */
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Reference to widget action
 */
inline QDebug operator << (QDebug debug, const WidgetAction& widgetAction)
{
    debug.noquote().nospace() << widgetAction.text();

    return debug.space();
}

/**
 * Print widget action to console
 * @param debug Debug
 * @param widgetAction Pointer to widget action
 */
inline QDebug operator << (QDebug debug, WidgetAction* widgetAction)
{
    debug.noquote().nospace() << widgetAction->text();

    return debug.space();
}

}
}
