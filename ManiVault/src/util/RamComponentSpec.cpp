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

    return QString("Insufficient RAM: %2 &lt; %3").arg(getNoBytesHumanReadable(_numberOfBytes), getNoBytesHumanReadable(ramComponentSpec._numberOfBytes));
}

QStandardItem* RamComponentSpec::getStandardItem() const
{
    auto item = HardwareComponentSpec::getStandardItem();

    item->setIcon(StyledIcon("memory"));

    auto systemRamComponentSpec = HardwareSpec::getSystemHardwareSpec().getHardwareComponentSpec<RamComponentSpec>("RAM");

    item->appendRow(getParameterRow("Installed", getNoBytesHumanReadable(systemRamComponentSpec->getNumberOfBytes(), false), getNoBytesHumanReadable(_numberOfBytes), systemRamComponentSpec->getNumberOfBytes() >= _numberOfBytes));

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
