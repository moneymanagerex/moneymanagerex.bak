CXX = g++
CXXFLAGS ?= -g -Wall -W -Winline -ansi 
LDFLAGS ?= 
SED = sed
MV = mv
RM = rm

.SUFFIXES: .o .cpp

lib = libUnitTest++.a
test = TestUnitTest++

src = AssertException.cpp \
	Test.cpp \
	Checks.cpp \
	TestRunner.cpp \
	TestResults.cpp \
	TestReporter.cpp \
	TestReporterStdout.cpp \
	ReportAssert.cpp \
	TestList.cpp \
	TimeConstraint.cpp \
	TestDetails.cpp \
	MemoryOutStream.cpp \
	DeferredTestReporter.cpp \
	DeferredTestResult.cpp \
	XmlTestReporter.cpp \
	CurrentTest.cpp
	
ifeq ($(MSYSTEM), MINGW32)
  src += Win32/TimeHelpers.cpp
else
  src += Posix/SignalTranslator.cpp \
	Posix/TimeHelpers.cpp
endif

test_src = tests/Main.cpp \
	tests/TestAssertHandler.cpp \
	tests/TestChecks.cpp \
	tests/TestUnitTest++.cpp \
	tests/TestTest.cpp \
	tests/TestTestResults.cpp \
	tests/TestTestRunner.cpp \
	tests/TestCheckMacros.cpp \
	tests/TestTestList.cpp \
	tests/TestTestMacros.cpp \
	tests/TestTimeConstraint.cpp \
	tests/TestTimeConstraintMacro.cpp \
	tests/TestMemoryOutStream.cpp \
	tests/TestDeferredTestReporter.cpp \
	tests/TestXmlTestReporter.cpp \
	tests/TestCurrentTest.cpp

objects = $(patsubst %.cpp, %.o, $(src))
test_objects = $(patsubst %.cpp, %.o, $(test_src))
dependencies = $(subst .o,.d,$(objects))
test_dependencies = $(subst .o,.d,$(test_objects))

define make-depend
  $(CXX) $(CXXFLAGS) -M $1 | \
  $(SED) -e 's,\($(notdir $2)\) *:,$(dir $2)\1: ,' > $3.tmp
  $(SED) -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$//' \
      -e '/^$$/ d' \
      -e 's/$$/ :/' $3.tmp >> $3.tmp
  $(MV) $3.tmp $3
endef


all: $(test)


$(lib): $(objects) 
	@echo Creating $(lib) library...
	@ar cr $(lib) $(objects)
    
$(test): $(lib) $(test_objects)
	@echo Linking $(test)...
	@$(CXX) $(LDFLAGS) -o $(test) $(test_objects) $(lib)
	@echo Running unit tests...
	@./$(test)

clean:
	-@$(RM) $(objects) $(test_objects) $(dependencies) $(test_dependencies) $(test) $(lib) 2> /dev/null

%.o : %.cpp
	@echo $<
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CXX) $(CXXFLAGS) -c $< -o $(patsubst %.cpp, %.o, $<)


ifneq "$(MAKECMDGOALS)" "clean"
-include $(dependencies)
-include $(test_dependencies)
endif
