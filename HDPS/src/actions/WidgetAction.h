#pragma once

#include "WidgetActionWidget.h"

#include <QWidgetAction>
#include <QJsonDocument>

class QLabel;
class QMenu;

namespace hdps {

class DataHierarchyItem;

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
     * @return Pointer to widget
     */
    QWidget* createLabelWidget(QWidget* parent);

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

public: // Settings

    /**
     * Establish whether there is a saved default
     * @return Whether there is a saved default
     */
    virtual bool hasSavedDefault() const;

    /**
     * Establish whether a default can be saved
     * @param recursive Check recursively
     * @return Whether a default can be saved
     */
    virtual bool canSaveDefault(bool recursive = true) const;

    /**
     * Load default from settings
     * @param recursive Load recursively
     */
    virtual void loadDefault(bool recursive = true);

    /**
     * Save default to settings
     * @param recursive Save recursively
     */
    virtual void saveDefault(bool recursive = true);

    /**
     * Determines whether the action can be reset to its default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its default
     */
    virtual bool isResettable(bool recursive = true) const;

    /**
     * Determines whether the action can be reset to its factory default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its factory default
     */
    virtual bool isFactoryResettable(bool recursive = true) const;

    /** Notify others if the action is (factory) resettable */
    virtual void notifyResettable() final;

    /**
     * Reset to factory default
     * @param recursive Reset to factory default recursively
     */
    virtual void reset(bool recursive = true);

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
     * Get whether the widget action may be serialized
     * @return Boolean indicating whether the widget action is serializable
     */
    virtual bool isSerializable() const;

    /**
     * Set whether the widget action is serializable
     * @param serializable whether the widget action is serializable
     * @param recursive Set serializable recursively
     */
    virtual void setSerializable(const bool& serializable, bool recursive = true);

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

public: // Serialization proxy

    /**
     * Get serialization proxy parent
     * @return Pointer to serialization proxy parent
     */
    WidgetAction* getSerializationProxyParent();

    /**
     * Get whether a serialization proxy parent is specified
     * @return Whether a serialization proxy parent is specified
     */
    bool hasSerializationProxyParent() const;

    /**
     * Set serialization proxy parent
     * @param Pointer to serialization proxy parent
     */
    void setSerializationProxyParent(WidgetAction* serializationProxyParent);

signals:

    /**
     * Signals that the resettable-ness changed
     * @param isResettable Whether the widget action can be reset to default
     */
    void resettableChanged(bool isResettable);

    /**
     * Signals that the factory resettable-ness changed
     * @param isFactoryResettable Whether the widget action can be reset to factory default
     */
    void factoryResettableChanged(bool isFactoryResettable);

    /**
     * Signals that serializing is currently being performed or not
     * @param isSerializing Whether serializing is currently being performed or not
     */
    void isSerializingChanged(bool isSerializing);

protected:
    std::int32_t    _defaultWidgetFlags;            /** Default widget flags */
    std::int32_t    _sortIndex;                     /** Sort index (used in the group action to sort actions) */
    bool            _serializable;                  /** Whether the widget action can be serialized/de-serialized */
    bool            _isSerializing;                 /** Whether the widget action is currently serializing */
    WidgetAction*   _serializationProxyParent;      /** If specified, uses this proxy parent widget in stead of the QObject parent for de-serialization */
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
