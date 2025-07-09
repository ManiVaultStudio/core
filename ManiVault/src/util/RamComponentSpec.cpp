// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RamComponentSpec.h"

#ifdef Q_OS_WIN
	#define _WIN32_WINNT 0x0600
	#include <windows.h>
#endif

#ifdef Q_OS_LINUX
	#include <sys/sysinfo.h>
#endif

#ifdef Q_OS_MAC
    #include <sys/sysctl.h>
#endif

#include <QSysInfo>

namespace mv::util
{

quint64 parseByteSize(const QString& input) {
    static const QRegularExpression regex(R"(^(\d+)([KMGTP]i?B)$)", QRegularExpression::CaseInsensitiveOption);
    static const QHash<QString, quint64> multipliers = {
        { "B",     1ull },
        { "KB",    1000ull },
        { "MB",    1000ull * 1000 },
        { "GB",    1000ull * 1000 * 1000 },
        { "TB",    1000ull * 1000 * 1000 * 1000 },
        { "PB",    1000ull * 1000 * 1000 * 1000 * 1000 },

        { "KIB",   1024ull },
        { "MIB",   1024ull * 1024 },
        { "GIB",   1024ull * 1024 * 1024 },
        { "TIB",   1024ull * 1024 * 1024 * 1024 },
        { "PIB",   1024ull * 1024 * 1024 * 1024 * 1024 }
    };

    QRegularExpressionMatch match = regex.match(input.trimmed());
    if (!match.hasMatch()) {
        throw std::invalid_argument(QString("Invalid size format: %1").arg(input).toStdString());
    }

    bool ok = false;
    quint64 number = match.captured(1).toULongLong(&ok);
    if (!ok) {
        throw std::runtime_error("Failed to convert number to integer");
    }

    QString unit = match.captured(2).toUpper();
    quint64 multiplier = multipliers.value(unit, 0);
    if (multiplier == 0) {
        throw std::runtime_error("Unknown unit suffix");
    }

    return number * multiplier;
}

RamComponentSpec::RamComponentSpec() :
    HardwareComponentSpec("RAM"),
    _numberOfBytes(0)
{
}

bool RamComponentSpec::meets(const HardwareComponentSpec& required) const
{
    return *this == dynamic_cast<const RamComponentSpec&>(required) || *this > dynamic_cast<const RamComponentSpec&>(required);
}

QString RamComponentSpec::getFailureString(const HardwareComponentSpec& required) const
{
    if (meets(required))
        return {};

    const auto& ramComponentSpec = dynamic_cast<const RamComponentSpec&>(required);

    return QString("Not enough RAM (installed: %2, required: %3").arg(QString::number(ramComponentSpec._numberOfBytes), QString::number(ramComponentSpec._numberOfBytes));
}

QStandardItem* RamComponentSpec::getStandardItem() const
{
    auto item = HardwareComponentSpec::getStandardItem();

    item->setIcon(StyledIcon("memory"));

    auto systemRamComponentSpec = HardwareSpec::getSystemHardwareSpec().getHardwareComponentSpec<RamComponentSpec>("RAM");

    item->appendRow(getParameterRow("Installed", getNoBytesHumanReadable(systemRamComponentSpec->getNumberOfBytes()), getNoBytesHumanReadable(_numberOfBytes), systemRamComponentSpec->getNumberOfBytes() >= _numberOfBytes));

    return item;
}

void RamComponentSpec::fromSystem()
{
    _numberOfBytes = getTotalSystemRAMBytes();

    setInitialized();
}

void RamComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
    try {
        if (!variantMap.contains("ram"))
            throw std::runtime_error("Variant map does not contain 'ram' key.");

        const auto ramString = variantMap.value("ram", 0).toString();

    	_numberOfBytes = parseByteSize(ramString);

    	setInitialized();
    }
    catch (std::exception& e)
    {
        qCritical() << "Unable read RAM hardware component properties from variant map:" << e.what();
    }
    catch (...)
    {
        qCritical() << "Unable read RAM hardware component properties from variant map";
    }
}

std::uint64_t RamComponentSpec::getTotalSystemRAMBytes()
{
#if defined(Q_OS_WIN)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
#elif defined(Q_OS_MAC)
    int64_t mem;
    size_t len = sizeof(mem);
    sysctlbyname("hw.memsize", &mem, &len, nullptr, 0);
    return mem;
#elif defined(Q_OS_UNIX)
    struct sysinfo info;
    sysinfo(&info);
    return quint64(info.totalram) * info.mem_unit;
#else
    return 0;
#endif
}

}
