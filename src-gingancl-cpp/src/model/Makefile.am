
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclmodeldir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libginganclmodel_so_LDFLAGS = -Wl,-soname,libginganclmodel.so -shared \
    -L/usr/local/lib -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players

libginganclmodel_PROGRAMS = libginganclmodel.so

libginganclmodel_so_SOURCES = time/CostFunctionDuration.cpp \
    time/DurationFactory.cpp \
    time/FlexibleTimeMeasurement.cpp \
    time/LinearCostFunctionDuration.cpp \
    time/TimeMeasurement.cpp \
    time/UnflexibleDuration.cpp \
    switches/ExecutionObjectSwitch.cpp \
    switches/SwitchEvent.cpp \
    presentation/focus/FocusSourceManager.cpp \
    presentation/CascadingDescriptor.cpp \
    presentation/FormatterDeviceRegion.cpp \
    presentation/FormatterLayout.cpp \
    presentation/FormatterRegion.cpp \
    link/FormatterCausalLink.cpp link/FormatterLink.cpp \
    link/LinkAction.cpp link/LinkAndCompoundTriggerCondition.cpp \
    link/LinkAssessmentStatement.cpp link/LinkAssignmentAction.cpp \
    link/LinkAttributeAssessment.cpp link/LinkCompoundAction.cpp \
    link/LinkCompoundStatement.cpp \
    link/LinkCompoundTriggerCondition.cpp link/LinkRepeatAction.cpp \
    link/LinkSimpleAction.cpp \
    link/LinkTransitionTriggerCondition.cpp \
    link/LinkTriggerCondition.cpp link/LinkValueAssessment.cpp \
    event/transition/BeginEventTransition.cpp \
    event/transition/EndEventTransition.cpp \
    event/transition/EventTransition.cpp \
    event/transition/EventTransitionManager.cpp \
    event/transition/TransitionDispatcher.cpp \
    event/AnchorEvent.cpp \
    event/AttributionEvent.cpp event/FormatterEvent.cpp \
    event/PresentationEvent.cpp event/SelectionEvent.cpp \
    components/CompositeExecutionObject.cpp \
    components/ExecutionObject.cpp \
    components/NodeNesting.cpp \
    components/ApplicationExecutionObject.cpp

libginganclmodel_so_LDADD = -ltelemidiautil -lpthread -lgingaccsystemthread \
   -lncl30 -lgingaccsystemiocodemap
