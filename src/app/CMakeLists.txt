# -----------------------------------------------------------------------------
# Source files for target MV_EXE
# -----------------------------------------------------------------------------
# defines MAIN_SOURCES, PRIVATE_SOURCES and PRIVATE_HEADERS
# all file path are relative to ROOT/src

set(MAIN_SOURCES
    Main.cpp
)

set(PRIVATE_CORE_HEADERS
    Core.h
)

set(PRIVATE_CORE_SOURCES
    Core.cpp
)

set(PRIVATE_CORE_FILES
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_CORE_SOURCES}
)

set(PRIVATE_ACTIONS_HEADERS
    ManiVaultVersionStatusBarAction.h
    PluginsStatusBarAction.h
    LoggingStatusBarAction.h
    BackgroundTasksStatusBarAction.h
    FrontPagesStatusBarAction.h
    SettingsStatusBarAction.h
    WorkspaceStatusBarAction.h
)

set(PRIVATE_ACTIONS_SOURCES
    PluginsStatusBarAction.cpp
    ManiVaultVersionStatusBarAction.cpp
    LoggingStatusBarAction.cpp
    BackgroundTasksStatusBarAction.cpp
    FrontPagesStatusBarAction.cpp
    SettingsStatusBarAction.cpp
    WorkspaceStatusBarAction.cpp
)

set(PRIVATE_ACTIONS_FILES
    ${PRIVATE_ACTIONS_HEADERS}    
    ${PRIVATE_ACTIONS_SOURCES}
)

set(PRIVATE_WORKSPACE_MANAGER_HEADERS
    WorkspaceManager.h
    DockManager.h
    DockComponentsFactory.h
    ViewPluginsDockWidget.h
    ViewPluginDockWidget.h
    CentralDockWidget.h
    DockWidget.h
    DockAreaTitleBar.h
    WorkspaceSettingsDialog.h
)

set(PRIVATE_WORKSPACE_MANAGER_SOURCES
    WorkspaceManager.cpp
    DockManager.cpp
    DockComponentsFactory.cpp
    ViewPluginsDockWidget.cpp
    ViewPluginDockWidget.cpp
    CentralDockWidget.cpp
    DockWidget.cpp
    DockAreaTitleBar.cpp
    WorkspaceSettingsDialog.cpp
)

set(PRIVATE_WORKSPACE_MANAGER_FILES
    ${PRIVATE_WORKSPACE_MANAGER_HEADERS}
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
)

set(PRIVATE_PLUGIN_MANAGER_HEADERS
    PluginManager.h
    PluginManagerDialog.h
)

set(PRIVATE_PLUGIN_MANAGER_SOURCES
    PluginManager.cpp
    PluginManagerDialog.cpp
)

set(PRIVATE_PLUGIN_MANAGER_FILES
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
)

set(PRIVATE_DATA_MANAGER_HEADERS
    DataManager.h
    GroupDataDialog.h
    RemoveDatasetsDialog.h
    DatasetsToRemoveModel.h
    DatasetsToRemoveFilterModel.h
)

set(PRIVATE_DATA_MANAGER_SOURCES
    DataManager.cpp
    GroupDataDialog.cpp
    RemoveDatasetsDialog.cpp
    DatasetsToRemoveModel.cpp
    DatasetsToRemoveFilterModel.cpp
)

set(PRIVATE_DATA_MANAGER_FILES
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_SOURCES}
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS
    DataHierarchyManager.h
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES
    DataHierarchyManager.cpp
)

set(PRIVATE_DATA_HIERARCHY_MANAGER_FILES
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
)

set(PRIVATE_EVENT_MANAGER_HEADERS
    EventManager.h
)

set(PRIVATE_EVENT_MANAGER_SOURCES
    EventManager.cpp
)

set(PRIVATE_EVENT_MANAGER_FILES
    ${PRIVATE_EVENT_MANAGER_HEADERS}
    ${PRIVATE_EVENT_MANAGER_SOURCES}
)

set(PRIVATE_ACTIONS_MANAGER_HEADERS
    ActionsManager.h
)

set(PRIVATE_ACTIONS_MANAGER_SOURCES
    ActionsManager.cpp
)

set(PRIVATE_ACTIONS_MANAGER_FILES
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
)

set(PRIVATE_PROJECT_MANAGER_HEADERS
    ProjectManager.h
    ProjectWidget.h
    ProjectSettingsDialog.h
    NewProjectDialog.h
)

set(PRIVATE_PROJECT_MANAGER_SOURCES
    ProjectManager.cpp
    ProjectWidget.cpp
    ProjectSettingsDialog.cpp
    NewProjectDialog.cpp
)

set(PRIVATE_PROJECT_MANAGER_FILES
    ${PRIVATE_PROJECT_MANAGER_HEADERS}
    ${PRIVATE_PROJECT_MANAGER_SOURCES}
)

set(PRIVATE_SETTINGS_MANAGER_HEADERS
    SettingsManager.h
    SettingsManagerDialog.h
)

set(PRIVATE_SETTINGS_MANAGER_SOURCES
    SettingsManager.cpp
    SettingsManagerDialog.cpp
)

set(PRIVATE_SETTINGS_MANAGER_FILES
    ${PRIVATE_SETTINGS_MANAGER_HEADERS}
    ${PRIVATE_SETTINGS_MANAGER_SOURCES}
)

set(PRIVATE_TASK_MANAGER_HEADERS
    TaskManager.h
)

set(PRIVATE_TASK_MANAGER_SOURCES
    TaskManager.cpp
)

set(PRIVATE_TASK_MANAGER_FILES
    ${PRIVATE_TASK_MANAGER_HEADERS}
    ${PRIVATE_TASK_MANAGER_SOURCES}
)

set(PRIVATE_HELP_MANAGER_HEADERS
    HelpManager.h
)

set(PRIVATE_HELP_MANAGER_SOURCES
    HelpManager.cpp
)

set(PRIVATE_HELP_MANAGER_FILES
    ${PRIVATE_HELP_MANAGER_HEADERS}
    ${PRIVATE_HELP_MANAGER_SOURCES}
)

set(PRIVATE_ERROR_MANAGER_HEADERS
    ErrorManager.h
)

set(PRIVATE_ERROR_MANAGER_SOURCES
    ErrorManager.cpp
)

set(PRIVATE_ERROR_MANAGER_FILES
    ${PRIVATE_ERROR_MANAGER_HEADERS}
    ${PRIVATE_ERROR_MANAGER_SOURCES}
)

set(PRIVATE_THEME_MANAGER_HEADERS
    ThemeManager.h
)

set(PRIVATE_THEME_MANAGER_SOURCES
    ThemeManager.cpp
)

set(PRIVATE_THEME_MANAGER_FILES
    ${PRIVATE_THEME_MANAGER_HEADERS}
    ${PRIVATE_THEME_MANAGER_SOURCES}
)

set(PRIVATE_SCRIPTING_MANAGER_HEADERS
    ScriptingManager.h
)

set(PRIVATE_SCRIPTING_MANAGER_SOURCES
    ScriptingManager.cpp
)

set(PRIVATE_SCRIPTING_MANAGER_FILES
    ${PRIVATE_SCRIPTING_MANAGER_HEADERS}
    ${PRIVATE_SCRIPTING_MANAGER_SOURCES}
)

set(PRIVATE_MANAGER_HEADERS
    ${PRIVATE_WORKSPACE_MANAGER_HEADERS}
    ${PRIVATE_PLUGIN_MANAGER_HEADERS}
    ${PRIVATE_DATA_MANAGER_HEADERS}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_HEADERS}
    ${PRIVATE_EVENT_MANAGER_HEADERS}
    ${PRIVATE_ACTIONS_MANAGER_HEADERS}
    ${PRIVATE_PROJECT_MANAGER_HEADERS}
    ${PRIVATE_SETTINGS_MANAGER_HEADERS}
    ${PRIVATE_TASK_MANAGER_HEADERS}
    ${PRIVATE_HELP_MANAGER_HEADERS}
    ${PRIVATE_ERROR_MANAGER_HEADERS}
    ${PRIVATE_THEME_MANAGER_HEADERS}
    ${PRIVATE_SCRIPTING_MANAGER_HEADERS}
    ${PRIVATE_SERIALIZATION_HEADERS}
)

set(PRIVATE_MANAGER_SOURCES
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
    ${PRIVATE_PLUGIN_MANAGER_SOURCES}
    ${PRIVATE_DATA_MANAGER_SOURCES}
    ${PRIVATE_DATA_HIERARCHY_MANAGER_SOURCES}
    ${PRIVATE_EVENT_MANAGER_SOURCES}
    ${PRIVATE_ACTIONS_MANAGER_SOURCES}
    ${PRIVATE_PROJECT_MANAGER_SOURCES}
    ${PRIVATE_WORKSPACE_MANAGER_SOURCES}
    ${PRIVATE_SETTINGS_MANAGER_SOURCES}
    ${PRIVATE_TASK_MANAGER_SOURCES}
    ${PRIVATE_HELP_MANAGER_SOURCES}
    ${PRIVATE_ERROR_MANAGER_SOURCES}
    ${PRIVATE_THEME_MANAGER_SOURCES}
    ${PRIVATE_SCRIPTING_MANAGER_SOURCES}
    ${PRIVATE_SERIALIZATION_SOURCES}
)

set(PRIVATE_MANAGER_FILES
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_MANAGER_SOURCES}
)

set(PRIVATE_APPLICATION_HEADERS
    MainWindow.h
    FileMenu.h
    ViewMenu.h
    LoadSystemViewMenu.h
    LoadedViewsMenu.h
    ProjectsMenu.h
    HelpMenu.h
    UserFeedbackDialog.h
    NoProxyRectanglesFusionStyle.h
    GifEncoder.h
    GifRecorder.h
)

if(WIN32)
    list(APPEND PRIVATE_APPLICATION_HEADERS
        DeveloperMenu.h
    )
endif()

if(MV_USE_ERROR_LOGGING)
	list(APPEND PRIVATE_APPLICATION_HEADERS
		SentryErrorLogger.h
		CrashReportDialog.h
		ErrorLoggingConsentDialog.h
	)
endif()

set(PRIVATE_APPLICATION_SOURCES
    MainWindow.cpp
    FileMenu.cpp
    ViewMenu.cpp
    LoadSystemViewMenu.cpp
    LoadedViewsMenu.cpp
    ProjectsMenu.cpp
    HelpMenu.cpp
    UserFeedbackDialog.cpp
    NoProxyRectanglesFusionStyle.cpp
    GifEncoder.cpp
    GifRecorder.cpp
)

if(WIN32)
    list(APPEND PRIVATE_APPLICATION_SOURCES
        DeveloperMenu.cpp
    )
endif()

if(MV_USE_ERROR_LOGGING)
	list(APPEND PRIVATE_APPLICATION_SOURCES
		SentryErrorLogger.cpp
		CrashReportDialog.cpp
		ErrorLoggingConsentDialog.cpp
	)
endif()

set(PRIVATE_APPLICATION_FILES
    ${PRIVATE_APPLICATION_HEADERS}
    ${PRIVATE_APPLICATION_SOURCES}
)

set(PRIVATE_WORKFLOW_HEADERS
    ProjectOpenWorkflowPlan.h
    ProjectSaveWorkflowPlan.h
    ProjectPublishWorkflowPlan.h
    ProjectPublishContext.h
    ParallelPhantomTestSuite.h
    TaskflowWorkflowPlanExecutor.h
    Taskflow.h
)

set(PRIVATE_WORKFLOW_SOURCES
    ProjectOpenWorkflowPlan.cpp
    ProjectSaveWorkflowPlan.cpp
    ProjectPublishWorkflowPlan.cpp
    ProjectPublishContext.cpp
    ParallelPhantomTestSuite.cpp
    TaskflowWorkflowPlanExecutor.cpp
)

set(PRIVATE_WORKFLOW_FILES
    ${PRIVATE_WORKFLOW_HEADERS}
    ${PRIVATE_WORKFLOW_SOURCES}
)

set(PRIVATE_CODEC_HEADERS
    PassthroughBlobCodec.h
    PassthroughBlobCodecFactory.h
    ZstdBlobCodec.h
    ZstdBlobCodecFactory.h
    ZstdCodecSettingsAction.h
    PassthroughCodecSettingsAction.h
)

set(PRIVATE_CODEC_SOURCES
    PassthroughBlobCodec.cpp
    PassthroughBlobCodecFactory.cpp
    ZstdBlobCodec.cpp
    ZstdBlobCodecFactory.cpp
    ZstdCodecSettingsAction.cpp
    PassthroughCodecSettingsAction.cpp
)

set(PRIVATE_CODEC_FILES
    ${PRIVATE_CODEC_HEADERS}
    ${PRIVATE_CODEC_SOURCES}
)

set(PRIVATE_PAGES_COMMON_HEADERS
    PageWidget.h
    PageHeaderWidget.h
    PageContentWidget.h
    LogoWidget.h
	PageActionsWidget.h
    AbstractPageActionsModel.h
    PageActionsTreeModel.h
    PageActionsFilterModel.h
    PageActionDelegate.h
    PageActionDelegateEditorWidget.h
    PageAction.h
    PageSubAction.h
	PageTutorialsWidget.h
)

set(PRIVATE_PAGES_COMMON_SOURCES
    PageWidget.cpp
    PageHeaderWidget.cpp
    PageContentWidget.cpp
    LogoWidget.cpp
	PageActionsWidget.cpp
    AbstractPageActionsModel.cpp
    PageActionsTreeModel.cpp
    PageActionsFilterModel.cpp
    PageActionDelegate.cpp
    PageActionDelegateEditorWidget.cpp
    PageAction.cpp
    PageSubAction.cpp
	PageTutorialsWidget.cpp
)

set(PRIVATE_PAGES_COMMON_FILES
    ${PRIVATE_PAGES_COMMON_HEADERS}
    ${PRIVATE_PAGES_COMMON_SOURCES}
)

set(PRIVATE_START_PAGE_HEADERS
    StartPageWidget.h
    StartPageContentWidget.h
    StartPageOpenProjectWidget.h
    StartPageGetStartedWidget.h
)

set(PRIVATE_START_PAGE_SOURCES
    StartPageWidget.cpp
    StartPageContentWidget.cpp
    StartPageOpenProjectWidget.cpp
    StartPageGetStartedWidget.cpp
)

set(PRIVATE_START_PAGE_FILES
    ${PRIVATE_START_PAGE_HEADERS}
    ${PRIVATE_START_PAGE_SOURCES}
)

set(PRIVATE_LEARNING_PAGE_HEADERS
    LearningPageWidget.h
    LearningPageContentWidget.h
    LearningPageVideosWidget.h
    LearningPageVideoWidget.h
    LearningPageVideoStyledItemDelegate.h
	LearningPageExamplesWidget.h
    LearningPagePluginResourcesWidget.h
    LearningPagePluginAction.h
)

set(PRIVATE_LEARNING_PAGE_SOURCES
    LearningPageWidget.cpp
    LearningPageContentWidget.cpp
    LearningPageVideosWidget.cpp
    LearningPageVideoWidget.cpp
    LearningPageVideoStyledItemDelegate.cpp
	LearningPageExamplesWidget.cpp
    LearningPagePluginResourcesWidget.cpp
    LearningPagePluginAction.cpp
)

set(PRIVATE_LEARNING_PAGE_FILES
    ${PRIVATE_LEARNING_PAGE_HEADERS}
    ${PRIVATE_LEARNING_PAGE_SOURCES}
)

set(PRIVATE_MISCELLANEOUS_HEADERS
    Archiver.h
)

set(PRIVATE_MISCELLANEOUS_SOURCES
    Archiver.cpp
)

set(PRIVATE_MISCELLANEOUS_FILES
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
)

set(PRIVATE_HEADERS
    ${PRIVATE_CORE_HEADERS}
    ${PRIVATE_MANAGER_HEADERS}
    ${PRIVATE_APPLICATION_HEADERS}
	${PRIVATE_PAGES_COMMON_HEADERS}
    ${PRIVATE_START_PAGE_HEADERS}
	${PRIVATE_LEARNING_PAGE_HEADERS}
    ${PRIVATE_MISCELLANEOUS_HEADERS}
    ${PRIVATE_ACTIONS_HEADERS}
    ${PRIVATE_WORKFLOW_HEADERS}
    ${PRIVATE_CODEC_HEADERS}
)

set(PRIVATE_SOURCES
    ${PRIVATE_CORE_SOURCES}
    ${PRIVATE_MANAGER_SOURCES}
    ${PRIVATE_APPLICATION_SOURCES}
	${PRIVATE_PAGES_COMMON_SOURCES}
    ${PRIVATE_START_PAGE_SOURCES}
	${PRIVATE_LEARNING_PAGE_SOURCES}
    ${PRIVATE_MISCELLANEOUS_SOURCES}
    ${PRIVATE_ACTIONS_SOURCES}
    ${PRIVATE_WORKFLOW_SOURCES}
    ${PRIVATE_CODEC_SOURCES}
    ${PRIVATE_HEADERS}
)

list(REMOVE_DUPLICATES PRIVATE_HEADERS)
list(REMOVE_DUPLICATES PRIVATE_SOURCES)

source_group(Core FILES ${PRIVATE_CORE_FILES})
source_group(Actions FILES ${PRIVATE_ACTION_FILES})
source_group(Application FILES ${PRIVATE_APPLICATION_FILES} ${MAIN_SOURCES})
source_group(Managers\\Layout FILES ${PRIVATE_LAYOUT_MANAGER_FILES})
source_group(Managers\\Plugin FILES ${PRIVATE_PLUGIN_MANAGER_FILES})
source_group(Managers\\Data FILES ${PRIVATE_DATA_MANAGER_FILES})
source_group(Managers\\DataHierarchy FILES ${PRIVATE_DATA_HIERARCHY_MANAGER_FILES})
source_group(Managers\\Event FILES ${PRIVATE_EVENT_MANAGER_FILES})
source_group(Managers\\Actions FILES ${PRIVATE_ACTIONS_MANAGER_FILES})
source_group(Managers\\Project FILES ${PRIVATE_PROJECT_MANAGER_FILES})
source_group(Managers\\Workspace FILES ${PRIVATE_WORKSPACE_MANAGER_FILES})
source_group(Managers\\Settings FILES ${PRIVATE_SETTINGS_MANAGER_FILES})
source_group(Managers\\Task FILES ${PRIVATE_TASK_MANAGER_FILES})
source_group(Managers\\Help FILES ${PRIVATE_HELP_MANAGER_FILES})
source_group(Managers\\Error FILES ${PRIVATE_ERROR_MANAGER_FILES})
source_group(Managers\\Theme FILES ${PRIVATE_THEME_MANAGER_FILES})
source_group(Managers\\Scripting FILES ${PRIVATE_SCRIPTING_MANAGER_FILES})
source_group(Pages\\Common FILES ${PRIVATE_PAGES_COMMON_FILES})
source_group(Pages\\StartPage FILES ${PRIVATE_START_PAGE_FILES})
source_group(Pages\\Learning FILES ${PRIVATE_LEARNING_PAGE_FILES})
source_group(Miscellaneous FILES ${PRIVATE_MISCELLANEOUS_FILES})
source_group(Actions FILES ${PRIVATE_ACTIONS_FILES})
source_group(Workflow FILES ${PRIVATE_WORKFLOW_FILES})
source_group(Codec FILES ${PRIVATE_CODEC_FILES})

# -----------------------------------------------------------------------------
# Resources
# -----------------------------------------------------------------------------
include(ResourceList)                   # defines RESOURCE_FILES, MAIN_APP_ICON_RESOURCE and MAIN_STYLES

# -----------------------------------------------------------------------------
# Target MV_EXE
# -----------------------------------------------------------------------------
add_executable(${MV_EXE} MACOSX_BUNDLE WIN32)

set_target_properties(${MV_EXE} PROPERTIES
    AUTOMOC ON
    OUTPUT_NAME ${MV_APPLICATION_NAME}
    UNITY_BUILD ${MV_UNITY_BUILD}
)

target_sources(${MV_EXE}
    PRIVATE
    ${MAIN_SOURCES}
    ${PRIVATE_SOURCES}
    ${MAIN_STYLES}
    ${RESOURCE_FILES}
    ${MAIN_APP_ICON_RESOURCE}
)

target_include_directories(${MV_EXE} PRIVATE
    ${MV_ROOT_DIR}/src/lib
    ${MV_ROOT_DIR}/res
)

target_include_directories(${MV_EXE} PRIVATE "${nlohmann_json_SOURCE_DIR}/include")
target_include_directories(${MV_EXE} PRIVATE "${valijson_SOURCE_DIR}/include")

target_compile_features(${MV_EXE} PRIVATE cxx_std_${MV_CXX_STANDARD})

target_compile_definitions(${MV_EXE} PRIVATE VALIJSON_USE_EXCEPTIONS=1)

target_compile_options(${MV_EXE} 
    PRIVATE
        $<$<AND:$<BOOL:${MV_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
)

target_link_options(${MV_EXE}
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>
        $<$<CXX_COMPILER_ID:MSVC>:/SUBSYSTEM:CONSOLE>
)

target_link_libraries(${MV_EXE} PRIVATE Qt6::Core)
target_link_libraries(${MV_EXE} PRIVATE Qt6::Gui)
target_link_libraries(${MV_EXE} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::Concurrent)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_EXE} PRIVATE qtadvanceddocking-qt6)
target_link_libraries(${MV_EXE} PRIVATE QuaZip)
target_link_libraries(${MV_EXE} PRIVATE Taskflow)
target_link_libraries(${MV_EXE} PRIVATE libzstd_static)

if(MV_USE_ERROR_LOGGING)
    target_compile_definitions(${MV_EXE} PRIVATE MV_USE_ERROR_LOGGING=1)
    
    if(NOT APPLE)
        target_link_libraries(${MV_EXE} PRIVATE sentry)
        target_link_libraries(${MV_EXE} PRIVATE crashpad_client)
        
        add_dependencies(${MV_EXE} crashpad_handler)
    endif()

    if(NOT APPLE)
        set(MV_CRASH_REPORTER MV_CrashReporter)

        add_executable(${MV_CRASH_REPORTER} WIN32
            CrashReporterMain.cpp
            CrashReportDialog.cpp
            CrashReportDialog.h
            ${RESOURCE_FILES}
        )

        set_target_properties(${MV_CRASH_REPORTER} PROPERTIES
            AUTOMOC ON
            OUTPUT_NAME "ManiVault Crash Reporter"
        )

        target_include_directories(${MV_CRASH_REPORTER} PRIVATE ${MV_ROOT_DIR}/src/lib)
        target_compile_features(${MV_CRASH_REPORTER} PRIVATE cxx_std_${MV_CXX_STANDARD})
        target_link_libraries(${MV_CRASH_REPORTER} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets ${MV_PUBLIC_LIB} sentry)

        add_dependencies(${MV_EXE} ${MV_CRASH_REPORTER})
    endif()
endif()

if(MV_ENABLE_CPPTRACE)
    target_link_libraries(${MV_EXE} PRIVATE cpptrace::cpptrace)
    target_compile_definitions(${MV_EXE} PRIVATE MV_ENABLE_CPPTRACE)
endif()


target_link_libraries(${MV_EXE} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${MV_EXE} ${MV_USE_AVX})

if(MV_PRECOMPILE_HEADERS)
    target_precompile_headers(${MV_EXE} PRIVATE 
        ${PRECOMPILE_HEADERS}
    )
endif()

if(APPLE)
    set_target_properties(${MV_EXE} PROPERTIES
        # Find libraries relative to the executable in the macOS .app tree
        BUILD_WITH_INSTALL_RPATH True
        INSTALL_RPATH "@loader_path;@executable_path/../Frameworks"
        # Debug ManiVault Studio from the install directory
        XCODE_SCHEME_WORKING_DIRECTORY "$<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>"
        XCODE_SCHEME_DEBUG_DOCUMENT_VERSIONING NO
        XCODE_SCHEME_EXECUTABLE "${MV_INSTALL_DIR}/Debug/ManiVault Studio.app"
    )
endif()

# -----------------------------------------------------------------------------
# Installation application
# -----------------------------------------------------------------------------

# install Manivault executable/bundle
if(APPLE)
    # These are the properties for the Info.plist in the mac bundle
    set(MACOSX_BUNDLE_BUNDLE_NAME "ManiVault Studio")
    set(MACOSX_BUNDLE_PATHNAME "ManiVault\ Studio")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${MV_VERSION_MAJOR}.${MV_VERSION_MINOR}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${MV_VERSION_MAJOR}.${MV_VERSION_MINOR}.${MV_VERSION_PATCH}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MV_VERSION_MAJOR}.${MV_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2023. All rights reserved.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.biovault.mv")
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)

    # set(BUNDLE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(BUNDLE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/ManiVault Studio.app")
    set(WEBENGINEINSTALL_CMD "${MV_ROOT_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION bin COMPONENT MACOS_BUNDLE
        BUNDLE DESTINATION . COMPONENT MACOS_BUNDLE
    )
    # -always-overwrite
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

    install(CODE "
        message(\"**** MacOS bundle install ****\")

        # Check if the Frameworks folder exists to determine if macdeployqt already ran
        if(EXISTS \"${BUNDLE_DIR}/Contents/Frameworks/QtCore.framework\")
            message(\"**** Skipping macdeployqt, bundle already deployed. ****\")
        else()
            message(\"**** Running macdeployqt for \"${BUNDLE_DIR}\" ****\")
            execute_process(COMMAND ${MACDEPLOYQT_EXECUTABLE} \"${BUNDLE_DIR}\" -no-strip RESULT_VARIABLE retval)
            if(NOT retval EQUAL 0)
                message(FATAL_ERROR \"macdeployqt failed\")
            endif()
        endif()
    " COMPONENT MACOS_BUNDLE)
      
    install(CODE "execute_process(
            COMMAND bash ${WEBENGINEINSTALL_CMD} ${Qt6_DIR} \"${BUNDLE_DIR}\"
        )"
        COMPONENT MACOS_BUNDLE
    )
    
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault MacOS bundle to ${MV_INSTALL_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MACOS_BUNDLE
            --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )

    # Make required non-Qt dylibs available in the build-tree app bundle for debugger runs
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:qtadvanceddocking-qt6>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/"
        # Keep one real dylib and add a SONAME alias so dyld can resolve @rpath/libqtadvanceddocking*.4.dylib. Make sure to remove any existing SONAME (symlink) first.
        COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/$<TARGET_SONAME_FILE_NAME:qtadvanceddocking-qt6>"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "$<TARGET_FILE_NAME:qtadvanceddocking-qt6>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/$<TARGET_SONAME_FILE_NAME:qtadvanceddocking-qt6>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${MV_PUBLIC_LIB}>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/"
        COMMENT "Copying bundle framework dependencies for build-tree debugging"
	)

    if(MV_USE_ERROR_LOGGING)
        add_custom_command(TARGET ${MV_EXE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:sentry>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:crashpad_wer>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:crashpad_handler>" "$<TARGET_FILE_DIR:${MV_EXE}>/../Frameworks/"
            COMMENT "Copying bundle framework dependencies for build-tree debugging (error logging)"
        )
    endif()

    
else() # Windows and Linux

    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION . COMPONENT MV_EXECUTABLE
    )

    if(MV_USE_ERROR_LOGGING)
        install(TARGETS ${MV_CRASH_REPORTER}
            RUNTIME DESTINATION . COMPONENT MV_EXECUTABLE
        )
    endif()

    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable to ${MV_INSTALL_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MV_EXECUTABLE
            --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )

    # Install dependency runtime libraries
    # on windows, the DLLs should be placed in the same directory as the executable
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency runtime libraries"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:qtadvanceddocking-qt6> ${MV_INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,,lib/>$<TARGET_FILE_NAME:qtadvanceddocking-qt6>
    )

    # Create a symlink, e.g. libqtadvanceddockingd.so.4 -> libqtadvanceddockingd.so.4.5.0
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_custom_command(TARGET ${MV_EXE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                $<TARGET_FILE_NAME:qtadvanceddocking-qt6>
                ${MV_INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_SONAME_FILE_NAME:qtadvanceddocking-qt6>
        )
    endif()

    if(MV_USE_ERROR_LOGGING)
        # Install sentry artifacts
        add_custom_command(TARGET ${MV_EXE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Installing: crashpad_handler executable"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:crashpad_handler> ${MV_INSTALL_DIR}/$<CONFIGURATION>/
        )
    endif()

endif()

# Install examples
add_custom_command(TARGET ${MV_EXE}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault examples"
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<VERSION_LESS:${CMAKE_VERSION},3.26.0>,copy_directory,copy_directory_if_different>  # copy_directory_if_different is only available starting 3.26
            ${MV_ROOT_DIR}/res/examples
            ${MV_INSTALL_DIR}/$<CONFIGURATION>/examples
)

# Install ManiVault licenses
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault licenses"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MV_ROOT_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/LICENSE
)

# Install dependency licenses
set(INSTALL_LICENSE_COMMANDS)
list(APPEND INSTALL_LICENSE_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency licenses: core"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/ads
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${advanced_docking_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/ads/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${quazip_SOURCE_DIR}/COPYING ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zlib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${zlib_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zlib/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/nlohmann_json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${nlohmann_json_SOURCE_DIR}/LICENSE.MIT ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/nlohmann_json/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/valijson
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${valijson_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/valijson/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/biovault_bfloat16
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${biovault_bfloat16_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/biovault_bfloat16/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zstd
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${zstd_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zstd/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/taskflow
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Taskflow_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/taskflow/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/msf_gif
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MV_ROOT_DIR}/external/msf_gif/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/msf_gif/LICENSE
)

if(MV_USE_ERROR_LOGGING)
    list(APPEND INSTALL_LICENSE_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency licenses: error logging"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/sentry
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${sentry-native_SOURCE_DIR}/LICENSE" ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/sentry/LICENSE
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/crashpad
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${sentry-native_SOURCE_DIR}/external/crashpad/LICENSE" ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/crashpad/LICENSE
    )
endif()

add_custom_command(TARGET ${MV_EXE} POST_BUILD ${INSTALL_LICENSE_COMMANDS})

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------

# Automatically set the debug environment (command + working directory) for MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin")
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${MV_INSTALL_DIR}/RelWithDebInfo,${MV_INSTALL_DIR}/Release>>)
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/Debug/${MV_APPLICATION_NAME}.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${MV_INSTALL_DIR}/RelWithDebInfo/${MV_APPLICATION_NAME}.exe","${MV_INSTALL_DIR}/Release/${MV_APPLICATION_NAME}.exe">>)
endif()
