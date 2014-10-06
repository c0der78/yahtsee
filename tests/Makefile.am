noinst_PROGRAMS = test_terminal_yahtzee

test_terminal_yahtzee_SOURCES = main.test.cpp

AM_CPPFLAGS = @AM_CPPFLAGS@ -Ivendor/bandit

check: test_terminal_yahtzee$(EXEEXT)
	./test_terminal_yahtzee$(EXEEXT)
