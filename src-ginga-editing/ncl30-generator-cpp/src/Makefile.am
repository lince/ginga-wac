# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
        $(all_includes)


LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libncl30generator_so_LDFLAGS = -Wl,-soname,libncl30generator.so -shared \
-L/usr/local/lib -L/usr/local/lib/ginga
localLib_PROGRAMS = libncl30generator.so

libncl30generator_so_SOURCES = DescriptorBaseGenerator.cpp DescriptorGenerator.cpp BindGenerator.cpp \
		Generable.cpp PropertyAnchorGenerator.cpp IntervalAnchorGenerator.cpp \
		AnchorGenerator.cpp LayoutRegionGenerator.cpp AssessmentStatementGenerator.cpp \
		NclDocumentGenerator.cpp AttributeAssessmentGenerator.cpp NclGenerator.cpp \
		CausalConnectorGenerator.cpp ParameterGenerator.cpp CausalLinkGenerator.cpp \
		PortGenerator.cpp CircleSpatialAnchorGenerator.cpp RectangleSpatialAnchorGenerator.cpp \
		CompoundActionGenerator.cpp RegionBaseGenerator.cpp CompoundConditionGenerator.cpp \
		CompoundStatementGenerator.cpp SimpleActionGenerator.cpp \
		ConnectorBaseGenerator.cpp SimpleConditionGenerator.cpp ContentNodeGenerator.cpp \
		TextAnchorGenerator.cpp ContextNodeGenerator.cpp ValueAssessmentGenerator.cpp NclFileGenerator.cpp \
		DescriptorSwitchGenerator.cpp TransitionBaseGenerator.cpp TransitionGenerator.cpp GeneratorUtil.cpp \
		RuleBaseGenerator.cpp SimpleRuleGenerator.cpp CompositeRuleGenerator.cpp \
		SwitchNodeGenerator.cpp SwitchPortGenerator.cpp LabeledAnchorGenerator.cpp


libncl30generator_so_LDADD = -ltelemidiautil -lncl30 -llinceutil

