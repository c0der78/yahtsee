SUBDIRS = arg3dice 

AM_CPPFLAGS = @AM_CPPFLAGS@ -Iarg3dice/src

bin_PROGRAMS = terminal-yahtzee

terminal_yahtzee_SOURCES = main.cpp

terminal_yahtzee_LDFLAGS = -lcaca arg3dice/arg3dice-0.2.la
