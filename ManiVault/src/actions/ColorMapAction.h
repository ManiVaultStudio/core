// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "util/ColorMap.h"
#include "util/ColorMapModel.h"
#include "util/ColorMapFilterModel.h"

#include "OptionAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "IntegralAction.h"
#include "HorizontalGroupAction.h"
#include "ColorMapSettings1DAction.h"
#include "ColorMapSettings2DAction.h"
#include "ColorMapEditor1DAction.h"
#include "ColorMapSettingsAction.h"

namespace mv::gui {

/**
 * Color map action class
 *
 * Color map selection and settings action
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorMapAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations (a color map combobox always exists) */
    enum WidgetFlag {
        Settings    = 0x00001,      /** Widgets have a settings popup to adjust range and other settings */
        EditRange   = 0x00002,      /** Users are allowed to change the color map range */

        Default = Settings | EditRange
    };

    /** Axis enum for distinguishing between x- and y axis part of the color map (range) */
    enum class Axis {
        X = 0,      /** Along x-axis */
        Y,          /** Along y-axis */

        Count
    };

public:

    /** Combobox widget class for display of a color map */
    class CORE_EXPORT ComboBoxWidget : public OptionAction::ComboBoxWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         * @param colorMapAction Pointer to color map action
         */
        ComboBoxWidget(QWidget* parent, OptionAction* optionAction, ColorMapAction* colorMapAction);

        /**
         * Paint event to override default paint
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent) override;

    protected:
        ColorMapAction*   _colorMapAction;      /** Pointer to color map action */

        friend class ColorMapAction;
    };

protected:

    /**
     * Get widget representation of the color map action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

protected:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMapType Type of color map (1D/2D)
     * @param colorMap Current color map
     */
    Q_INVOKABLE ColorMapAction(QObject* parent, const QString& title, const util::ColorMap::Type& colorMapType = util::ColorMap::Type::OneDimensional, const QString& colorMap = "RdYlBu");

public:

    /**
     * Initialize the color map action
     * @param colorMap Current color map
     */
    void initialize(const QString& colorMap);

    /** Gets the current color map type */
    util::ColorMap::Type getColorMapType() const;

protected:

    /** Sets the current color map type */
    void setColorMapType(const util::ColorMap::Type& colorMapType);

public: // Option action wrappers

    /** Gets the current color map name */
    QString getColorMap() const;

    /** Gets the current color map */
    QImage getColorMapImage() const;

    /**
     * Sets the current color map name
     * @param colorMap Name of the current color map
     */
    void setColorMap(const QString& colorMap);

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

signals:

    /**
     * Signals that the current color map type changed
     * @param colorMapType Current color map type
     */
    void typeChanged(const util::ColorMap::Type& colorMapType);

    /**
     * Signals that the current color map image changed
     * @param image Current color map image
     */
    void imageChanged(const QImage& image);

public: // Action getters

    const OptionAction& getCurrentColorMapAction() const { return _currentColorMapAction; }
    const DecimalRangeAction& getRangeAction(const Axis& axis) const { return _rangeAction[static_cast<int>(axis)]; }
    const DecimalRangeAction& getDataRangeAction(const Axis& axis) const { return _dataRangeAction[static_cast<int>(axis)]; }
    const DecimalRangeAction& getSharedDataRangeAction(const Axis& axis) const { return _sharedDataRangeAction[static_cast<int>(axis)]; }
    const ToggleAction& getLockToSharedDataRangeAction() const { return _lockToSharedDataRangeAction; }
    const ToggleAction& getMirrorAction(const Axis& axis) const { return _mirrorAction[static_cast<int>(axis)]; }
    const HorizontalGroupAction& getMirrorGroupAction() const { return _mirrorGroupAction; }
    const ToggleAction& getDiscretizeAction() const { return _discretizeAction; }
    const IntegralAction& getNumberOfDiscreteStepsAction()const { return _numberOfDiscreteStepsAction; }
    const ToggleAction& getDiscretizeAlphaAction() const { return _discretizeAlphaAction; }
    const ColorMapSettings1DAction& getSettings1DAction() const { return _settings1DAction; }
    const ColorMapSettings2DAction& getSettings2DAction() const { return _settings2DAction; }
    const ToggleAction& getCustomColorMapAction() const { return _customColorMapAction; }
    const ColorMapEditor1DAction& getEditor1DAction() const { return _editor1DAction; }
    const HorizontalGroupAction& getCustomColorMapGroupAction() const { return _customColorMapGroupAction; }
    const ColorMapSettingsAction& getSettingsAction() const { return _settingsAction; }

    OptionAction& getCurrentColorMapAction() { return _currentColorMapAction; }
    DecimalRangeAction& getRangeAction(const Axis& axis) { return _rangeAction[static_cast<int>(axis)]; }
    DecimalRangeAction& getDataRangeAction(const Axis& axis) { return _dataRangeAction[static_cast<int>(axis)]; }
    DecimalRangeAction& getSharedDataRangeAction(const Axis& axis) { return _sharedDataRangeAction[static_cast<int>(axis)]; }
    ToggleAction& getLockToSharedDataRangeAction() { return _lockToSharedDataRangeAction; }
    ToggleAction& getMirrorAction(const Axis& axis) { return _mirrorAction[static_cast<int>(axis)]; }
    HorizontalGroupAction& getMirrorGroupAction() { return _mirrorGroupAction; }
    ToggleAction& getDiscretizeAction() { return _discretizeAction; }
    IntegralAction& getNumberOfDiscreteStepsAction() { return _numberOfDiscreteStepsAction; }
    ToggleAction& getDiscretizeAlphaAction() { return _discretizeAlphaAction; }
    ColorMapSettings1DAction& getSettings1DAction() { return _settings1DAction; }
    ColorMapSettings2DAction& getSettings2DAction() { return _settings2DAction; }
    ToggleAction& getCustomColorMapAction() { return _customColorMapAction; }
    ColorMapEditor1DAction& getEditor1DAction() { return _editor1DAction; }
    HorizontalGroupAction& getCustomColorMapGroupAction() { return _customColorMapGroupAction; }
    ColorMapSettingsAction& getSettingsAction() { return _settingsAction; }

protected:
    util::ColorMapFilterModel   _colorMapFilterModel;                                   /** The filtered color map model (contains either 1D or 2D color maps) */
    OptionAction                _currentColorMapAction;                                 /** Current color map selection action */
    DecimalRangeAction          _rangeAction[static_cast<int>(Axis::Count)];            /** Color map range action (1D/2D color map) */
    DecimalRangeAction          _dataRangeAction[static_cast<int>(Axis::Count)];        /** Range of the associated dataset (1D/2D color map) */
    DecimalRangeAction          _sharedDataRangeAction[static_cast<int>(Axis::Count)];  /** Shared range (1D/2D color map) of all connected color maps (if the color map is connected to a public color map) */
    ToggleAction                _lockToSharedDataRangeAction;                           /** Lock to shared data range action */
    ToggleAction                _mirrorAction[static_cast<int>(Axis::Count)];           /** Mirror along the horizontal/vertical axis action */
    HorizontalGroupAction       _mirrorGroupAction;                                     /** Group action for mirroring along horizontal/vertical axis */
    ToggleAction                _discretizeAction;                                      /** Discretize setEnabled action */
    IntegralAction              _numberOfDiscreteStepsAction;                           /** Number of discrete steps action */
    ToggleAction                _discretizeAlphaAction;                                 /** Whether to also discretize the alpha channel of the color map */
    ColorMapSettings1DAction    _settings1DAction;                                      /** One-dimensional settings action */
    ColorMapSettings2DAction    _settings2DAction;                                      /** Two-dimensional settings action */
    ToggleAction                _customColorMapAction;                                  /** Toggle custom color map action */
    ColorMapEditor1DAction      _editor1DAction;                                        /** One-dimensional editor action */
    HorizontalGroupAction       _customColorMapGroupAction;                             /** Groups the custom color map and editor action */
    ColorMapSettingsAction      _settingsAction;                                        /** Color map settings action */

    friend class AbstractActionsManager;
    friend class ColorMap1DAction;
    friend class ColorMap2DAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapAction)

inline const auto colorMapActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapAction*>("mv::gui::ColorMapAction");
