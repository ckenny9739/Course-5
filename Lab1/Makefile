CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused -Werror
LAB = 1
DISTDIR = lab1-connorkenny

all: simpsh

TESTS = $(wildcard test*.sh)
TEST_BASES = $(subst .sh,,$(TESTS))

SIMPSH_SOURCES = main.c

SIMPSH_OBJECTS = $(subst .c,.o,$(SIMPSH_SOURCES))

DIST_SOURCES = \
  $(SIMPSH_SOURCES)  Makefile \
  $(TESTS) check-dist README setup.sh benchmarks.sh

simpsh: $(SIMPSH_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SIMPSH_OBJECTS)

dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES) check-dist
	rm -fr $(DISTDIR)
	tar -czf $@.tmp --transform='s,^,$(DISTDIR)/,' $(DIST_SOURCES)
	./check-dist $(DISTDIR)
	mv $@.tmp $@

check: $(TEST_BASES)

$(TEST_BASES): simpsh
	chmod +x setup.sh
	./setup.sh
	chmod +x benchmarks.sh
	./benchmarks.sh
	chmod +x test.sh
	./test.sh

clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp *.txt tmp simpsh $(DISTDIR)

.PHONY: all dist check $(TEST_BASES) clean
