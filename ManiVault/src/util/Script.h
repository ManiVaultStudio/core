// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QMap>
#include <QString>

namespace mv::util
{

/**
 * Script class
 *
 * Contains information for running a script
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Script : public QObject
{
    Q_OBJECT

public:

    /** Script types */
    enum class Type {
        None = 0,               /** Not a valid script type */
        DataLoader,             /** Data loading scripts */
        DataWriter,             /** Data writer scripts */
        DataAnalysis,           /** Data analysis scripts */
        DataTransformation,     /** Data transformation scripts */
        DataView                /** Data view scripts */
    };

    /** Maps type to type name */
    static QMap<Type, QString> typeMap;

    /**
     * Get the type name from \p type
     * @param type Type enum
     * @return Script type name
     */
    static QString getTypeName(const Type& type);

    /**
     * Get type enum from \p typeName
     * @param typeName Type name
     * @return Script type enum
     */
    static Type getTypeEnum(const QString& typeName);

    /** Script language */
    enum class Language {
        None = 0,   /** Not a valid script language */
    	Python,     /** Python scripting */
        R           /** R scripting */
    };

    /** Maps language enum to language name */
    static QMap<Language, QString> languageMap;

    /**
     * Get the language name from \p language
     * @param language Language enum
     * @return Language name
     */
    static QString getLanguageName(const Language& language);

    /**
     * Get language enum from \p language name
     * @param languageName Language name
     * @return Language enum
     */
    static Language getLanguageEnum(const QString& languageName);

    /**
     * Construct script with \p type and \p language
     * @param title Script title
     * @param type Script type
     * @param language Script language
     * @param location Script location
     * @param datasets List of datasets that the script can work with (optional, default is empty)
     * @param parent Pointer to parent object (optional, default is nullptr)
     */
    explicit Script(const QString& title, const Type& type, const Language& language, const QUrl& location, const Datasets& datasets, QObject* parent = nullptr);

    /** Runs the script */
    void run();

    QString getTitle() const { return _title; }       /** Get script title */
    Type     getType() const { return _type; }         /** Get script type */
    Language getLanguage() const { return _language; } /** Get script language */
    QUrl     getLocation() const { return _location; } /** Get script location */
    QIcon    getLanguageIcon() const;

private:
    QString     _title;         /** Script title */
    Type        _type;          /** Script type */
    Language    _language;      /** Script language */
    QUrl        _location;      /** Script location */
    Datasets    _datasets;      /** Datasets that the script can work with */
};

using Scripts = std::vector<const Script*>;

}
