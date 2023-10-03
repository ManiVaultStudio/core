// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FontAwesome.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace hdps {

FontAwesome::FontAwesome(const std::uint32_t& majorVersion /*= 5*/, const std::uint32_t& minorVersion /*= 14*/, bool defaultFont /*= false*/) :
    IconFont("FontAwesome", majorVersion, minorVersion, defaultFont),
    _metaDataResource(QString(":/IconFonts/%1.json").arg(getFullName()))
{
    initialize();
}

void FontAwesome::initialize()
{
    IconFont::initialize();

    QFile iconFontMetaDataFile;

    iconFontMetaDataFile.setFileName(_metaDataResource);
    iconFontMetaDataFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString iconFontMetaData = iconFontMetaDataFile.readAll();

    iconFontMetaDataFile.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(iconFontMetaData.toUtf8());

    QJsonObject jObject = jsonDocument.object();

    QVariantMap mainMap = jObject.toVariantMap();

    for (auto iconName : mainMap.keys()) {
        auto status = false;

        const auto codePoint = mainMap[iconName].toMap()["unicode"].toString().toUInt(&status, 16);

        if (!status)
            throw std::runtime_error("Cannot establish unicode code point");

        _characters[iconName] = QChar(codePoint);
    }

    qDebug() << QString("Found %1 icons").arg(_characters.count());
}

QString FontAwesome::getIconCharacter(const QString& iconName) const
{
    try
    {
        if (!_characters.contains(iconName))
            throw std::runtime_error(QString("%1 not found").arg(iconName).toStdString().c_str());

        return _characters[iconName];
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An icon font error occurred", QString("Failed to fetch icon character: %1").arg(e.what()));
    }
    catch (...) {
        QMessageBox::critical(nullptr, "An icon font error occurred", "Failed to fetch icon character due to an unhandled exception");
    }

    return 0;
}

}