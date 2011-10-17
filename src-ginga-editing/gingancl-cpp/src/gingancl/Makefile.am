
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

# the library search path.
localLibdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingancl_so_LDFLAGS = -Wl,-soname,libgingancl.so -shared \
-L/usr/local/lib/ginga

localLib_PROGRAMS = libgingancl.so

libgingancl_so_SOURCES = model/time/CostFunctionDuration.cpp \
model/time/DurationFactory.cpp \
model/time/FlexibleTimeMeasurement.cpp \
model/time/LinearCostFunctionDuration.cpp \
model/time/TimeMeasurement.cpp \
model/time/UnflexibleDuration.cpp \
model/switches/ExecutionObjectSwitch.cpp \
model/switches/SwitchEvent.cpp \
model/presentation/focus/FocusSourceManager.cpp \
model/presentation/CascadingDescriptor.cpp \
model/presentation/FormatterDeviceRegion.cpp \
model/presentation/FormatterLayout.cpp \
model/presentation/FormatterRegion.cpp \
model/link/FormatterCausalLink.cpp model/link/FormatterLink.cpp \
model/link/LinkAction.cpp model/link/LinkAndCompoundTriggerCondition.cpp \
model/link/LinkAssessmentStatement.cpp model/link/LinkAssignmentAction.cpp \
model/link/LinkAttributeAssessment.cpp model/link/LinkCompoundAction.cpp \
model/link/LinkCompoundStatement.cpp \
model/link/LinkCompoundTriggerCondition.cpp model/link/LinkRepeatAction.cpp \
model/link/LinkSimpleAction.cpp \
model/link/LinkTransitionTriggerCondition.cpp \
model/link/LinkTriggerCondition.cpp model/link/LinkValueAssessment.cpp \
model/event/transition/BeginEventTransition.cpp \
model/event/transition/EndEventTransition.cpp \
model/event/transition/EventTransition.cpp \
model/event/transition/TransitionDispatcher.cpp \
model/event/AnchorEvent.cpp \
model/event/AttributionEvent.cpp model/event/FormatterEvent.cpp \
model/event/PresentationEvent.cpp model/event/SelectionEvent.cpp \
model/components/CompositeExecutionObject.cpp \
model/components/ExecutionObject.cpp model/components/NodeNesting.cpp \
model/components/ProceduralExecutionObject.cpp \
focus/FormatterFocusManager.cpp converter/FormatterConverter.cpp \
converter/FormatterLinkConverter.cpp adapters/text/LinksPlayerAdapter.cpp \
adapters/text/PlainTxtPlayerAdapter.cpp \
adapters/text/SubtitlePlayerAdapter.cpp \
adapters/procedural/lua/LuaPlayerAdapter.cpp \
adapters/procedural/ProceduralPlayerAdapter.cpp \
adapters/image/ImagePlayerAdapter.cpp \
adapters/av/tv/ProgramAVPlayerAdapter.cpp \
adapters/av/AVPlayerAdapter.cpp \
adapters/av/ChannelPlayerAdapter.cpp \
adapters/FormatterPlayerAdapter.cpp adapters/NominalEventMonitor.cpp \
adapters/PlayerAdapterManager.cpp \
adaptation/context/ContextBase.cpp \
adaptation/context/PresentationContext.cpp \
adaptation/context/RuleAdapter.cpp Formatter.cpp FormatterScheduler.cpp \
PresentationEngineManager.cpp

libgingancl_so_LDADD = -ltelemidiautil -ltelemidialinks -lgingaccio \
-lgingaccplayer -lncl30 -lncl30converter $(TUNER_LIBS) $(TSPARSER_LIBS) \
 $(DATAPROCESSING_LIBS) $(NCLET_LIBS)
