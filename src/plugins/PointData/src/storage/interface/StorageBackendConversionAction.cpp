// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConversionAction.h"

#include "../../PointData.h"

#include <QStringList>

#include <stdexcept>

namespace mv::gui
{

StorageBackendConversionAction::StorageBackendConversionAction(QObject* parent, const QString& title, const std::optional<std::uint32_t> currentElementTypeIndex) :
    GroupAction(parent, title),
    _elementTypeAction(this, "Element type")
{
    if (!currentElementTypeIndex)
        return;

    const auto elementTypeNames = PointData::getElementTypeNames();
    if (*currentElementTypeIndex >= elementTypeNames.size())
        throw std::invalid_argument("Storage conversion uses an unsupported element type");

    QStringList elementTypeOptions;
    for (const auto* elementTypeName : elementTypeNames)
        elementTypeOptions.append(QString::fromLatin1(elementTypeName));

    _hasElementTypeAction = true;
    _elementTypeAction.setOptions(elementTypeOptions);
    _elementTypeAction.setCurrentIndex(static_cast<int>(*currentElementTypeIndex));
    addAction(&_elementTypeAction);

    connect(&_elementTypeAction, &OptionAction::currentIndexChanged, this, [this] {
        notifyOptionsChanged();
    });
}

std::optional<std::uint32_t> StorageBackendConversionAction::getTargetElementTypeIndex() const
{
    if (!_hasElementTypeAction)
        return {};

    const auto index = _elementTypeAction.getCurrentIndex();
    return index >= 0 ? std::optional<std::uint32_t>(static_cast<std::uint32_t>(index)) : std::nullopt;
}

std::optional<std::size_t> StorageBackendConversionAction::getTargetElementSize() const
{
    const auto elementTypeIndex = getTargetElementTypeIndex();
    return elementTypeIndex ? std::optional<std::size_t>(PointData::getElementTypeSize(*elementTypeIndex)) : std::nullopt;
}

}
