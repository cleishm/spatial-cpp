# spatial

A type-safe, header-only C++20 library for spatial units, modeled after `std::chrono::duration`.

📚 **[Full API Documentation](https://cleishm.github.io/spatial-cpp/)**

## Features

- **Type-safe lengths, areas and volumes** with distinct types for different precisions
- **Metric units**: nanometers through kilometers; square millimeters through square kilometers and hectares; microliters, milliliters, liters and cubic meters
- **Imperial and nautical units**: mils, inches, feet, yards, miles, nautical miles; square inches through square miles and acres; cubic inches, feet and yards and US gallons — all defined by exact ratios of the meter (1 in = 25.4 mm), so metric and imperial values mix without drift
- **Dimensional arithmetic**: length × length = area, length × area = volume, and the corresponding quotients, with exact result precisions (`3_ft * 4_ft` is exactly `square_feet(12)`)
- **Integer and floating-point representations** for exact or fractional precision
- **Lossless implicit conversions** (lossy conversions require explicit casts)
- **User-defined literals** for concise notation (`250_mm`, `8_mil`, `6_ft`, `12_nmi`, `40_ac`, `250_mL`)
- **`std::format` support** (when available), with opt-in power-form (`24cm³`), base-unit (`0.000024m³`) and scaled (`1.5L`) display
- **Duration-like interface** for lengths (`count()`, `rep`, `period`), so lengths plug into generic code such as [frequency](https://github.com/cleishm/frequency-cpp)'s `wavelength()`
- **Fully constexpr** - all operations can be evaluated at compile time

## Requirements

- C++20 compiler (GCC 12+, Clang 15+, MSVC 19.30+)
- **MSVC users:** The `/utf-8` flag is required (the source contains the unit symbols µ, ² and ³)
- **GCC < 14**, which includes the ESP-IDF toolchains before GCC 14 (e.g. the GCC 12 of ESP-IDF 5.1 and the esp-13.2 toolchains), cannot lex non-ASCII literal suffixes, so `_µm` and `_µL` are unavailable there; use the ASCII alternatives `_um` and `_uL`. `SPATIAL_HAS_UTF8_LITERALS` reports which applies.

## Installation

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    spatial
    GIT_REPOSITORY https://github.com/cleishm/spatial-cpp.git
    GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(spatial)

target_link_libraries(your_target PRIVATE spatial::spatial)
```

### Manual

Copy the `include/spatial/` directory to your project.

## Usage

```cpp
#include <spatial/spatial>  // or <spatial/spatial.hpp>

using namespace spatial;
using namespace spatial_literals;

// Basic lengths
millimeters board_width{100};
kilometers commute{12};
feet ceiling{9};

// Using literals
auto trace = 8_mil;
auto run = 5_km;
auto wavelength = 850_nm;

// Precision control
millimeters precise{1532};
meters coarse = meters(precise);   // explicit lossy conversion (truncates to 1 m)
millimeters back = coarse;         // implicit lossless conversion (1000 mm)

// Arithmetic
auto total = 1_m + 5_cm;           // 105 cm
auto scaled = 12_in * 3;           // 36 in
auto ratio = 1_mi / 1_ft;          // 5280 (scalar)

// Comparisons work across precisions and systems
if (feet{1} == inches{12} && inches{1} == micrometers{25400}) {
    // true - same length, different precision
}

// Areas and volumes
auto floor_area = 12_ft * 10_ft;   // square_feet(120)
auto room = floor_area * 8_ft;     // cubic_feet(960)
auto height = room / floor_area;   // feet(8)

// String conversion
std::string s = to_string(millimeters(250));  // "250mm"

// Formatting: the default renders the exact stored value; a format spec
// changes only how the number is rendered, never the unit.
millimeters reading{1500};
std::string raw  = std::format("{}", reading);      // "1500mm" (exact stored value)
std::string disp = std::format("{:.1f}", reading);  // "1500.0mm"
std::format("{:.1f}", feet(6));                     // "6.0ft"
std::format("{}", 250_mL);                          // "250mL"
std::format("{:p}", 250_mL);                        // "250cm³" (power form)
```

### Metric and Imperial

Every precision is an exact `std::ratio` of one meter, so metric and imperial
quantities compare, add and convert exactly. Where a conversion is lossless it
is implicit; otherwise it needs a cast, and `floor`, `ceil` and `round` give
explicit rounding control:

```cpp
micrometers um = inches(1);                 // implicit: 25400 µm
inches in = feet(2);                        // implicit: 24 in
mils mil = inches(1);                       // implicit: 1000 mil
feet ft = miles(1);                         // implicit: 5280 ft

millimeters mm = millimeters(inches(1));    // explicit: 25 mm (truncated from 25.4)
auto r = round<millimeters>(feet(1));       // 305 mm (from 304.8)
auto f = floor<miles>(42195_m);             // 26 mi

auto sum = 1_m + 1_in;                      // exact, in units of 0.2 mm
length<double, std::milli> exact = 1_in;    // 25.4 mm
```

### Frequency Interop

Lengths expose the same `count()`/`rep`/`period` interface as
`std::chrono::duration`, with `period` giving the precision in meters. That is
what [frequency](https://github.com/cleishm/frequency-cpp)'s `distance_like`
concept requires, so a spatial length can be the result type of a wavelength
calculation (spatial itself does not depend on frequency). Areas and volumes
have `count()` and `rep` but deliberately no `period`, so they are not
mistaken for distances or durations:

```cpp
#include <frequency/frequency>
#include <spatial/spatial>

using namespace frequency_literals;

auto wifi = (2400_MHz).wavelength<spatial::millimeters>();    // 124 mm
auto fm = (100_MHz).wavelength<spatial::length<double>>();    // 2.998 m
auto sound = (1_kHz).wavelength<spatial::millimeters>(343.0); // 343 mm, in air
// (2400_MHz).wavelength<spatial::square_meters>();           // error: not distance_like
```

## Length Types

All standard aliases use `int64_t` representation. The alias template
`length<Rep, Precision>` covers custom representations and precisions, e.g.
`length<double>` or `length<float, std::milli>`.

| Type | Unit | Precision |
|------|------|-----------|
| `nanometers` | nm | 10⁻⁹ m |
| `micrometers` | µm | 10⁻⁶ m |
| `millimeters` | mm | 0.001 m |
| `centimeters` | cm | 0.01 m |
| `meters` | m | 1 m |
| `kilometers` | km | 1,000 m |
| `mils` | mil | 1/1000 in = 25.4 µm |
| `inches` | in | 25.4 mm |
| `feet` | ft | 12 in = 0.3048 m |
| `yards` | yd | 3 ft = 0.9144 m |
| `miles` | mi | 5280 ft = 1609.344 m |
| `nautical_miles` | nmi | 1852 m |

## Area and Volume

Areas and volumes are quantities of `square_meter_unit` and
`cubic_meter_unit`, with alias templates `area<Rep, Precision>` and
`volume<Rep, Precision>` whose precision is a `std::ratio` of one square or
cubic meter. They support the same arithmetic, conversions, rounding and
comparisons as lengths, within their own unit.

| Type | Unit | Precision |
|------|------|-----------|
| `square_millimeters` | mm² | 10⁻⁶ m² |
| `square_centimeters` | cm² | 10⁻⁴ m² |
| `square_meters` | m² | 1 m² |
| `hectares` | ha | 1 hm² = 10,000 m² |
| `square_kilometers` | km² | 10⁶ m² |
| `square_inches` | in² | 645.16 mm² |
| `square_feet` | ft² | 144 in² = 0.09290304 m² |
| `square_yards` | yd² | 9 ft² = 0.83612736 m² |
| `acres` | ac | 43,560 ft² = 4046.8564224 m² |
| `square_miles` | mi² | 640 ac = 2,589,988.110336 m² |

| Type | Unit | Precision |
|------|------|-----------|
| `microliters` (= `cubic_millimeters`) | µL | 1 mm³ = 10⁻⁹ m³ |
| `milliliters` (= `cubic_centimeters`) | mL | 1 cm³ = 10⁻⁶ m³ |
| `liters` | L | 1 dm³ = 10⁻³ m³ |
| `cubic_meters` | m³ | 1 m³ |
| `cubic_inches` | in³ | 16.387064 mL |
| `cubic_feet` | ft³ | 1728 in³ = 28.316846592 L |
| `cubic_yards` | yd³ | 27 ft³ = 0.764554857984 m³ |
| `us_gallons` | gal | 231 in³ = 3.785411784 L |

Lengths, areas and volumes are related by these operators, and no others:

| Operation | Result |
|-----------|--------|
| length × length | area |
| length × area, area × length | volume |
| area ÷ length | length |
| volume ÷ length | area |
| volume ÷ area | length |

Dividing two quantities of the same unit still yields a scalar
(`1_ac / 1_ft2 == 43560`). Any other combination, such as area × area,
length ÷ area or area + length, does not compile.

The result precision is the product (or quotient) of the operand precisions,
and the representation is their common type, so products are exact:

```cpp
static_assert(std::is_same_v<decltype(3_ft * 4_ft), square_feet>);
static_assert(3_ft * 4_ft == 12_ft2);
static_assert(1_ac == 43560_ft2);
static_assert(1_gal == 231_in3);

auto tank = 2_m * 1_m * 50_cm;      // 100 in units of 0.01 m³ (1 m³)
auto depth = 1000_cm3 / 100_cm2;    // centimeters(10)
auto mixed = 1_m * 1_mm;            // area in units of 10⁻³ m²
liters l = round<liters>(1_gal);    // 4 L (from 3.785)
```

Quotients divide the counts, so with integer representations they truncate
in the result precision, which may be coarse when the operand precisions
differ: `square_meters(10) / millimeters(3)` has kilometer precision and
yields 3 km. Convert the operands first, or use floating-point
representations, for fractional results.

A product whose precision cannot be represented as a `std::ratio` with
`intmax_t` terms, such as `nanometers × nanometers × nanometers` (10⁻²⁷ m³),
is a compile-time error.

## Literals

```cpp
using namespace spatial_literals;

850_nm     // nanometers(850)
50_um      // micrometers(50), also 50_µm (not on GCC < 14)
250_mm     // millimeters(250)
30_cm      // centimeters(30)
100_m      // meters(100)
5_km       // kilometers(5)
8_mil      // mils(8)
12_in      // inches(12)
6_ft       // feet(6)
100_yd     // yards(100)
26_mi      // miles(26)
12_nmi     // nautical_miles(12)
```

| Area literal | Type | | Volume literal | Type |
|--------------|------|-|----------------|------|
| `50_mm2` | `square_millimeters` | | `50_uL` (also `50_µL`, not on GCC < 14) | `microliters` |
| `100_cm2` | `square_centimeters` | | `50_mm3` | `microliters` (= `cubic_millimeters`) |
| `12_m2` | `square_meters` | | `250_mL` | `milliliters` |
| `3_ha` | `hectares` | | `250_cm3` | `milliliters` (= `cubic_centimeters`) |
| `5_km2` | `square_kilometers` | | `2_L` | `liters` |
| `144_in2` | `square_inches` | | `3_m3` | `cubic_meters` |
| `120_ft2` | `square_feet` | | `231_in3` | `cubic_inches` |
| `9_yd2` | `square_yards` | | `10_ft3` | `cubic_feet` |
| `40_ac` | `acres` | | `3_yd3` | `cubic_yards` |
| `2_mi2` | `square_miles` | | `5_gal` | `us_gallons` |

Literals are integral and checked for overflow at compile time; use a
floating-point type such as `length<double>` or `volume<double, std::milli>`
for fractional values.

## Conversions

Following `std::chrono` semantics:

- **Implicit** conversions are allowed when lossless (finer target precision,
  or floating-point target representation)
- **Explicit** conversions (constructor or `quantity_cast`) are required when
  the conversion may lose precision
- `floor`, `ceil` and `round` provide explicit rounding control

```cpp
millimeters mm = meters(5);                      // implicit: 5000 mm
meters m = meters(millimeters(5500));            // explicit: truncates to 5 m
auto r = round<meters>(millimeters(5500));       // 6 m (round half to even)
auto in = quantity_cast<inches>(centimeters(254)); // 100 in
```

## Formatting

By default, `to_string` and `std::format` show a quantity in its **own unit**,
whatever the format spec. `{}` renders the exact stored count with the
precision's suffix: SI-prefixed meters for metric lengths (`nm`, `µm`, `mm`,
`cm`, `m`, `km`) and `mil`, `in`, `ft`, `yd`, `mi`, `nmi` for the others. A
non-empty spec such as `{:.1f}` changes only the number formatting: the count
is rendered as a `double` with the spec and followed by the same suffix as
`{}`. A different unit is shown only when you opt in with a unit marker (see
below), or by converting the value first.

```cpp
std::format("{}", 850_nm);                              // "850nm"
std::format("{:.1f}", 850_nm);                          // "850.0nm"
std::format("{:.2f}", 30_cm);                           // "30.00cm"
std::format("{:.1f}", 26_mi);                           // "26.0mi"
std::format("{:8.1f}", 1500_mm);                        // "  1500.0mm"
std::format("{:.2f}", length<double, std::milli>(25.4)); // "25.40mm"
std::format("{:.3f}", length<double>(1500_mm));         // "1.500m"
```

Areas and volumes have explicit suffixes rather than SI-prefixed ones (a
millimeter squared is 10⁻⁶ m², not 10⁻³ m²). Some precisions also have a
**power form**, which writes the precision as a power of a length unit:

| Precision | Default | Power form |
|-----------|---------|------------|
| 10⁻¹⁸, 10⁻¹², 10⁻⁶, 10⁻⁴, 1, 10⁶ m² | `nm²`, `µm²`, `mm²`, `cm²`, `m²`, `km²` | same |
| 10⁴ m² | `ha` | `hm²` |
| mil², in², ft², yd², mi², nmi² | `mil²`, `in²`, `ft²`, `yd²`, `mi²`, `nmi²` | same |
| acre | `ac` | `ac` |
| 10⁻¹⁸, 1, 10⁹ m³ | `µm³`, `m³`, `km³` | same |
| 10⁻⁹ m³ | `µL` | `mm³` |
| 10⁻⁶ m³ | `mL` | `cm³` |
| 10⁻³ m³ | `L` | `dm³` |
| in³, ft³, yd³, mi³, nmi³ | `in³`, `ft³`, `yd³`, `mi³`, `nmi³` | same |
| US gallon | `gal` | `gal` |

Lengths have no separate power form.

### Unit markers

A format spec may begin with one marker letter that opts into a different
unit; the rest of the spec is then handled exactly as above. The same choices
are available from `to_string` through tag arguments, which work with or
without `std::format` support.

| Marker | `to_string` tag | Shows | Examples |
|--------|-----------------|-------|----------|
| `p` | `spatial::power_form` | the exact count, with the power-form suffix | `{:p}` on `24_mL` → `24cm³`; `{:p.1f}` → `24.0cm³` |
| `b` | `spatial::base_unit` | the value in the base unit: m, m² or m³ | `{:b.1f}` on `1500_mm` → `1.5m`; `{:b}` on `6_ft` → `1.8288m`; `{:b}` on `2_L` → `0.002m³` |
| `s` | `spatial::scaled` | metric values in a unit suited to their magnitude | `{:s}` on `1500_mm` → `1.5m`; `{:s}` on `1500_mL` → `1.5L`; `{:s}` on `6_ft` → `6ft` |

Wherever a floating-point value is rendered without a spec (`{:b}` and `{:s}`
with nothing after the marker, `{}` and `{:p}` on floating-point quantities,
and every `to_string` form), it is written in the shortest **fixed** notation
that round-trips: never scientific, and without padding zeros (`2.5`,
`1000000`, `0.000024`). `to_string` and `std::format` therefore produce exactly
the same text: `to_string(q, spatial::base_unit)` matches `{:b}`,
`to_string(q, spatial::scaled)` matches `{:s}`, and `to_string(q)` matches `{}`.
An explicit spec such as `{:b.3e}` is applied as given.

The scaled form applies to metric precisions (those that are powers of ten of
the base unit). It chooses the largest of these units in which the magnitude is
at least 1, or the smallest if there is none, and shows zero in the base unit:

- length: `nm`, `µm`, `mm`, `m`, `km`
- area: `nm²`, `µm²`, `mm²`, `m²`, `km²`
- volume: `µL`, `mL`, `L`, `m³`

Other precisions (mil, in, ft, yd, mi, nmi, their squares and cubes, acres and
US gallons) are shown in their own unit, with the count as a `double`.

```cpp
std::format("{}", 24_mL);                    // "24mL"
std::format("{:p}", 24_mL);                  // "24cm³"
std::format("{:p.1f}", 24_mL);               // "24.0cm³"
std::format("{:p>8.1f}", 24_mL);             // "    24.0cm³"
std::format("{:b}", 3_ha);                   // "30000m²"
std::format("{:b}", 24_mL);                  // "0.000024m³"
std::format("{}", length<double>(1e6));      // "1000000m"
std::format("{:s}", 850_nm);                 // "850nm"
std::format("{:s}", micrometers(-2500));     // "-2.5mm"
std::format("{:s.2f}", 1500_mm);             // "1.50m"
std::format("{:s}", 2000_L);                 // "2m³"
std::format("{:s}", 0_m3);                   // "0m³"
to_string(3_ha, spatial::power_form);        // "3hm²"
to_string(1500_mL, spatial::scaled);         // "1.5L"
to_string(6_ft, spatial::base_unit);         // "1.8288m"
```

A spec that starts with `p`, `b` or `s` is always a marker, so these letters
cannot be used as the fill character at the start of a spec: `{:p>8.1f}` is
the marker followed by `>8.1f`, not right-alignment filled with `p`. Only one
marker is allowed; a second one is passed on as part of the numeric spec and
is a format error.

Precisions with no conventional suffix (such as the 0.2 mm common type of
`meters` and `inches`, or the 10⁻³ m² area of `meters × millimeters`) are not
formattable, with or without a marker; convert them to a named precision first.

## ESP-IDF

The library can be used as an ESP-IDF component. Add it to your project's
`idf_component.yml`:

```yaml
dependencies:
  cleishm/spatial: "^0.1.0"
```

`std::format` support is controlled via `CONFIG_SPATIAL_STD_FORMAT` in
menuconfig (under "Component config" → "Spatial Library").

## Building and Testing

```bash
cmake -B build -DSPATIAL_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

## License

MIT License - see [LICENSE](LICENSE) for details.
