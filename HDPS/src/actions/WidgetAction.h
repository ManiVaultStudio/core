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
     * Get name
     * @return Name
     */
    QString getName() const;

    /**
     * Set name
     * @param name Name
     */
    void setName(const QString& name);

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

    /**
     * Determines whether the action can be reset to its default
     * @return Whether the action can be reset to its default
     */
    virtual bool isResettable() const final;

    /**
     * Determines whether the action can be reset to its factory default
     * @return Whether the action can be reset to its factory default
     */
    virtual bool isFactoryResettable() const final;

    /** Sets the action resettable */
    virtual void setResettable(const bool& resettable);

    /** Reset to default */
    virtual void reset() final;

    /** Gets the default widget flags */
    std::int32_t getDefaultWidgetFlags() const;

    /**
     * Set the widget flags
     * @param widgetFlags Widget flags
     */
    void setDefaultWidgetFlags(const std::int32_t& widgetFlags);

public: // Settings

    /**
     * Get whether the widget action is serializable
     * @return Whether the widget action is serializable
     */
    bool isSerializable() const;

    /**
     * Set whether the widget action is serializable
     * @param serializable whether the widget action is serializable
     * @param recursive Set serializable recursively
     */
    void setSerializable(const bool& serializable, bool recursive = true);

    /**
     * Establish whether there is a saved default
     * @return Whether there is a saved default
     */
    bool hasSavedDefault() const;

    /**
     * Establish whether a default can be saved
     * @return Whether a default can be saved
     */
    bool canSaveDefault() const;

    /**
     * Load default from settings
     * @param recursive Load recursively
     */
    void loadDefault(bool recursive = true);

    /**
     * Save default to settings
     * @param recursive Save recursively
     */
    void saveDefault(bool recursive = true);

    /**
     * Set value from variant
     * @param value Value
     */
    virtual void setValueFromVariant(const QVariant& value)
    {
        qDebug() << "Set value not implemented in " << text();
    }

    /**
     * Convert value to variant
     * @return Value as variant
     */
    virtual QVariant valueToVariant() const
    {
        qDebug() << "Value to variant not implemented in " << text();

        return QVariant();
    }

    /**
     * Load saved default value to variant
     * @return Saved default value as variant
     */
    virtual QVariant savedDefaultValueToVariant() const
    {
        return Application::current()->getSetting(getSettingsPath() + "/Default");
    }

    /**
     * Convert default value to variant
     * @return Default value as variant
     */
    virtual QVariant defaultValueToVariant() const
    {
        qDebug() << "Default value to variant not implemented in derived widget action class";

        return QVariant();
    }

    /**
     * Get settings path
     * @return Path of the action w.r.t. to the top-level action
     */
    QString getSettingsPath() const;

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
    QString         _name;                  /** Name (for internal use like serializing settings etc.) */
    std::int32_t    _defaultWidgetFlags;    /** Default widget flags */
    bool            _resettable;            /** Whether the action can be reset */
    bool            _mayReset;              /** Whether the action may be reset (from the user interface) */
    std::int32_t    _sortIndex;             /** Sort index (used in the group action to sort actions) */
    bool            _serializable;          /** Whether the widget action can be serialized/de-serialized */
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
