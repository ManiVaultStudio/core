// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QMap>

namespace mv::util {

/** Preset types enum */
enum class PresetType
{
    Normal,             /** Normal preset */
    Global,             /** Global preset (not coupled to a dataset/plugin etc.) */
    FactoryDefault      /** Factory default */
};

/** Maps preset type enum to name */
static const QMap<PresetType, QString> presetTypes = {
    { PresetType::Normal, "Normal" },
    { PresetType::Global, "Global" },
    { PresetType::FactoryDefault, "FactoryDefault" }
};

/**
 * Preset class
 *
 * Class for storing preset properties
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Preset
{
protected:

    /**
     * Constructor
     * @param name Name of the preset
     * @param description Preset description
     * @param presetType Type of preset
     */
    Preset(const QString& name, const QString& description, const PresetType& presetType = PresetType::Normal);

public:

    /**
     * Get preset name
     * @return Preset name
     */
    QString getName() const;

    /**
     * Get preset description
     * @return Preset description
     */
    QString getDescription() const;

    /**
     * Get icon
     * @return Icon
     */
    QIcon getIcon() const;

    /**
     * Get is divergent
     * @return Whether the preset is divergent from current widget action settings
     */
    bool isDivergent() const;

    /**
     * Set is divergent
     * @param isDivergent Whether the preset is divergent from current widget action settings
     */
    void isDivergent(bool isDivergent);

private:
    const QString       _name;          /** Preset internal name */
    const QString       _description;   /** Preset description */
    const PresetType    _presetType;    /** The type of preset */
    bool                _isDivergent;   /** Whether the preset is divergent from current widget action settings */

    /** Only the presets model may instantiate this class */
    friend class PresetsModel;
};

}
