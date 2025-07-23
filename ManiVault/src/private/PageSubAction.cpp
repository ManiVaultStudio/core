// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageSubAction.h"

using namespace mv::util;

PageSubAction::PageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback /*= {}*/, const ClickedCallback& clickedCallback /*= {}*/) :
    _icon(icon),
    _clickedCallback(clickedCallback),
	_tooltipCallback(tooltipCallback)
{
}

QString PageSubAction::getTooltip() const
{
    if (_tooltipCallback)
        return _tooltipCallback();

    return {};
}

CommentsPageSubAction::CommentsPageSubAction(const QString& comments) :
	PageSubAction(mv::util::StyledIcon("scroll"))
{
    setTooltipCallback([this, comments]() -> QString {
        return comments;
    });
}

TagsPageSubAction::TagsPageSubAction(const QStringList& tags) :
    PageSubAction(mv::util::StyledIcon("tags"))
{
    setTooltipCallback([this, tags]() -> QString {
        return tags.join(", ");
    });
}

PageDownloadSubAction::PageDownloadSubAction(const QString& downloadSize) :
    PageSubAction(mv::util::StyledIcon("download"))
{
    setTooltipCallback([this, downloadSize]() -> QString {
        return QString("Requires a %1 download").arg(downloadSize);
    });
}

PageCompatibilitySubAction::PageCompatibilitySubAction(const HardwareSpec::SystemCompatibilityInfo& systemCompatibilityInfo) :
    PageSubAction({})
{
	switch (systemCompatibilityInfo._compatibility) {
		case HardwareSpec::SystemCompatibility::Incompatible:
		{
			setIcon(StyledIcon("circle-xmark"));
			setTooltipCallback([this]() -> QString {
				return "Your system does not meet the minimum requirements for this project, there might be problems with opening it, its stability and performance!";
			});

			break;
		}

        case HardwareSpec::SystemCompatibility::Minimum:
        {
            setIcon(StyledIcon("circle-exclamation"));
            setTooltipCallback([this]() -> QString {
                return "Your system does not meet the recommended requirements for this project, the interactivity might not be optimal!";
            });

            break;
        }

        case HardwareSpec::SystemCompatibility::Compatible:
        {
            setIcon(StyledIcon("circle-check"));
            setTooltipCallback([this]() -> QString {
                return "Your system meets the minimum requirements for this project!";
            });
			
            break;
        }

        case HardwareSpec::SystemCompatibility::Unknown:
        {
            setIcon(StyledIcon("circle-question"));
            setTooltipCallback([this]() -> QString {
                return "Your system compatibility is unknown, the project does not specify hardware requirements!";
            });
			
            break;
        }
	}
}

ProjectPreviewPageSubAction::ProjectPreviewPageSubAction() :
    PageSubAction(mv::util::StyledIcon("image"))
{
}

ContributorsPageSubAction::ContributorsPageSubAction(const QStringList& contributors) :
    PageSubAction(mv::util::StyledIcon("user"))
{
    setTooltipCallback([this, contributors]() -> QString {
        return QString("Contributor(s): %1").arg(contributors.join(", "));
	});
}

