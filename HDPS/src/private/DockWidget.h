#pragma once

#include <util/Serializable.h>

#include <widgets/InfoOverlayWidget.h>

#include <DockWidget.h>

#include <QMenu>
#include <QUuid>

class QToolButton;

/**
 * Dock widget class
 *
 * ADS dock widget class enhanced with:
 * - Serialization
 * - Widget overlay for display of additional information
 * - Ability to choose additional settings from tool button in tab bar
 *
 * @author Thomas Kroes
 */
class DockWidget : public ads::CDockWidget, public hdps::util::Serializable
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    DockWidget(const QString& title, QWidget* parent = nullptr);

    /** Destructor */
    ~DockWidget();

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    virtual QString getTypeString() const;

    /** Performs custom dock widget initialization */
    virtual void initialize() {};

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

    /**
     * Get info overlay widget (for showing docking information)
     * @return Reference to info overlay widget
     */
    hdps::gui::InfoOverlayWidget& getInfoOverlayWidget();

    /**
     * Get settings menu (when derived dock widgets return a valid menu, a tool button will in the tab bar that shows this menu)
     * @return Pointer to settings menu
     */
    virtual QMenu* getSettingsMenu();

    /**
     * Override ADS dock widget
     * @param widget Pointer to widget
     * @param insertMode Insert modus
     */
    void setWidget(QWidget* widget, eInsertMode insertMode = AutoScrollArea);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    QString                         _id;                    /** Unique identifier by which to identify the dock widget */
    hdps::gui::InfoOverlayWidget    _infoOverlayWidget;     /** Info overlay widget for showing loading information */
    QToolButton*                    _settingsToolButton;    /** Pointer to settings tool button (located in the dock widget tab bar) */
    QMenu                           _settingsMenu;          /** Settings menu for settings tool button */
};

using DockWidgets = QVector<DockWidget*>;