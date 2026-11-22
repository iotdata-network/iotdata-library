#
# IoT Sensor Telemetry Protocol
# Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
#
# Makefile - iotdata library, test and support Makefile
#
# Targets:
#   all           - Build library and both test suites
#   tests         - Build and run all tests
#   test-default  - Build and run default variant tests
#   test-custom   - Build and run custom variant tests
#   test-complete - Build and run comprehensive all-field-type tests
#   test-failures - Build and run failure oriented tests
#   test-example  - Build and run example default variant test
#   test-versions - Build and run all compile-time variant smoke tests
#   minimal       - Build and show full versus minimal build sizes (native)
#   minimal-esp32 - Build and show full versus minimal build sizes (esp32 cross)
#   lib           - Build static library
#   clean         - Remove build artifacts
#   format        - Run clang-format across the code (*.c/*.h)
#   prettier      - Run prettier across the documentation (*.md)
#
# Compile-time options (pass via EXTRA=):
#   IOTDATA_VARIANT_MAPS_DEFAULT   Default variant maps (weather station)
#   IOTDATA_VARIANT_MAPS <sym>     Custom variant maps array symbol
#   IOTDATA_VARIANT_MAPS_COUNT <n> Number of entries in custom maps
#   IOTDATA_ENABLE_SELECTIVE       Only compile explicitly enabled elements
#   IOTDATA_ENABLE_xxx             Enable individual field types
#   IOTDATA_ENABLE_TLV             Enable TLV
#   IOTDATA_NO_DECODE              Exclude decoder
#   IOTDATA_NO_ENCODE              Exclude encoder
#   IOTDATA_NO_PRINT               Exclude Print output support
#   IOTDATA_NO_DUMP                Exclude Dump output support
#   IOTDATA_NO_JSON                Exclude JSON support
#   IOTDATA_NO_TLV_SPECIFIC        Exclude TLV specific type handling
#   IOTDATA_NO_CHECKS_STATE        Remove runtime state checks
#   IOTDATA_NO_CHECKS_TYPES        Remove runtime type checks (e.g. temp limits)
#   IOTDATA_NO_ERROR_STRINGS       Exclude error strings (iotdata_strerror)
#   IOTDATA_NO_FLOATING_DOUBLES    Use float instead of double for position
#   IOTDATA_NO_FLOATING            Integer-only mode (no float/double)
#

CC=gcc
CFLAGS_DEFINES=
CFLAGS_COMMON=-Wall -Wextra -Wpedantic
CFLAGS_STRICT=-Werror \
    -Wstrict-prototypes \
    -Wold-style-definition \
    -Wcast-align -Wcast-qual -Wconversion \
    -Wfloat-equal -Wformat=2 -Wformat-security \
    -Winit-self -Wjump-misses-init \
    -Wlogical-op -Wmissing-include-dirs \
    -Wnested-externs -Wpointer-arith \
    -Wredundant-decls -Wshadow \
    -Wstrict-overflow=2 -Wswitch-default \
    -Wundef \
    -Wunreachable-code -Wunused \
    -Wwrite-strings
CFLAGS_OPT=-Os
# CFLAGS_OPT=-O6
CFLAGS  = $(CFLAGS_COMMON) $(CFLAGS_STRICT) $(CFLAGS_DEFINES) $(CFLAGS_OPT)
CFLAGS_NO_FLOATING_POINT=-mno-sse -mno-mmx -mno-80387
CFLAGS_VERSIONS=-DIOTDATA_VARIANT_MAPS=test_version_variants -DIOTDATA_VARIANT_MAPS_COUNT=1
CFLAGS_SELECTIVE=-DIOTDATA_ENABLE_SELECTIVE \
    -DIOTDATA_ENABLE_TLV \
    -DIOTDATA_ENABLE_BATTERY -DIOTDATA_ENABLE_LINK \
    -DIOTDATA_ENABLE_ENVIRONMENT -DIOTDATA_ENABLE_TEMPERATURE -DIOTDATA_ENABLE_PRESSURE -DIOTDATA_ENABLE_HUMIDITY \
    -DIOTDATA_ENABLE_WIND -DIOTDATA_ENABLE_WIND_SPEED -DIOTDATA_ENABLE_WIND_DIRECTION -DIOTDATA_ENABLE_WIND_GUST \
    -DIOTDATA_ENABLE_RAIN -DIOTDATA_ENABLE_RAIN_RATE -DIOTDATA_ENABLE_RAIN_SIZE \
    -DIOTDATA_ENABLE_SOLAR -DIOTDATA_ENABLE_CLOUDS \
    -DIOTDATA_ENABLE_AIR_QUALITY -DIOTDATA_ENABLE_AIR_QUALITY_INDEX -DIOTDATA_ENABLE_AIR_QUALITY_PM -DIOTDATA_ENABLE_AIR_QUALITY_GAS \
    -DIOTDATA_ENABLE_RADIATION -DIOTDATA_ENABLE_RADIATION_CPM -DIOTDATA_ENABLE_RADIATION_DOSE \
    -DIOTDATA_ENABLE_DEPTH -DIOTDATA_ENABLE_POSITION -DIOTDATA_ENABLE_DATETIME \
    -DIOTDATA_ENABLE_FLAGS -DIOTDATA_ENABLE_IMAGE
AR      = ar
LDFLAGS =
LIBS      = -lm -lcjson
LIBS_NOJSON = -lm
LIBS_NOJSON_NO_MATH =
LIBS_NO_MATH=-lcjson
EXTRA   ?= -DIOTDATA_VARIANT_MAPS_DEFAULT

CC_MACHINE := $(shell $(CC) -dumpmachine)
ifneq ($(findstring x86_64,$(CC_MACHINE)),)
    CFLAGS_NO_FLOATING_POINT = -mno-sse -mno-mmx -mno-80387
else ifneq ($(findstring i686,$(CC_MACHINE)),)
    CFLAGS_NO_FLOATING_POINT = -mno-sse -mno-mmx -mno-80387
else ifneq ($(findstring i386,$(CC_MACHINE)),)
    CFLAGS_NO_FLOATING_POINT = -mno-sse -mno-mmx -mno-80387
else
    CFLAGS_NO_FLOATING_POINT =
endif
CFLAGS_STACK_USAGE=-fstack-usage
CFLAGS_TEST=-I.
STACK_USAGE_FILE_LIST=tests/*.su
STACK_USAGE_FILE_EXAMPLE_IOTDATA=tests/test_example-iotdata.su

LIB_SRC    = iotdata.c
LIB_OBJ    = iotdata.o
LIB_HDR    = iotdata.h iotdata_node.h iotdata_mesh.h iotdata_fields.h iotdata_fields_tlv.h iotdata_fields_image.h
LIB_STATIC = libiotdata.a

TEST_DEFAULT_SRC = tests/test_default.c
TEST_DEFAULT_BIN = tests/test_default
TEST_CUSTOM_SRC  = tests/test_custom.c
TEST_CUSTOM_BIN  = tests/test_custom
TEST_COMPLETE_SRC = tests/test_complete.c
TEST_COMPLETE_BIN = tests/test_complete
TEST_EXAMPLE_SRC = tests/test_example.c
TEST_EXAMPLE_BIN = tests/test_example
TEST_FAILURES_SRC = tests/test_failures.c
TEST_FAILURES_BIN = tests/test_failures
TEST_ORDERED_SRC = tests/test_ordered.c
TEST_ORDERED_BIN = tests/test_ordered
TEST_FIELDS_SRC  = tests/test_fields.c
TEST_FIELDS_BIN  = tests/test_fields
TEST_VERSION_SRC = tests/test_version.c

MAPS_DEFAULT   = -DIOTDATA_VARIANT_MAPS_DEFAULT
MAPS_CUSTOM    = -DIOTDATA_VARIANT_MAPS=custom_variants -DIOTDATA_VARIANT_MAPS_COUNT=3
MAPS_COMPLETE  = -DIOTDATA_VARIANT_MAPS=complete_variants -DIOTDATA_VARIANT_MAPS_COUNT=3
MAPS_FAILURES  = -DIOTDATA_VARIANT_MAPS=failure_variants -DIOTDATA_VARIANT_MAPS_COUNT=3
MAPS_ORDERED   = -DIOTDATA_VARIANT_MAPS=ordered_variants -DIOTDATA_VARIANT_MAPS_COUNT=3
MAPS_FIELDS    = -DIOTDATA_VARIANT_MAPS=field_variants -DIOTDATA_VARIANT_MAPS_COUNT=3
CFLAGS_ORDERED = -DIOTDATA_OPERATE_ORDERED

SUITES_DEFERRED = $(TEST_DEFAULT_BIN) $(TEST_CUSTOM_BIN) $(TEST_COMPLETE_BIN) $(TEST_FAILURES_BIN) \
    $(TEST_FIELDS_BIN)
SUITES_ORDERED  = tests/test_default_ordered tests/test_custom_ordered \
    tests/test_complete_ordered tests/test_failures_ordered tests/test_fields_ordered \
    $(TEST_ORDERED_BIN)

MINIMAL_OBJ=iotdata_full.o iotdata_minimal.o

VERSION_BINS = \
    tests/test_version_FULL \
    tests/test_version_NO_PRINT \
    tests/test_version_NO_DUMP \
    tests/test_version_NO_JSON \
    tests/test_version_NO_DECODE \
    tests/test_version_NO_ENCODE \
    tests/test_version_NO_FLOATING \
    tests/test_version_NO_FLOATING_NO_JSON \
    tests/test_version_NO_ERROR_STRINGS \
    tests/test_version_NO_FLOATING_DOUBLES \
    tests/test_version_SELECTIVE \
    tests/test_version_NO_CHECKS \
    tests/test_version_NO_PRINT_NO_DUMP \
    tests/test_version_ORDERED \
    tests/test_version_ORDERED_COMPLETE

################################################################################

all: lib $(TEST_DEFAULT_BIN) $(TEST_CUSTOM_BIN) $(TEST_COMPLETE_BIN) $(TEST_FAILURES_BIN) $(TEST_FIELDS_BIN) $(TEST_ORDERED_BIN) $(TEST_EXAMPLE_BIN)

################################################################################

lib: $(LIB_STATIC)

$(LIB_OBJ): $(LIB_SRC) $(LIB_HDR)
	$(CC) $(CFLAGS) $(EXTRA) -c $(LIB_SRC) -o $(LIB_OBJ)
$(LIB_STATIC): $(LIB_OBJ)
	$(AR) rcs $(LIB_STATIC) $(LIB_OBJ)

################################################################################
# Generation
#
# specification/iotdata-fields.xml is the source; generators/templates hold every
# brace and guard; the generator knows no C and only pumps one into the other.
#
# generate-check is the acceptance test, and it is deliberately strict: generate,
# format, and the tree must be unchanged. The committed files ARE the test -- they
# have been in service, so reproducing them byte for byte is the only evidence
# that nothing was quietly re-derived. Formatting first means the generator is
# judged on content and not on its ability to pad columns.

GENERATOR      = generators/iotdata-generator-fields.js
# A wildcard, not a list: the point of several specification documents is that a
# vendor adds fields by dropping a file in, and a file nobody has listed here
# would otherwise not trigger a regeneration.
GENERATOR_SPEC = $(wildcard specification/iotdata-fields*.xml)
GENERATOR_TMPL = $(wildcard generators/templates/fields.*.tmpl)
GENERATOR_DEPS = $(GENERATOR) generators/iotdata-generator-functions-xml.js \
    $(GENERATOR_TMPL) $(GENERATOR_SPEC)

generate: $(GENERATOR_DEPS)
	node $(GENERATOR)
	$(MAKE) --no-print-directory format

# TWO QUESTIONS, BOTH WORTH ASKING, AND THEY ARE NOT THE SAME.
#
# FIDELITY: does the generated output match what is COMMITTED? That is the real
# acceptance test while fields are being migrated -- the committed files have been
# in service, so reproducing them is the only evidence nothing was quietly
# re-derived. It is `git diff`, and a non-empty diff here is for reading, not
# necessarily a fault: a deliberate change to a generated file shows up the same
# way as a mistake, and only a person can tell those apart.
#
# IDEMPOTENCE: does generating twice give the same thing? A generator that is not
# a fixed point is broken however good its first output looks, and this is the
# check that a snapshot comparison can make on its own. Testing only this would
# be hollow once the tree already holds generated output -- it would be comparing
# generated to generated -- which is why fidelity is asked first.
# What the generator writes. One header now, where it used to be a header and a
# source: see the banner in iotdata_fields.h.
GENERATED  = iotdata_fields.h

generate-check: $(GENERATOR_DEPS)
	@node $(GENERATOR) >/dev/null && $(MAKE) --no-print-directory format >/dev/null
	@d=$$(mktemp -d) && cp $(GENERATED) $$d/ && \
	node $(GENERATOR) >/dev/null && $(MAKE) --no-print-directory format >/dev/null && \
	if for f in $(GENERATED); do cmp -s $$f $$d/$$f || exit 1; done; then \
		rm -rf $$d; echo "generate-check: idempotent"; \
	else \
		echo "generate-check: NOT IDEMPOTENT -- generating twice gave different output"; \
		for f in $(GENERATED); do diff -u $$d/$$f $$f | head -40; done; rm -rf $$d; exit 1; \
	fi
	@if git diff --quiet -- $(GENERATED); then \
		echo "generate-check: matches the committed files byte for byte"; \
	else \
		echo "generate-check: DIFFERS FROM THE COMMITTED FILES -- read this before committing:"; \
		git --no-pager diff --stat -- $(GENERATED); \
		git --no-pager diff -- $(GENERATED) | head -60; \
		exit 1; \
	fi

################################################################################

$(TEST_DEFAULT_BIN): $(TEST_DEFAULT_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_DEFAULT) $(TEST_DEFAULT_SRC) $(LIB_SRC) $(LIBS) -o $@
$(TEST_CUSTOM_BIN): $(TEST_CUSTOM_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_CUSTOM) $(TEST_CUSTOM_SRC) $(LIB_SRC) $(LIBS) -o $@
$(TEST_COMPLETE_BIN): $(TEST_COMPLETE_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_COMPLETE) $(TEST_COMPLETE_SRC) $(LIB_SRC) $(LIBS) -o $@
$(TEST_FAILURES_BIN): $(TEST_FAILURES_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_FAILURES) $(TEST_FAILURES_SRC) $(LIB_SRC) $(LIBS) -o $@

# ... and the same four under IOTDATA_OPERATE_ORDERED
tests/test_default_ordered: $(TEST_DEFAULT_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_DEFAULT) $(CFLAGS_ORDERED) $(TEST_DEFAULT_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_custom_ordered: $(TEST_CUSTOM_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_CUSTOM) $(CFLAGS_ORDERED) $(TEST_CUSTOM_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_complete_ordered: $(TEST_COMPLETE_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_COMPLETE) $(CFLAGS_ORDERED) $(TEST_COMPLETE_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_failures_ordered: $(TEST_FAILURES_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_FAILURES) $(CFLAGS_ORDERED) $(TEST_FAILURES_SRC) $(LIB_SRC) $(LIBS) -o $@

# One test per generated field, asking the same four questions of each: round trip,
# boundaries, range, absence. The behavioural guard on the templates -- generate-check
# proves the output has not changed, this proves a change was right.
$(TEST_FIELDS_BIN): $(TEST_FIELDS_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_FIELDS) $(TEST_FIELDS_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_fields_ordered: $(TEST_FIELDS_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_FIELDS) $(CFLAGS_ORDERED) $(TEST_FIELDS_SRC) $(LIB_SRC) $(LIBS) -o $@
$(TEST_ORDERED_BIN): $(TEST_ORDERED_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(MAPS_ORDERED) $(CFLAGS_ORDERED) $(TEST_ORDERED_SRC) $(LIB_SRC) $(LIBS) -o $@

test-default: $(TEST_DEFAULT_BIN)
	./$(TEST_DEFAULT_BIN)
test-custom: $(TEST_CUSTOM_BIN)
	./$(TEST_CUSTOM_BIN)
test-complete: $(TEST_COMPLETE_BIN)
	./$(TEST_COMPLETE_BIN)
test-failures: $(TEST_FAILURES_BIN)
	./$(TEST_FAILURES_BIN)
test-ordered: $(TEST_ORDERED_BIN)
	./$(TEST_ORDERED_BIN)
test-fields: $(TEST_FIELDS_BIN)
	./$(TEST_FIELDS_BIN)

test-suites: $(SUITES_DEFERRED) $(SUITES_ORDERED)
	@for t in $(SUITES_DEFERRED) $(SUITES_ORDERED); do echo "### $$t"; ./$$t || exit 1; done

test-suites-deferred: $(SUITES_DEFERRED)
	@for t in $(SUITES_DEFERRED); do echo "### $$t"; ./$$t || exit 1; done

test-suites-ordered: $(SUITES_ORDERED)
	@for t in $(SUITES_ORDERED); do echo "### $$t"; ./$$t || exit 1; done

################################################################################

tests/test_version_FULL: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_PRINT: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_PRINT \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_DUMP: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_DUMP \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_JSON: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_JSON \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS_NOJSON) -o $@
tests/test_version_NO_DECODE: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_DECODE \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_ENCODE: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_ENCODE \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_FLOATING: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_FLOATING \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS_NO_MATH) -o $@
tests/test_version_NO_FLOATING_NO_JSON: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_FLOATING -DIOTDATA_NO_JSON $(CFLAGS_NO_FLOATING_POINT) \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS_NOJSON_NO_MATH) -o $@
tests/test_version_NO_ERROR_STRINGS: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_ERROR_STRINGS \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_FLOATING_DOUBLES: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_FLOATING_DOUBLES \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_SELECTIVE: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) $(CFLAGS_SELECTIVE) \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_ORDERED: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_OPERATE_ORDERED \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_ORDERED_COMPLETE: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_OPERATE_ORDERED -DIOTDATA_OPERATE_ORDERED_COMPLETE \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_PRINT_NO_DUMP: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_PRINT -DIOTDATA_NO_DUMP \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@
tests/test_version_NO_CHECKS: $(TEST_VERSION_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_VERSIONS) -DIOTDATA_NO_CHECKS_STATE -DIOTDATA_NO_CHECKS_TYPES \
		$(TEST_VERSION_SRC) $(LIB_SRC) $(LIBS) -o $@

test-versions: $(VERSION_BINS)
	@for t in $(VERSION_BINS); do ./$$t; done

################################################################################

$(TEST_EXAMPLE_BIN): $(TEST_EXAMPLE_SRC) $(LIB_HDR) $(LIB_SRC)
	$(CC) $(CFLAGS) $(CFLAGS_TEST) $(CFLAGS_STACK_USAGE) -DIOTDATA_VARIANT_MAPS_DEFAULT $(TEST_EXAMPLE_SRC) $(LIB_SRC) $(LIBS) -o $(TEST_EXAMPLE_BIN)

test-example: $(TEST_EXAMPLE_BIN)
	./$(TEST_EXAMPLE_BIN)

################################################################################

# EVERY FIELD ALONE, ACROSS THE BUILD AXES. IOTDATA_ENABLE_SELECTIVE compiles exactly one
# field's code, and that is the only configuration that can expose a helper or a table whose
# guard is wrong: in every other build some other field happens to use it, so the mistake stays
# silent. Two helpers in iotdata.c and one table in the air quality family were wrong this way,
# found only by sweeping it by hand -- hence this target.
#
# The field list derives itself from the GENERATED REGISTRY, so a field added tomorrow is swept
# without anyone remembering to add it here. The registry rather than iotdata_fields.h, because
# it is the whole-set view by construction: IMAGE's identifier lives in iotdata_fields_image.h,
# and reading only the generated header silently dropped it from the sweep.
SELECTIVE_FIELDS = $(shell awk -F, '/^0x/ { print toupper($$2) }' iotdata_fields.csv | sort -u)
SELECTIVE_AXES   = default NO_FLOATING NO_FLOATING_DOUBLES NO_JSON NO_PRINT+NO_DUMP \
                   NO_ENCODE NO_DECODE OPERATE_ORDERED NO_CHECKS_STATE+NO_CHECKS_TYPES

test-selective: $(LIB_HDR) $(LIB_SRC)
	@err=$$(mktemp); fail=0; n=0; \
	for ax in $(SELECTIVE_AXES); do \
		flags=""; \
		if [ "$$ax" != default ]; then \
			for one in $$(echo $$ax | tr '+' ' '); do flags="$$flags -DIOTDATA_$$one"; done; \
		fi; \
		for f in $(SELECTIVE_FIELDS); do \
			n=$$((n+1)); \
			if ! $(CC) $(CFLAGS) $(CFLAGS_VERSIONS) -DIOTDATA_ENABLE_SELECTIVE \
				-DIOTDATA_ENABLE_$$f $$flags -c $(LIB_SRC) -o /dev/null 2>$$err; then \
				echo "  FAIL $$f ($$ax)"; \
				sed -n 's/.*error: /        /p' $$err | head -1; \
				fail=$$((fail+1)); \
			fi; \
		done; \
	done; \
	rm -f $$err; \
	echo "test-selective: $$((n-fail))/$$n field x axis combinations clean"; \
	[ $$fail -eq 0 ]

################################################################################

# A 16-BIT TARGET, COMPILE ONLY. avr-gcc has `int` at sixteen bits, and that is the one property
# no amount of testing on x86 or an ESP32 reaches -- both have it at thirty-two. It found two real
# faults on its first run: the datetime helper held a day number in `int` (y * 365 is about
# 739000, and its seconds remainder reaches 86399), and ten print formats used %d for an
# iotdata_float_t, which is `int` on x86 and `long` here.
#
# WHAT IT CATCHES is what a compiler can see: a conversion that narrows, a format that does not
# match its argument, a type whose width the code assumed. It does NOT catch plain arithmetic
# overflow -- `int j = y * 365` draws no warning here on its own, and the datetime fault was
# visible only because a long literal in the same expression forced a narrowing conversion at the
# assignment. Catching the overflow itself would mean RUNNING these, which wants an emulator.
#
# Compile only, and no linking: there is no cJSON for an AVR, so the JSON-using suites cannot
# build and are not asked to. What IS built is the library across the whole configuration matrix,
# plus the three suites that need no cJSON -- which between them instantiate every field.
#
# Nothing here runs. Running it would want an emulator, and that is a separate question.
AVR_CC    ?= avr-gcc
AVR_MCU   ?= atmega328p
AVR_FLAGS  = -mmcu=$(AVR_MCU) -I. $(CFLAGS_OPT) $(CFLAGS_COMMON) $(CFLAGS_STRICT) -DIOTDATA_NO_JSON
AVR_BASES  = none OPERATE_ORDERED OPERATE_ORDERED+OPERATE_ORDERED_COMPLETE
AVR_AXES   = none NO_ENCODE NO_DECODE NO_FLOATING NO_FLOATING_DOUBLES NO_PRINT+NO_DUMP \
             NO_CHECKS_STATE+NO_CHECKS_TYPES

test-16bit:
	@command -v $(AVR_CC) >/dev/null || { echo "test-16bit: $(AVR_CC) not installed -- skipping"; exit 0; }; \
	fail=0; n=0; \
	for base in $(AVR_BASES); do \
		bf=""; [ "$$base" = none ] || for one in $$(echo $$base | tr '+' ' '); do bf="$$bf -DIOTDATA_$$one"; done; \
		for ax in $(AVR_AXES); do \
			af=""; [ "$$ax" = none ] || for one in $$(echo $$ax | tr '+' ' '); do af="$$af -DIOTDATA_$$one"; done; \
			n=$$((n+1)); \
			$(AVR_CC) $(AVR_FLAGS) -DIOTDATA_VARIANT_MAPS_DEFAULT $$bf $$af -c $(LIB_SRC) -o /dev/null || { \
				echo "  FAIL library $$base + $$ax"; fail=$$((fail+1)); }; \
		done; \
	done; \
	for one in "$(TEST_VERSION_SRC) $(CFLAGS_VERSIONS)" "$(TEST_FIELDS_SRC) $(MAPS_FIELDS)" \
	           "$(TEST_ORDERED_SRC) $(MAPS_ORDERED) -DIOTDATA_OPERATE_ORDERED"; do \
		n=$$((n+1)); \
		$(AVR_CC) $(AVR_FLAGS) -I tests -c $$one -o /dev/null || { \
			echo "  FAIL $$one"; fail=$$((fail+1)); }; \
	done; \
	echo "test-16bit: $$((n-fail))/$$n compile clean for $(AVR_MCU) (int is 16 bits there)"; \
	[ $$fail -eq 0 ]

################################################################################

tests: test-suites test-versions test-example test-selective test-16bit

################################################################################

format:
	clang-format-22 --verbose -i $$(find . -name build -prune -o \( -name '*.c' -o -name '*.h' \) -print)

prettier:
	prettier --write $$(find . -name build -prune -o \( -name '*.md' \) -print)

clean:
	rm -f $(LIB_OBJ) $(LIB_STATIC) $(TEST_DEFAULT_BIN) $(TEST_CUSTOM_BIN) $(TEST_COMPLETE_BIN) $(TEST_FAILURES_BIN) $(TEST_EXAMPLE_BIN) $(VERSION_BINS) $(MINIMAL_OBJ) $(STACK_USAGE_FILE_LIST)

.PHONY: all test-default test-custom test-complete test-failures test-suites test-example test-versions test-selective test-16bit tests lib format clean minimal

################################################################################

stack-usage:
	@cat $(STACK_USAGE_FILE_EXAMPLE_IOTDATA) | sort -t"$$(printf '\t')" -k2 -rn | head -10 | column -t

################################################################################

minimal:
	@echo "--- Complete library (platform) ---"
	$(CC) $(CFLAGS) -DIOTDATA_VARIANT_MAPS_DEFAULT -c $(LIB_SRC) -o iotdata_platform_full.o
	@size iotdata_platform_full.o
	@echo "--- Minimal encoder (platform) (battery + environment, integer-only) ---"
	$(CC) $(CFLAGS) $(CFLAGS_NO_FLOATING_POINT) \
		-DIOTDATA_NO_DECODE \
		-DIOTDATA_ENABLE_SELECTIVE -DIOTDATA_ENABLE_BATTERY -DIOTDATA_ENABLE_ENVIRONMENT \
		-DIOTDATA_NO_JSON -DIOTDATA_NO_DUMP -DIOTDATA_NO_PRINT \
		-DIOTDATA_NO_FLOATING -DIOTDATA_NO_ERROR_STRINGS -DIOTDATA_NO_CHECKS_STATE -DIOTDATA_NO_CHECKS_TYPES \
		-c $(LIB_SRC) -o iotdata_platform_minimal.o
	@echo "Minimal object size:"
	@size iotdata_platform_minimal.o
	@objdump -t iotdata_platform_minimal.o | grep ' \.data\| \.rodata' | sort -k5 -rn
	@rm -f iotdata_platform_full.o iotdata_platform_minimal.o

################################################################################

ESP_CC = riscv32-esp-elf-gcc
ESP_CFLAGS_BASE = -march=rv32imc -mabi=ilp32 -Os

minimal-esp32:
	@echo "--- Complete library (esp32c3) (no JSON) ---"
	$(ESP_CC) $(ESP_CFLAGS_BASE) -DIOTDATA_NO_JSON -c $(LIB_SRC) -o iotdata_esp32c3_full.o
	@riscv32-esp-elf-size iotdata_esp32c3_full.o
	@echo "--- Minimal encoder (esp32c3) (battery + environment, integer-only) ---"
	$(ESP_CC) $(ESP_CFLAGS_BASE) \
		-DIOTDATA_NO_DECODE \
		-DIOTDATA_ENABLE_SELECTIVE -DIOTDATA_ENABLE_BATTERY -DIOTDATA_ENABLE_ENVIRONMENT \
		-DIOTDATA_NO_JSON -DIOTDATA_NO_DUMP -DIOTDATA_NO_PRINT \
		-DIOTDATA_NO_FLOATING -DIOTDATA_NO_ERROR_STRINGS -DIOTDATA_NO_CHECKS_STATE -DIOTDATA_NO_CHECKS_TYPES \
		-c $(LIB_SRC) -o iotdata_esp32c3_minimal.o
	@echo "Minimal object size:"
	@riscv32-esp-elf-size iotdata_esp32c3_minimal.o
	@riscv32-esp-elf-objdump -t iotdata_esp32c3_minimal.o | grep ' \.data\| \.rodata' | sort -k5 -rn
	@rm -f iotdata_esp32c3_full.o iotdata_esp32c3_minimal.o

################################################################################

