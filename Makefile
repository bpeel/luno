PRJ=$(OO_SDK_HOME)
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/platform.mk

# Define non-platform/compiler specific settings
COMP_NAME=luno
COMP_IMPL_NAME=$(COMP_NAME).uno.$(SHAREDLIB_EXT)
COMP_RDB_NAME = $(COMP_NAME).uno.rdb
COMP_RDB = $(OUT_BIN)/$(COMP_RDB_NAME)
OUT_COMP_INC=$(OUT_INC)/$(COMP_NAME)
OUT_COMP_GEN=$(OUT_MISC)/$(COMP_NAME)
OUT_COMP_SLO=$(OUT_SLO)/$(COMP_NAME)
COMP_PACKAGE = $(OUT_BIN)/$(COMP_NAME).$(UNOOXT_EXT)
COMP_PACKAGE_URL = $(subst \\,\,"$(COMP_PACKAGE_DIR)$(PS)$(COMP_NAME).$(UNOOXT_EXT)")
COMP_UNOPKG_MANIFEST = $(OUT_COMP_GEN)/$(COMP_NAME)/META-INF/manifest.xml
COMP_UNOPKG_DESCRIPTION = $(OUT_COMP_GEN)/$(COMPONENT_NAME)/description.xml
COMP_COMPONENTS = $(OUT_COMP_GEN)/$(COMP_NAME).components
COMP_REGISTERFLAG = $(OUT_MISC)/cpp_$(COMP_NAME)_register_component.flag
COMP_TYPEFLAG = $(OUT_MISC)/cpp_$(COMP_NAME)_types.flag

CXXFILES = \
           protocolhandler.cxx \
           luno.cxx \
           object.cxx \
           method.cxx \
           conversions.cxx \
           struct.cxx \
           lookup.cxx \
           type.cxx \
           testhelper.cxx \
           exports.cxx

SLOFILES = $(patsubst %.cxx,$(OUT_COMP_SLO)/%.$(OBJ_EXT),$(CXXFILES))

UNIT_TESTER = $(OUT_BIN)/unittester

UNIT_TESTER_CXXFILES = \
           unittester.cxx

UNIT_TESTER_SLOFILES = \
    $(patsubst %.cxx,$(OUT_COMP_SLO)/%.$(OBJ_EXT),$(UNIT_TESTER_CXXFILES))

IDLFILES = \
           idl/uk/co/busydoingnothing/luno/LuaException.idl \
           idl/uk/co/busydoingnothing/luno/Runner.idl \
           idl/uk/co/busydoingnothing/luno/XRunner.idl \
           idl/uk/co/busydoingnothing/luno/qa/TestConstants.idl \
           idl/uk/co/busydoingnothing/luno/qa/TestHelper.idl \
           idl/uk/co/busydoingnothing/luno/qa/TestStruct.idl \
           idl/uk/co/busydoingnothing/luno/qa/XTestHelper.idl

DATA_FILES = \
           Addons.xcu \
           ProtocolHandler.xcu \
           pkg-description.txt \
           COPYING

# Targets
.PHONY: ALL
ALL : Luno

include $(SETTINGS)/stdtarget.mk

ifeq "$(OS)" "WIN"
LOCAL_CXX_FLAGS=/std:c++20 /utf-8
else
LOCAL_CXX_FLAGS=-std=c++20 $$(pkg-config lua --cflags)
endif

$(OUT_BIN)/%.rdb : $(IDLFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	$(UNOIDLWRITE) $(URE_TYPES) $(OFFICE_TYPES) idl $@

$(COMP_TYPEFLAG) : $(COMP_RDB) $(SDKTYPEFLAG)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_TYPEFLAG)))
	$(CPPUMAKER) -Gc -O$(OUT_COMP_INC) $(TYPESLIST) $(COMP_RDB) -X $(URE_TYPES) -X $(OFFICE_TYPES)
	echo flagged > $@

$(OUT_COMP_SLO)/%.$(OBJ_EXT) : %.cxx $(COMP_TYPEFLAG)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(CC) $(LOCAL_CXX_FLAGS) $(CC_FLAGS) $(CC_INCLUDES) -I$(OUT_COMP_INC) $(CC_DEFINES) $(CC_OUTPUT_SWITCH)$(subst /,$(PS),$@) $<

$(OUT_COMP_SLO)/protocolhandler.$(OBJ_EXT) : protocolhandler.hxx luno.hxx
$(OUT_COMP_SLO)/luno.$(OBJ_EXT) : luno.hxx object.hxx lookup.hxx runtime.hxx
$(OUT_COMP_SLO)/exports.$(OBJ_EXT) : protocolhandler.hxx
$(OUT_COMP_SLO)/object.$(OBJ_EXT) : object.hxx method.hxx runtime.hxx
$(OUT_COMP_SLO)/conversions.$(OBJ_EXT) : conversions.hxx runtime.hxx
$(OUT_COMP_SLO)/method.$(OBJ_EXT) : method.hxx
$(OUT_COMP_SLO)/struct.$(OBJ_EXT) : struct.hxx runtime.hxx
$(OUT_COMP_SLO)/lookup.$(OBJ_EXT) : lookup.hxx type.hxx runtime.hxx
$(OUT_COMP_SLO)/type.$(OBJ_EXT) : type.hxx runtime.hxx
$(OUT_COMP_SLO)/testhelper.$(OBJ_EXT) : testhelper.hxx

ifeq "$(OS)" "WIN"
$(SHAREDLIB_OUT)/%.$(SHAREDLIB_EXT) : $(SLOFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	-$(MKDIR) $(subst /,$(PS),$(OUT_COMP_GEN))
	$(LINK) $(COMP_LINK_FLAGS) /OUT:$@ \
	/MAP:$(OUT_COMP_GEN)/$(subst $(SHAREDLIB_EXT),map,$(@F)) $(SLOFILES) \
	$(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) msvcprt.lib $(LIBO_SDK_LDFLAGS_STDLIBS)
	$(LINK_MANIFEST)
else
$(SHAREDLIB_OUT)/%.$(SHAREDLIB_EXT) : $(SLOFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(LINK) $(COMP_LINK_FLAGS) $(LINK_LIBS) $$(pkg-config lua --libs) \
	-o $@ $(SLOFILES) \
	$(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB)
ifeq "$(OS)" "MACOSX"
	$(INSTALL_NAME_URELIBS)  $@
endif
endif

# rule for extension description.xml
$(COMP_UNOPKG_DESCRIPTION) :  description.xml
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_CAT) $< | $(SDK_SED) -e "s/#EXTENSION_PLATFORM#/$(EXTENSION_PLATFORM)/" > $@

# rule for component package manifest
$(OUT_COMP_GEN)/%/manifest.xml : manifest.xml.in
	-$(MKDIR) $(subst /,$(PS),$(@D))
	sed -e "s|@UNOPKG_PLATFORM@|$(UNOPKG_PLATFORM)|g" \
	-e "s|@COMP_NAME@|$(COMP_NAME)|g" \
	-e "s|@SO_PATH@|$(COMP_NAME)|g" \
	-e "s|@URI@|$(subst /META-INF,,$(subst $(OUT_COMP_GEN)/,,$(UNOPKG_PLATFORM)/$(@D))).uno.$(SHAREDLIB_EXT)|g" \
	< $< > $@

$(COMP_COMPONENTS) : luno.components.in
	-$(MKDIR) $(subst /,$(PS),$(@D))
	sed -e "s|@URI@|$(UNOPKG_PLATFORM)/$(COMP_IMPL_NAME)|g" < $< > $@

# rule for component package file
$(COMP_PACKAGE) : $(SHAREDLIB_OUT)/$(COMP_IMPL_NAME) $(DATA_FILES) $(COMP_UNOPKG_MANIFEST) $(COMP_COMPONENTS) $(COMP_UNOPKG_DESCRIPTION)
	-$(MKDIR) $(subst /,$(PS),$(@D)) && $(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(OUT_COMP_GEN)/$(UNOPKG_PLATFORM))
	$(COPY) $(subst /,$(PS),$<) $(subst /,$(PS),$(OUT_COMP_GEN)/$(UNOPKG_PLATFORM))
	$(CD) $(subst /,$(PS),$(OUT_COMP_GEN)) && $(SDK_ZIP) -u ../../bin/$(@F) $(COMP_NAME).components description.xml
	$(CD) $(subst /,$(PS),$(OUT_COMP_GEN)) && $(SDK_ZIP) -u ../../bin/$(@F) $(UNOPKG_PLATFORM)/$(<F)
	$(SDK_ZIP) -u $@ $(DATA_FILES)
	$(CD) $(subst /,$(PS),$(OUT_COMP_GEN)/$(subst .$(UNOOXT_EXT),,$(@F))) && $(SDK_ZIP) -u ../../../bin/$(@F) META-INF/manifest.xml

$(COMP_REGISTERFLAG) : $(COMP_PACKAGE)
ifeq "$(SDK_AUTO_DEPLOYMENT)" "YES"
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	$(DEPLOYTOOL) $(COMP_PACKAGE_URL)
	@echo flagged > $(subst /,$(PS),$@)
else
	@echo --------------------------------------------------------------------------------
	@echo  If you want to install your component automatically, please set the environment
	@echo  variable SDK_AUTO_DEPLOYMENT = YES. But note that auto deployment is only
	@echo  possible if no office instance is running.
	@echo --------------------------------------------------------------------------------
endif

Luno : $(COMP_REGISTERFLAG)
	@echo --------------------------------------------------------------------------------
	@echo The "$(QM)Luno$(QM)" addon component was installed if SDK_AUTO_DEPLOYMENT = YES.
	@echo You can use this component inside your office installation, see the example
	@echo description.
	@echo --------------------------------------------------------------------------------

run: $(COMP1_COMP_REGISTERFLAG)
	"$(OFFICE_PROGRAM_PATH)$(PS)soffice" --writer

.PHONY: clean
clean :
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_COMP_INC))
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_COMP_GEN))
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_COMP_SLO))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_PACKAGE_URL)))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_REGISTERFLAG)))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_TYPEFLAG)))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(SHAREDLIB_OUT)/$(COMP_NAME).*))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_RDB)))

$(UNIT_TESTER) : $(UNIT_TESTER_SLOFILES)
	$(LINK) $(EXE_LINK_FLAGS) $(LINK_LIBS) -o $@ $^ \
	$(CPPUHELPERLIB) $(CPPULIB) $(SALHELPERLIB) $(SALLIB) $(STDC++LIB)

.PHONY : check
check : $(UNIT_TESTER) $(COMP_RDB) $(COMP_COMPONENTS) $(COMP_PACKAGE)
	$(UNIT_TESTER) -env:URE_MORE_SERVICES=$(URLPREFIX)$(COMP_COMPONENTS) \
	-env:URE_MORE_TYPES=$(URLPREFIX)$(COMP_RDB) \
	unittests.lua
