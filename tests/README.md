# iotdata tests

Test suite for the iotdata encoder, decoder, and supporting functions. All tests
use a lightweight framework defined in `test_common.h` with `PASS`/`FAIL` output
and process exit code 0 on success, 1 on any failure.

Not everything that tests this library lives in this directory. The behavioural
suites are here; three further gates live in the Makefile alone, because what
they ask — does this build at all, does the generator still reproduce its output
— has no program to run. They are listed under
[Gates](#gates-no-programs-to-run) below, and they catch a class of fault no
passing suite can see.

## Running

```
make test-suites     # the eleven behavioural suites -- 438 checks
make test-versions   # the fifteen compile-time configuration builds
make test-selective  # 306 single-field compiles (needs no runner)
make test-16bit      # 24 avr-gcc compiles, where int is 16 bits
make generate-check  # the generated field code is reproducible
```

Individually: `make test-default`, `test-custom`, `test-complete`,
`test-failures`, `test-fields`, `test-ordered`, `test-example`.

`make tests` chains
`test-suites test-versions test-example test-selective test-16bit`. Note that
`test-example` is the interactive simulator and runs until Ctrl-C, so that chain
does not terminate on its own; the list above is the usable sequence.

## Test programs

### test_default

Exercises the built-in default variant map (weather station, variant 0). Covers
all 12 fields of that variant with individual round-trip tests, presence byte
grouping (pres0-only vs pres0+pres1), full-station encoding, boundary values
(min/max for every field), quantisation accuracy sweeps (temperature, wind,
position, radiation dose), TLV raw and string round-trips, JSON
binary→JSON→binary round-trips, dump and print output verification, error
conditions (out-of-range values, duplicate fields, invalid variant/station IDs),
and edge cases (empty packets, single pres1 field, packet size reporting).

### test_complete

Defines two custom variants to exercise field types not present in the default
map: `complete` (3 presence bytes, 16 fields including air quality PM/gas
channels, depth, and image) and `standalone` (3 presence bytes, 15 individual
sub-field types). Tests air quality PM partial channel masks, gas slot
boundaries, image format/size/compression/flag combinations, full-variant
encoding with all fields populated, TLV typed helpers (version, status, health,
config, diagnostic, userdata), multiple TLVs in a single packet, JSON
round-trips for both variants including TLV preservation, dump/print output, and
image RLE and heatshrink compress/decompress round-trips.

### test_custom

Defines three custom variants to verify the custom variant map mechanism:
`soil_sensor` (1 presence byte, standalone temperature and humidity),
`wind_mast` (1 presence byte, individual wind speed/direction/gust), and
`radiation_monitor` (2 presence bytes, 11 fields across pres0+pres1). Tests that
custom field ordering works, fields decode to the correct positions, partial
field encoding omits absent fields, JSON output uses custom field labels (e.g.
`soil_temp`, `soil_moist`), JSON round-trips produce identical wire bytes, print
output shows custom variant names, `iotdata_get_variant()` returns correct
definitions, and empty packets work for all variants.

### test_failures

Negative testing and error path coverage using the same two-variant layout as
test_complete. Covers negative value round-trips (temperature, RSSI, SNR,
position), boundary values for every field type, out-of-range error codes for
every encoder function, encoder state errors (null context, null buffer, encode
before begin, encode after end, duplicate fields, invalid variants, buffer too
small), decoder error paths (null inputs, zero length, truncated packets,
reserved variants), image edge cases (1-byte and 254-byte payloads, all pixel
formats), TLV edge cases (max entries, max data length, empty strings, type
boundaries, KV mismatch, invalid 6-bit characters), buffer overflow scenarios,
JSON parse errors and missing fields, dump/print with short or empty inputs, and
full-packet round-trips at all-minimum and all-maximum values.

### test_fields

One test per field, and the only suite that can answer "is this field tested"
without reading all the others. The four suites above reach for whatever field
is convenient to make a point about presence bytes, variant maps, slot order or
JSON; useful, and it left BITS32 never encoded anywhere at all. This suite asks
the same four questions of every field — round trip within its quantisation
cost, both boundaries, every `<check>` in its specifier actually firing, and
absence reading back as absent rather than as whatever the last packet left
behind — and a field joins it when it moves into
`specifiers/iotdata-fields-*.xml`.

Thirty-four fields, one test each, plus two that cut across: that a split
family's parts and its composite keep separate storage, and every field encoded
at once. Three variants are needed to hold them, because 27 slots
(`IOTDATA_MAX_DATA_FIELDS`, the protocol's ceiling) is fewer than there are
fields: `fields` (4 presence bytes, 27 fields), `image` (IMAGE alone), and
`parts` (the six standalone halves of the three families that used to be bundles
— battery level/charge, link RSSI/SNR, solar irradiance/ultraviolet).

It is also the behavioural guard on the generator templates.
`make generate-check` proves the generated files have not _changed_; it cannot
say whether a change is right, and it fails by design the moment someone means
to edit a template. These tests are what then say whether the new output still
behaves: a template that compiles and produces subtly wrong arithmetic is caught
here and nowhere else.

### test_ordered

Covers `IOTDATA_OPERATE_ORDERED` — what only that mode can do and what only it
refuses. In-order operation carries no intermediate field storage: a setter
packs straight into the buffer, an accessor unpacks straight out of it, and the
cursor only ever moves forwards. That buys duplicate fields (three temperature
slots, and the second call means the second slot) and costs the freedom to
encode or read in any order. Three variants (`duplicated`, `wide`, `simple`)
across seven sections: that an ordered packet is byte-for-byte what the deferred
mode writes, duplicate slots, the by-slot setters, skipping, reading out of
order, what the mode gives up (truncation surfacing at the accessor rather than
at decode), and the generic walkers. Unlike the suites above it exists only in
the ordered build; the field semantics both modes share are covered by those.

### test_version

Build-variant smoke test. A single source file compiled fifteen times with
different `#define` combinations to verify that every compile-time configuration
compiles, links, and runs correctly. Each build encodes a packet (or uses a
prebuilt one for `NO_ENCODE`), then exercises whichever API surface is
available. See [the configuration axis](#compile-time-configuration) below.

### test_example

Not a test — a live weather station simulator that runs until Ctrl-C. Generates
random-walk sensor data using the default variant, encodes a packet every 30
seconds (with position/datetime added every 5 minutes), and displays the raw
values, hex dump, diagnostic dump, decoded output, and JSON for each packet.
Useful for visually inspecting encoder output and verifying the full
encode→decode→print→JSON pipeline interactively. It is also built with
`-fstack-usage`, which is what `make stack-usage` reports on.

## Axes

A suite is a program, but a program is only one point in a configuration space,
and most of what has gone wrong in this library went wrong at a point no suite
was standing on. These are the axes the suites are swept along.

### Operating mode

Every suite above except `test_ordered` is built and run twice, once deferred
and once with `-DIOTDATA_OPERATE_ORDERED`, which is why there are eleven
binaries for six sources:

| Suite           | deferred | ordered |
| --------------- | -------- | ------- |
| `test_default`  | 30       | 30      |
| `test_custom`   | 14       | 14      |
| `test_complete` | 46       | 46      |
| `test_failures` | 81       | 81      |
| `test_fields`   | 36       | 36      |
| `test_ordered`  | —        | 24      |

438 checks in total. `make test-suites-deferred` and `make test-suites-ordered`
run one column each.

Writing a suite that passes in both modes costs something: ordered mode's cursor
only moves forwards, so the tests have to encode in slot order, and that
constrains how a variant's slots are laid out. `test_fields` orders its variant
to match the order its tests run in for exactly this reason.

### Compile-time configuration

`test_version` is one source compiled fifteen ways. The Makefile builds and runs
all of them under `make test-versions`.

| Build                 | Configuration                               |
| --------------------- | ------------------------------------------- |
| `FULL`                | All features enabled                        |
| `NO_PRINT`            | Exclude print functions                     |
| `NO_DUMP`             | Exclude dump functions                      |
| `NO_PRINT_NO_DUMP`    | Exclude both                                |
| `NO_JSON`             | Exclude JSON support (no cJSON dependency)  |
| `NO_DECODE`           | Encoder only                                |
| `NO_ENCODE`           | Decoder only                                |
| `NO_FLOATING`         | Integer-only mode (`int32_t` scaled values) |
| `NO_FLOATING_NO_JSON` | Integer-only + no JSON                      |
| `NO_FLOATING_DOUBLES` | `float` instead of `double` for position    |
| `NO_ERROR_STRINGS`    | Exclude `iotdata_strerror`                  |
| `NO_CHECKS`           | No runtime state or type checks             |
| `SELECTIVE`           | All types via `IOTDATA_ENABLE_SELECTIVE`    |
| `ORDERED`             | `IOTDATA_OPERATE_ORDERED`                   |
| `ORDERED_COMPLETE`    | Ordered, plus `OPERATE_ORDERED_COMPLETE`    |

## Gates (no programs to run)

These three live in the Makefile and have no source file in this directory. Each
one tests something a running suite structurally cannot.

### test-selective

Compiles the library once for every field on its own, across nine configuration
axes: 34 × 9 = **306 compiles**, no linking and no running.

`IOTDATA_ENABLE_SELECTIVE` with a single `IOTDATA_ENABLE_<FIELD>` is the build a
real sensor ships — one field, nothing else — and it is the build every `#if`
guard in the generated code is wrong about first. Eleven passing suites and
fifteen passing configurations can all be green while a field is unbuildable
alone, because every suite enables everything. This gate found three distinct
guard faults that nothing else saw, and one of them was in code that had been in
service.

The field list is not maintained here: it is read out of `iotdata_fields.csv`,
which the generator writes, so the gate grows by itself when a field is added to
a specifier. The axes are `default`, `NO_FLOATING`, `NO_FLOATING_DOUBLES`,
`NO_JSON`, `NO_PRINT`+`NO_DUMP`, `NO_ENCODE`, `NO_DECODE`, `OPERATE_ORDERED`,
and `NO_CHECKS_STATE`+`NO_CHECKS_TYPES`.

### test-16bit

Compiles the library and three of the test sources with `avr-gcc` for an
atmega328p — 3 operating-mode bases × 7 axes for the library, plus
`test_version`, `test_fields` and `test_ordered`: **24 compiles**.
`IOTDATA_NO_JSON` throughout, there being no cJSON there.

Nothing is run. The point is that `int` is 16 bits on that target and 32 on the
build host, so arithmetic that silently fits at home overflows in the field, and
a `%d` that matches `iotdata_float_t` on x86 does not when the typedef widens to
`long`. Its first run found two real portability faults in code that had been in
service: a datetime helper doing day-of-year arithmetic that overflowed at
`y * 365`, and ten print formats using `%d` where the width is
platform-dependent (now `PRId32` and the other `PRI` forms).

It skips with a message rather than failing if `avr-gcc` is not installed.
Nothing executes these binaries yet; qemu would, and has not been tried.

Override the target with `make test-16bit AVR_MCU=...`.

### generate-check

The acceptance test on the field generator, and the only thing standing between
a hand edit of a generated file and a silent divergence from its specifier. Two
questions:

- **idempotence** — generate twice, and the output must be identical. A
  generator that is not a fixed point is broken however good its first output
  looks.
- **fidelity** — the output must match what is _committed_, byte for byte. The
  committed files have been in service, so reproducing them is the only evidence
  that nothing was quietly re-derived.

Fidelity is a `git diff`, and a non-empty diff is for reading rather than
necessarily a fault: a deliberate change to a template shows up exactly like a
mistake, and only a person can tell those apart. When it does fail by design,
`test_fields` is what says whether the new output still behaves.

## Other Makefile targets

Not tests, but the same `make` and worth knowing:

| Target          | What it does                                        |
| --------------- | --------------------------------------------------- |
| `all`           | The library plus every suite binary                 |
| `lib`           | `libiotdata.a` alone                                |
| `generate`      | Run the field generator, then `format`              |
| `minimal`       | Full vs minimal object sizes, native                |
| `minimal-esp32` | The same, cross-compiled for esp32c3 (riscv32)      |
| `stack-usage`   | The ten deepest frames, from `test_example`'s `.su` |
| `format`        | `clang-format` across every `*.c` / `*.h`           |
| `prettier`      | `prettier` across every `*.md`                      |
| `clean`         | Remove build artifacts                              |

`minimal` is a size check rather than a correctness one: it builds the library
whole, then builds it as an encoder for battery and environment only with every
optional feature off, and prints both so the cost of the configuration space is
visible. `minimal-esp32` does it for the target that actually ships.

## Shared framework

`test_common.h` provides the test macros (`TEST`, `PASS`, `FAIL`, `ASSERT_EQ`,
`ASSERT_NEAR`, `ASSERT_OK`, `ASSERT_ERR`, `ASSERT_TRUE`) and shared
encoder/decoder state. Each test function calls `begin()` to start encoding,
`finish()` to finalise, and `decode_pkt()` to decode the result into a shared
`iotdata_decoded_t`. Tests return early on first assertion failure within each
test function.

Every variant map a suite defines writes `IOTDATA_FIELD_NONE` into all its
trailing slots explicitly. A partly written initialiser leaves the rest zero,
and zero is a real field type — `NONE` is -1.
