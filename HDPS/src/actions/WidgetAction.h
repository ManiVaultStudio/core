#pragma once

#include "WidgetActionWidget.h"

#include "Plugin.h"

#include <QWidgetAction>
#include <QMenu>
#include <QDebug>
#include <QFlags>

class QLabel;

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
    WidgetAction(QObject* parent);

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
    virtual QMenu* getContextMenu(QWidget* parent = nullptr)
    {
        return nullptr;
    };

    /**
     * Create widget for the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags
     */
    QWidget* createWidget(QWidget* parent, const std::int32_t& widgetFlags);

    /** Determines whether a user may reset the action to the default value */
    virtual bool getMayReset() const;

    /** Set whether a user may reset the action to the default value */
    virtual void setMayReset(const bool& mayReset);
    
    /** Get the sort index */
    std::int32_t getSortIndex() const;

    /**
     * Set the sort index
     * @param sortIndex Sorting index
     */
    void setSortIndex(const std::int32_t& sortIndex);

    /** Determines whether the action can be reset to its default */
    virtual bool isResettable() const;

    /** Sets the action resettable */
    virtual void setResettable(const bool& resettable);

    /** Reset to default */
    virtual void reset();

    /** Gets the default widget flags */
    std::int32_t getDefaultWidgetFlags() const;

    /**
     * Set the widget flags
     * @param widgetFlags Widget flags
     */
    void setDefaultWidgetFlags(const std::int32_t& widgetFlags);

public: // Settings

    /** Get settings prefix */
    QString getSettingsPrefix() const;

    /**
     * Set the settings prefix globally (not in the plugin namespace)
     * @param settingsPrefix Prefix for the widget action settings
     */
    void setSettingsPrefix(const QString& settingsPrefix);

    /**
     * Set the settings prefix relative to a plugin
     * @param settingsPrefix Prefix for the widget action settings
     * @param plugin Pointer to plugin
     */
    void setSettingsPrefix(const QString& settingsPrefix, const plugin::Plugin* plugin);

    /** Get whether the widget action can serialized to settings */
    bool hasSettingsPrefix() const
    {
        return !_settingsPrefix.isEmpty();
    }

    /** Load default value from disk */
    void loadDefault();

    /** Save default value to disk */
    void saveDefault();

    /**
     * Set action value from variant
     * @param value Value
     */
    virtual void fromVariant(const QVariant& value)
    {
        qDebug() << "From variant not implemented in derived widget action class";
    }

    /**
     * Convert action value to variant
     * @return Value as variant
     */
    virtual QVariant toVariant() const
    {
        qDebug() << "To variant not implemented in derived widget action class";

        return QVariant();
    }

protected:

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    virtual QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags);

signals:

    /**
     * Signals that the resettable-ness changed
     * @param isResettable Whether the widget action can be reset
     */
    void resettableChanged(const bool& isResettable);

protected:
    const DataHierarchyItem*    _dataHierarchyItemContext;      /** The widget action resides somewhere in the data hierarchy item */
    std::int32_t                _defaultWidgetFlags;            /** Default widget flags */
    bool                        _resettable;                    /** Whether the action can be reset */
    bool                        _mayReset;                      /** Whether the action may be reset (from the user interface) */
    std::int32_t                _sortIndex;                     /** Sort index (used in the group action to sort actions) */
    QString                     _settingsPrefix;                /** Settings prefix for persistent widget action properties (e.g. presets and defaults) */
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
