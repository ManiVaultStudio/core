#!/usr/bin/env bash

qtroot=$1
bundledir=$2


rsync -a --delete $qtroot/../../QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/ $bundledir/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/

if ! [[ -f $bundledir/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess ]]; then
    echo "No QtWebEngineProcess.app not found. Was macdeployqt successful?"
    exit 1
fi

pushd $bundledir/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS
for LIB in QtGui QtCore QtWebEngineCore QtQuick QtWebChannel QtNetwork QtPositioning QtQmlModels QtQml
do
    OLD_PATH=`otool -L QtWebEngineProcess | grep ${LIB}.framework | cut -f 1 -d ' '`
    NEW_PATH="@loader_path/../../../../../../../${LIB}.framework/${LIB}"
    echo ${OLD_PATH} ${NEW_PATH}
    install_name_tool -change ${OLD_PATH} ${NEW_PATH} QtWebEngineProcess
done
popd
