SUBDIRS = . tests


lib_LTLIBRARIES = libUnitTest++.la


# -version-number major[:minor[:revision]]
libUnitTest___la_LDFLAGS = -version-number 1:4

# -version-info current:revision:age
#libUnitTest___la_LDFLAGS = -version-info C:R:A


libUnitTest___ladir = $(includedir)/UnitTest++

libUnitTest___la_HEADERS = \
	AssertException.h \
	CheckMacros.h \
	Checks.h \
	Config.h \
	CurrentTest.h \
	DeferredTestReporter.h \
	DeferredTestResult.h \
	ExecuteTest.h \
	MemoryOutStream.h \
	ReportAssert.h \
	Test.h \
	TestDetails.h \
	TestList.h \
	TestMacros.h \
	TestReporter.h \
	TestReporterStdout.h \
	TestResults.h \
	TestRunner.h \
	TestSuite.h \
	TimeConstraint.h \
	TimeHelpers.h \
	UnitTest++.h \
	XmlTestReporter.h


libUnitTest___la_Posixdir = $(libUnitTest___ladir)/Posix

libUnitTest___la_Posix_HEADERS = \
	Posix/SignalTranslator.h \
	Posix/TimeHelpers.h


libUnitTest___la_SOURCES = \
	AssertException.cpp \
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
	CurrentTest.cpp \
	Posix/SignalTranslator.cpp  \
	Posix/SignalTranslator.h \
	Posix/TimeHelpers.cpp \
	Posix/TimeHelpers.h
