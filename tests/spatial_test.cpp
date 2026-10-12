// Unit tests for spatial library
// Uses Catch2 test framework with compile-time static_asserts

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <spatial/spatial>

using namespace spatial;
using namespace spatial_literals;

// =============================================================================
// Compile-time tests (static_assert)
// These verify correctness at compile time - if this file compiles, they pass.
// =============================================================================

// Construction and count
static_assert(nanometers(850).count() == 850);
static_assert(micrometers(50).count() == 50);
static_assert(millimeters(250).count() == 250);
static_assert(centimeters(30).count() == 30);
static_assert(meters(100).count() == 100);
static_assert(kilometers(5).count() == 5);
static_assert(mils(8).count() == 8);
static_assert(inches(12).count() == 12);
static_assert(feet(6).count() == 6);
static_assert(yards(100).count() == 100);
static_assert(miles(26).count() == 26);
static_assert(nautical_miles(12).count() == 12);

// Precisions are exact ratios of one meter
static_assert(std::is_same_v<meters::precision, std::ratio<1>>);
static_assert(std::is_same_v<centimeters::precision, std::centi>);
static_assert(std::is_same_v<mils::precision, std::ratio<127, 5000000>>);
static_assert(std::is_same_v<inches::precision, std::ratio<127, 5000>>);
static_assert(std::is_same_v<feet::precision, std::ratio<381, 1250>>);
static_assert(std::is_same_v<yards::precision, std::ratio<1143, 1250>>);
static_assert(std::is_same_v<miles::precision, std::ratio<201168, 125>>);
static_assert(std::is_same_v<nautical_miles::precision, std::ratio<1852>>);
static_assert(std::is_same_v<meters::unit, meter_unit>);
static_assert(std::is_same_v<length<double>, quantity<meter_unit, double, std::ratio<1>>>);

// Same-unit arithmetic
static_assert(meters(5) + meters(3) == meters(8));
static_assert(meters(5) - meters(3) == meters(2));
static_assert(meters(5) * 2 == meters(10));
static_assert(2 * meters(5) == meters(10));
static_assert(meters(10) / 2 == meters(5));
static_assert(meters(10) / meters(2) == 5);
static_assert(meters(10) % 3 == meters(1));
static_assert(meters(10) % meters(3) == meters(1));
static_assert(-meters(5) == meters(-5));
static_assert(+meters(5) == meters(5));
static_assert(miles(1) / feet(1) == 5280);
static_assert(feet(1) % inches(5) == inches(2));

// Same-precision results keep their type
static_assert(std::is_same_v<decltype(feet(1) + feet(2)), feet>);
static_assert(std::is_same_v<decltype(feet(1) * 2), feet>);
static_assert(std::is_same_v<decltype(-feet(1)), feet>);
static_assert(std::is_same_v<decltype(meters(10) / meters(2)), int64_t>);

// Mixed-precision arithmetic yields the common type
static_assert(meters(1) + centimeters(5) == centimeters(105));
static_assert(feet(1) + inches(1) == inches(13));
static_assert(meters(1) + inches(1) == micrometers(1025400));
static_assert((meters(1) + inches(1)).count() == 5127); // in units of 0.2 mm
static_assert(kilometers(1) - meters(1) == meters(999));

// Only the length/area/volume products and quotients are defined across units
template<typename A, typename B>
concept multipliable = requires(A a, B b) { a * b; };
template<typename A, typename B>
concept dividable = requires(A a, B b) { a / b; };
template<typename A, typename B>
concept addable = requires(A a, B b) { a + b; };
template<typename A, typename B>
concept subtractable = requires(A a, B b) { a - b; };
template<typename A, typename B>
concept comparable = requires(A a, B b) { a == b; };
static_assert(multipliable<meters, meters>);
static_assert(multipliable<inches, millimeters>);
static_assert(multipliable<meters, int>);
static_assert(multipliable<meters, square_meters>);
static_assert(multipliable<square_meters, meters>);
static_assert(dividable<square_meters, meters>);
static_assert(dividable<cubic_meters, meters>);
static_assert(dividable<cubic_meters, square_meters>);
static_assert(!multipliable<square_meters, square_meters>);
static_assert(!multipliable<square_meters, cubic_meters>);
static_assert(!multipliable<cubic_meters, meters>);
static_assert(!multipliable<meters, cubic_meters>);
static_assert(!multipliable<cubic_meters, cubic_meters>);
static_assert(!dividable<meters, square_meters>);
static_assert(!dividable<meters, cubic_meters>);
static_assert(!dividable<square_meters, cubic_meters>);
static_assert(!addable<square_meters, meters>);
static_assert(!addable<cubic_meters, square_meters>);
static_assert(!subtractable<meters, square_meters>);
static_assert(!comparable<square_meters, meters>);
static_assert(!comparable<liters, square_meters>);
static_assert(!multipliable<decltype(meters(1) * meters(1)), decltype(meters(1) * meters(1))>); // length^4
static_assert(!multipliable<decltype(meters(1) * meters(1) * meters(1)), meters>);              // length^4

// Metric precision equalities
static_assert(meters(1) == millimeters(1000));
static_assert(meters(1) == centimeters(100));
static_assert(centimeters(1) == millimeters(10));
static_assert(millimeters(1) == micrometers(1000));
static_assert(micrometers(1) == nanometers(1000));
static_assert(kilometers(1) == meters(1000));

// Imperial and nautical equalities, in both directions against metric
static_assert(inches(1) == micrometers(25400));
static_assert(micrometers(25400) == inches(1));
static_assert(inches(5) == millimeters(127));
static_assert(millimeters(127) == inches(5));
static_assert(mils(1) == nanometers(25400));
static_assert(inches(1) == mils(1000));
static_assert(feet(1) == inches(12));
static_assert(feet(1) == millimeters(304) + micrometers(800));
static_assert(yards(1) == feet(3));
static_assert(yards(1) == millimeters(914) + micrometers(400));
static_assert(miles(1) == feet(5280));
static_assert(miles(1) == yards(1760));
static_assert(miles(125) == meters(201168));
static_assert(nautical_miles(1) == meters(1852));
static_assert(millimeters(1000) != inches(39));

// Implicit lossless conversions
constexpr millimeters mm_from_m = meters(1);
static_assert(mm_from_m.count() == 1000);
constexpr centimeters cm_from_m = meters(2);
static_assert(cm_from_m.count() == 200);
constexpr nanometers nm_from_um = micrometers(3);
static_assert(nm_from_um.count() == 3000);
constexpr micrometers um_from_in = inches(1);
static_assert(um_from_in.count() == 25400);
constexpr mils mil_from_in = inches(2);
static_assert(mil_from_in.count() == 2000);
constexpr nanometers nm_from_mil = mils(1);
static_assert(nm_from_mil.count() == 25400);
constexpr inches in_from_ft = feet(2);
static_assert(in_from_ft.count() == 24);
constexpr inches in_from_yd = yards(1);
static_assert(in_from_yd.count() == 36);
constexpr feet ft_from_mi = miles(1);
static_assert(ft_from_mi.count() == 5280);
constexpr meters m_from_nmi = nautical_miles(2);
static_assert(m_from_nmi.count() == 3704);
constexpr millimeters mm_from_nmi = nautical_miles(1);
static_assert(mm_from_nmi.count() == 1852000);

// Lossless directions are implicit; lossy ones are explicit only
static_assert(std::is_convertible_v<meters, millimeters>);
static_assert(std::is_convertible_v<inches, micrometers>);
static_assert(std::is_convertible_v<inches, mils>);
static_assert(std::is_convertible_v<feet, inches>);
static_assert(std::is_convertible_v<miles, yards>);
static_assert(std::is_convertible_v<nautical_miles, meters>);
static_assert(!std::is_convertible_v<millimeters, meters>);
static_assert(!std::is_convertible_v<inches, millimeters>); // 25.4 mm
static_assert(!std::is_convertible_v<millimeters, inches>);
static_assert(!std::is_convertible_v<meters, feet>);
static_assert(!std::is_convertible_v<feet, meters>);
static_assert(!std::is_convertible_v<meters, nautical_miles>);
static_assert(!std::is_convertible_v<miles, meters>);
static_assert(std::is_constructible_v<millimeters, inches>);
static_assert(std::is_constructible_v<inches, millimeters>);
static_assert(std::is_constructible_v<meters, feet>);
static_assert(std::is_constructible_v<feet, meters>);

// Implicit to floating-point, from any precision
static_assert(std::is_convertible_v<millimeters, length<double>>);
static_assert(std::is_convertible_v<feet, length<double>>);
static_assert(!std::is_convertible_v<length<double>, meters>);

// Explicit lossy conversions (truncate toward zero)
static_assert(meters(millimeters(1500)).count() == 1);
static_assert(kilometers(meters(2500)).count() == 2);
static_assert(inches(millimeters(100)).count() == 3); // 3.937 in
static_assert(millimeters(inches(1)).count() == 25);  // 25.4 mm
static_assert(feet(meters(1)).count() == 3);          // 3.28 ft
static_assert(meters(feet(10)).count() == 3);         // 3.048 m
static_assert(meters(miles(1)).count() == 1609);      // 1609.344 m
static_assert(miles(kilometers(10)).count() == 6);    // 6.21 mi
static_assert(nautical_miles(kilometers(4)).count() == 2);
static_assert(inches(millimeters(-100)).count() == -3);

// quantity_cast
static_assert(quantity_cast<millimeters>(meters(5)).count() == 5000);
static_assert(quantity_cast<meters>(millimeters(5500)).count() == 5);
static_assert(quantity_cast<inches>(centimeters(254)).count() == 100);
static_assert(quantity_cast<centimeters>(inches(100)).count() == 254);
static_assert(quantity_cast<feet>(kilometers(1)).count() == 3280);
static_assert(quantity_cast<micrometers>(mils(3)).count() == 76); // 76.2 µm
static_assert(std::is_same_v<decltype(quantity_cast<feet>(meters(1))), feet>);

// Rounding conversions
static_assert(floor<meters>(millimeters(1500)) == meters(1));
static_assert(floor<meters>(millimeters(-1500)) == meters(-2));
static_assert(ceil<meters>(millimeters(1500)) == meters(2));
static_assert(ceil<meters>(millimeters(-1500)) == meters(-1));
static_assert(round<meters>(millimeters(1400)) == meters(1));
static_assert(round<meters>(millimeters(1600)) == meters(2));
static_assert(round<meters>(millimeters(1500)) == meters(2)); // tie to even
static_assert(round<meters>(millimeters(2500)) == meters(2)); // tie to even
static_assert(floor<inches>(centimeters(10)) == inches(3));   // 3.937
static_assert(ceil<inches>(centimeters(10)) == inches(4));
static_assert(round<inches>(centimeters(10)) == inches(4));
static_assert(floor<inches>(centimeters(-10)) == inches(-4));
static_assert(ceil<inches>(centimeters(-10)) == inches(-3));
static_assert(round<millimeters>(feet(1)) == millimeters(305)); // 304.8
static_assert(floor<millimeters>(feet(1)) == millimeters(304));
static_assert(ceil<feet>(meters(1)) == feet(4));
static_assert(round<feet>(meters(1)) == feet(3));
static_assert(round<miles>(kilometers(42)) == miles(26));        // 26.1
static_assert(round<millimeters>(inches(1)) == millimeters(25)); // 25.4
static_assert(round<millimeters>(mils(1000)) == millimeters(25));
static_assert(abs(millimeters(-5)) == millimeters(5));
static_assert(abs(millimeters(5)) == millimeters(5));
static_assert(abs(feet(-3)) == feet(3));

// zero/min/max
static_assert(meters::zero().count() == 0);
static_assert(meters::max().count() == std::numeric_limits<int64_t>::max());
static_assert(meters::min().count() == std::numeric_limits<int64_t>::lowest());
static_assert(feet::zero() == millimeters(0));
static_assert(length<double>::zero().count() == 0.0);

// Comparison operators
static_assert(meters(5) == meters(5));
static_assert(meters(5) != meters(6));
static_assert(meters(5) < meters(6));
static_assert(meters(6) > meters(5));
static_assert(meters(5) <= meters(5));
static_assert(meters(5) >= meters(5));
static_assert(millimeters(999) < meters(1));
static_assert(inches(39) < meters(1));
static_assert(inches(40) > meters(1));
static_assert(feet(3) < meters(1));
static_assert(yards(1) < meters(1));
static_assert(yards(2) > meters(1));
static_assert(miles(1) < nautical_miles(1));
static_assert(kilometers(2) > miles(1));
static_assert(mils(39) < millimeters(1));
static_assert(mils(40) > millimeters(1));
static_assert((inches(1) <=> millimeters(25)) == std::strong_ordering::greater);
static_assert((feet(1) <=> inches(12)) == std::strong_ordering::equal);
static_assert((length<double>(1.0) <=> length<double>(2.0)) == std::partial_ordering::less);

// =============================================================================
// Floating-point representations
// =============================================================================

static_assert(length<double>(2.5).count() == 2.5);
static_assert(length<double>(meters(5)).count() == 5.0); // implicit int -> double
static_assert(length<double>(millimeters(2500)).count() == 2.5);
static_assert(length<double>(inches(1)).count() == 0.0254);
static_assert(length<double, std::milli>(inches(1)).count() == 25.4);
static_assert(length<double, std::ratio<127, 5000>>(millimeters(127)).count() == 5.0);
static_assert(length<double>(1.0) + length<double>(0.5) == length<double>(1.5));

// =============================================================================
// common_type
// =============================================================================

static_assert(std::is_same_v<std::common_type_t<meters, millimeters>, millimeters>);
static_assert(std::is_same_v<std::common_type_t<kilometers, meters>, meters>);
static_assert(std::is_same_v<std::common_type_t<meters, meters>, meters>);
static_assert(std::is_same_v<std::common_type_t<centimeters, millimeters>, millimeters>);
static_assert(std::is_same_v<std::common_type_t<feet, inches>, inches>);
static_assert(std::is_same_v<std::common_type_t<miles, feet>, feet>);
static_assert(std::is_same_v<std::common_type_t<yards, feet>, feet>);
static_assert(std::is_same_v<std::common_type_t<inches, mils>, mils>);
static_assert(std::is_same_v<std::common_type_t<inches, micrometers>, micrometers>);
static_assert(std::is_same_v<std::common_type_t<nautical_miles, meters>, meters>);
// Mixed metric/imperial lands on the finest exact common precision
static_assert(std::is_same_v<std::common_type_t<inches, millimeters>, length<int64_t, std::ratio<1, 5000>>>);
static_assert(std::is_same_v<std::common_type_t<feet, meters>, length<int64_t, std::ratio<1, 1250>>>);
static_assert(std::is_same_v<std::common_type_t<miles, kilometers>, length<int64_t, std::ratio<8, 125>>>);
static_assert(std::is_same_v<std::common_type_t<nautical_miles, miles>, length<int64_t, std::ratio<4, 125>>>);
static_assert(std::is_same_v<std::common_type_t<meters, length<double>>, length<double>>);
static_assert(std::is_same_v<std::common_type_t<feet, length<double>>, length<double, std::ratio<1, 1250>>>);

// =============================================================================
// Area and volume
// =============================================================================

static_assert(std::is_same_v<area<double>, quantity<square_meter_unit, double, std::ratio<1>>>);
static_assert(std::is_same_v<volume<double>, quantity<cubic_meter_unit, double, std::ratio<1>>>);
static_assert(std::is_same_v<square_meters::unit, square_meter_unit>);
static_assert(std::is_same_v<cubic_meters::unit, cubic_meter_unit>);
static_assert(is_quantity_v<square_feet>);
static_assert(is_quantity_v<liters>);

// Area precisions, as exact ratios of one square meter
static_assert(std::is_same_v<square_millimeters::precision, std::ratio<1, 1000000>>);
static_assert(std::is_same_v<square_centimeters::precision, std::ratio<1, 10000>>);
static_assert(std::is_same_v<square_meters::precision, std::ratio<1>>);
static_assert(std::is_same_v<hectares::precision, std::ratio<10000>>);
static_assert(std::is_same_v<square_kilometers::precision, std::ratio<1000000>>);
static_assert(std::is_same_v<square_inches::precision, std::ratio<16129, 25000000>>);
static_assert(std::is_same_v<square_feet::precision, std::ratio<145161, 1562500>>);
static_assert(std::is_same_v<square_yards::precision, std::ratio<1306449, 1562500>>);
static_assert(std::is_same_v<acres::precision, std::ratio<316160658, 78125>>);
static_assert(std::is_same_v<square_miles::precision, std::ratio<40468564224, 15625>>);

// ... which are the squares of the length precisions
template<typename L>
using square_of = std::ratio_multiply<typename L::precision, typename L::precision>;
template<typename L>
using cube_of = std::ratio_multiply<square_of<L>, typename L::precision>;
static_assert(std::is_same_v<square_millimeters::precision, square_of<millimeters>>);
static_assert(std::is_same_v<square_centimeters::precision, square_of<centimeters>>);
static_assert(std::is_same_v<square_meters::precision, square_of<meters>>);
static_assert(std::is_same_v<square_kilometers::precision, square_of<kilometers>>);
static_assert(std::is_same_v<square_inches::precision, square_of<inches>>);
static_assert(std::is_same_v<square_feet::precision, square_of<feet>>);
static_assert(std::is_same_v<square_yards::precision, square_of<yards>>);
static_assert(std::is_same_v<square_miles::precision, square_of<miles>>);
static_assert(std::is_same_v<hectares::precision, std::ratio_multiply<std::hecto, std::hecto>>);

// Volume precisions, as exact ratios of one cubic meter
static_assert(std::is_same_v<microliters::precision, std::ratio<1, 1000000000>>);
static_assert(std::is_same_v<milliliters::precision, std::ratio<1, 1000000>>);
static_assert(std::is_same_v<liters::precision, std::ratio<1, 1000>>);
static_assert(std::is_same_v<cubic_meters::precision, std::ratio<1>>);
static_assert(std::is_same_v<cubic_inches::precision, std::ratio<2048383, 125000000000>>);
static_assert(std::is_same_v<cubic_feet::precision, std::ratio<55306341, 1953125000>>);
static_assert(std::is_same_v<cubic_yards::precision, std::ratio<1493271207, 1953125000>>);
static_assert(std::is_same_v<us_gallons::precision, std::ratio<473176473, 125000000000>>);
static_assert(std::is_same_v<cubic_millimeters, microliters>);
static_assert(std::is_same_v<cubic_centimeters, milliliters>);

// ... which are the cubes of the length precisions
static_assert(std::is_same_v<microliters::precision, cube_of<millimeters>>);
static_assert(std::is_same_v<milliliters::precision, cube_of<centimeters>>);
static_assert(std::is_same_v<liters::precision, cube_of<length<int64_t, std::deci>>>);
static_assert(std::is_same_v<cubic_meters::precision, cube_of<meters>>);
static_assert(std::is_same_v<cubic_inches::precision, cube_of<inches>>);
static_assert(std::is_same_v<cubic_feet::precision, cube_of<feet>>);
static_assert(std::is_same_v<cubic_yards::precision, cube_of<yards>>);

// Defined equalities
static_assert(hectares(1) == square_meters(10000));
static_assert(square_kilometers(1) == hectares(100));
static_assert(square_meters(1) == square_centimeters(10000));
static_assert(square_centimeters(1) == square_millimeters(100));
static_assert(square_feet(1) == square_inches(144));
static_assert(square_yards(1) == square_feet(9));
static_assert(acres(1) == square_feet(43560));
static_assert(acres(1) == square_yards(4840));
static_assert(square_miles(1) == acres(640));
static_assert(square_inches(1) == square_millimeters(645) + area<int64_t, std::ratio<1, 100000000>>(16));
static_assert(liters(1) == milliliters(1000));
static_assert(milliliters(1) == microliters(1000));
static_assert(cubic_meters(1) == liters(1000));
static_assert(cubic_feet(1) == cubic_inches(1728));
static_assert(cubic_yards(1) == cubic_feet(27));
static_assert(us_gallons(1) == cubic_inches(231));
static_assert(cubic_inches(1000) == microliters(16387064));
static_assert(us_gallons(1000) == milliliters(3785411) + microliters(784));

// Products and quotients
static_assert(std::is_same_v<decltype(feet(3) * feet(4)), square_feet>);
static_assert(feet(3) * feet(4) == square_feet(12));
static_assert(std::is_same_v<decltype(meters(3) * meters(4)), square_meters>);
static_assert(std::is_same_v<decltype(millimeters(1) * millimeters(1)), square_millimeters>);
static_assert(std::is_same_v<decltype(centimeters(1) * centimeters(1)), square_centimeters>);
static_assert(std::is_same_v<decltype(kilometers(1) * kilometers(1)), square_kilometers>);
static_assert(std::is_same_v<decltype(inches(1) * inches(1)), square_inches>);
static_assert(std::is_same_v<decltype(yards(1) * yards(1)), square_yards>);
static_assert(std::is_same_v<decltype(miles(1) * miles(1)), square_miles>);
static_assert(std::is_same_v<decltype(meters(1) * millimeters(1)), area<int64_t, std::milli>>);
static_assert(meters(2) * millimeters(3) == square_millimeters(6000));
static_assert(feet(1) * inches(12) == square_feet(1));
static_assert(yards(66) * yards(220) == acres(3)); // 3 chains x 10 chains

static_assert(std::is_same_v<decltype(feet(2) * square_feet(12)), cubic_feet>);
static_assert(std::is_same_v<decltype(square_feet(12) * feet(2)), cubic_feet>);
static_assert(feet(2) * square_feet(12) == cubic_feet(24));
static_assert(square_feet(12) * feet(2) == cubic_feet(24));
static_assert(std::is_same_v<decltype(centimeters(2) * square_centimeters(3)), milliliters>);
static_assert(std::is_same_v<decltype(millimeters(1) * square_millimeters(1)), microliters>);
static_assert(std::is_same_v<decltype(inches(1) * square_inches(1)), cubic_inches>);
static_assert(std::is_same_v<decltype(yards(1) * square_yards(1)), cubic_yards>);
static_assert(std::is_same_v<decltype(meters(1) * meters(1) * meters(1)), cubic_meters>);
static_assert(meters(2) * meters(1) * centimeters(50) == cubic_meters(1));
static_assert(centimeters(10) * centimeters(10) * centimeters(10) == liters(1));

static_assert(std::is_same_v<decltype(square_feet(12) / feet(3)), feet>);
static_assert(square_feet(12) / feet(3) == feet(4));
static_assert(square_meters(1) / millimeters(1) == meters(1000));
static_assert(std::is_same_v<decltype(square_meters(1) / millimeters(1)), kilometers>);
static_assert(square_meters(10) / millimeters(3) == kilometers(3)); // truncates in the result precision
static_assert(std::is_same_v<decltype(cubic_feet(24) / feet(2)), square_feet>);
static_assert(cubic_feet(24) / feet(2) == square_feet(12));
static_assert(std::is_same_v<decltype(milliliters(1000) / centimeters(10)), square_centimeters>);
static_assert(milliliters(1000) / centimeters(10) == square_centimeters(100));
static_assert(std::is_same_v<decltype(cubic_feet(24) / square_feet(12)), feet>);
static_assert(cubic_feet(24) / square_feet(12) == feet(2));
static_assert(std::is_same_v<decltype(milliliters(1000) / square_centimeters(100)), centimeters>);
static_assert(milliliters(1000) / square_centimeters(100) == centimeters(10));

// Same-unit quotients are still scalars
static_assert(std::is_same_v<decltype(acres(1) / square_feet(1)), int64_t>);
static_assert(acres(1) / square_feet(1) == 43560);
static_assert(liters(1) / milliliters(1) == 1000);
static_assert(us_gallons(1) / cubic_inches(1) == 231);

// Mixed representations take the common type
static_assert(std::is_same_v<decltype(length<double>(1.5) * meters(2)), area<double>>);
static_assert(length<double>(1.5) * meters(2) == area<double>(3.0));
static_assert(std::is_same_v<decltype(volume<double>(3.0) / length<int>(2)), area<double>>);
static_assert(volume<double>(3.0) / length<int>(2) == area<double>(1.5));

// Same-unit arithmetic, conversions and rounding work for areas and volumes
static_assert(square_feet(1) + square_inches(1) == square_inches(145));
static_assert(liters(1) - milliliters(1) == milliliters(999));
static_assert(std::is_convertible_v<square_feet, square_inches>);
static_assert(!std::is_convertible_v<square_inches, square_feet>);
static_assert(std::is_convertible_v<us_gallons, cubic_inches>);
static_assert(std::is_convertible_v<liters, milliliters>);
static_assert(!std::is_convertible_v<milliliters, liters>);
static_assert(round<liters>(us_gallons(1)) == liters(4)); // 3.785 L
static_assert(floor<acres>(hectares(1)) == acres(2));     // 2.471 ac
static_assert(quantity_cast<square_meters>(acres(1)).count() == 4046);
static_assert(std::is_same_v<std::common_type_t<square_feet, square_inches>, square_inches>);
static_assert(square_meters::max().count() == std::numeric_limits<int64_t>::max());

// =============================================================================
// User-defined literals
// =============================================================================

static_assert(850_nm == nanometers(850));
static_assert(50_um == micrometers(50));
#if SPATIAL_HAS_UTF8_LITERALS
static_assert(50_µm == micrometers(50));
#endif
static_assert(250_mm == millimeters(250));
static_assert(30_cm == centimeters(30));
static_assert(100_m == meters(100));
static_assert(5_km == kilometers(5));
static_assert(8_mil == mils(8));
static_assert(12_in == inches(12));
static_assert(6_ft == feet(6));
static_assert(100_yd == yards(100));
static_assert(26_mi == miles(26));
static_assert(12_nmi == nautical_miles(12));
static_assert(std::is_same_v<decltype(850_nm), nanometers>);
#if SPATIAL_HAS_UTF8_LITERALS
static_assert(std::is_same_v<decltype(50_µm), micrometers>);
#endif
static_assert(std::is_same_v<decltype(30_cm), centimeters>);
static_assert(std::is_same_v<decltype(8_mil), mils>);
static_assert(std::is_same_v<decltype(12_nmi), nautical_miles>);
static_assert(9223372036854775807_m == meters::max());

// Area literals
static_assert(50_mm2 == square_millimeters(50));
static_assert(100_cm2 == square_centimeters(100));
static_assert(12_m2 == square_meters(12));
static_assert(3_ha == hectares(3));
static_assert(5_km2 == square_kilometers(5));
static_assert(144_in2 == square_inches(144));
static_assert(120_ft2 == square_feet(120));
static_assert(9_yd2 == square_yards(9));
static_assert(40_ac == acres(40));
static_assert(2_mi2 == square_miles(2));
static_assert(std::is_same_v<decltype(50_mm2), square_millimeters>);
static_assert(std::is_same_v<decltype(100_cm2), square_centimeters>);
static_assert(std::is_same_v<decltype(12_m2), square_meters>);
static_assert(std::is_same_v<decltype(3_ha), hectares>);
static_assert(std::is_same_v<decltype(5_km2), square_kilometers>);
static_assert(std::is_same_v<decltype(144_in2), square_inches>);
static_assert(std::is_same_v<decltype(120_ft2), square_feet>);
static_assert(std::is_same_v<decltype(9_yd2), square_yards>);
static_assert(std::is_same_v<decltype(40_ac), acres>);
static_assert(std::is_same_v<decltype(2_mi2), square_miles>);

// Volume literals
static_assert(50_uL == microliters(50));
#if SPATIAL_HAS_UTF8_LITERALS
static_assert(50_µL == microliters(50));
static_assert(std::is_same_v<decltype(50_µL), microliters>);
#endif
static_assert(250_mL == milliliters(250));
static_assert(2_L == liters(2));
static_assert(3_m3 == cubic_meters(3));
static_assert(50_mm3 == microliters(50));
static_assert(250_cm3 == milliliters(250));
static_assert(231_in3 == cubic_inches(231));
static_assert(10_ft3 == cubic_feet(10));
static_assert(3_yd3 == cubic_yards(3));
static_assert(5_gal == us_gallons(5));
static_assert(std::is_same_v<decltype(50_uL), microliters>);
static_assert(std::is_same_v<decltype(250_mL), milliliters>);
static_assert(std::is_same_v<decltype(2_L), liters>);
static_assert(std::is_same_v<decltype(3_m3), cubic_meters>);
static_assert(std::is_same_v<decltype(50_mm3), microliters>);
static_assert(std::is_same_v<decltype(250_cm3), milliliters>);
static_assert(std::is_same_v<decltype(231_in3), cubic_inches>);
static_assert(std::is_same_v<decltype(10_ft3), cubic_feet>);
static_assert(std::is_same_v<decltype(3_yd3), cubic_yards>);
static_assert(std::is_same_v<decltype(5_gal), us_gallons>);
static_assert(9223372036854775807_L == liters::max());

// Literals in expressions
static_assert(1_m + 5_cm == 105_cm);
static_assert(1_ft + 1_in == 13_in);
static_assert(1_mi == 5280_ft);
static_assert(1_in == 25400_um);
static_assert(1_nmi == 1852_m);
static_assert(-3_mm == millimeters(-3));
static_assert(1_ac == 43560_ft2);
static_assert(1_gal == 231_in3);
static_assert(1_L == 1000_cm3);
static_assert(1000_cm3 / 100_cm2 == 10_cm);
static_assert(12_ft * 10_ft == 120_ft2);

// =============================================================================
// Traits
// =============================================================================

static_assert(is_quantity_v<meters>);
static_assert(is_quantity_v<feet>);
static_assert(is_quantity_v<length<double>>);
static_assert(!is_quantity_v<int>);
static_assert(treat_as_inexact_v<double>);
static_assert(treat_as_inexact_v<float>);
static_assert(!treat_as_inexact_v<int64_t>);

// =============================================================================
// Duration-like interface (period alias)
// =============================================================================

// A local copy of freq::distance_like from frequency-cpp
// (frequency/include/frequency/frequency.hpp); this repo does not depend on
// frequency, so the concept is reproduced here to pin the interface.
template<typename T>
concept distance_like = requires(T t) {
    { t.count() };
    typename T::period;
    typename T::rep;
};

static_assert(distance_like<meters>);
static_assert(distance_like<nanometers>);
static_assert(distance_like<feet>);
static_assert(distance_like<length<double, std::milli>>);
static_assert(std::is_same_v<meters::period, std::ratio<1>>);
static_assert(std::is_same_v<millimeters::period, std::milli>);
static_assert(std::is_same_v<inches::period, std::ratio<127, 5000>>);
static_assert(std::is_same_v<nautical_miles::period, std::ratio<1852>>);
static_assert(std::is_same_v<length<int, std::ratio<2, 4>>::period, std::ratio<1, 2>>); // reduced
static_assert(std::is_same_v<feet::period, feet::precision>);

// Only lengths have a period, so areas and volumes are not distance-like
template<typename T>
concept has_period = requires { typename T::period; };
static_assert(has_period<meters>);
static_assert(has_period<length<double>>);
static_assert(!has_period<square_meters>);
static_assert(!has_period<square_feet>);
static_assert(!has_period<area<double>>);
static_assert(!has_period<cubic_meters>);
static_assert(!has_period<milliliters>);
static_assert(!has_period<volume<double>>);
static_assert(!distance_like<square_meters>);
static_assert(!distance_like<liters>);

// ... but everything else of the interface is common to all units
template<typename T>
concept quantity_interface = requires(T t) {
    { t.count() } -> std::same_as<typename T::rep>;
    typename T::precision;
    typename T::unit;
    T::zero();
    T::min();
    T::max();
};
static_assert(quantity_interface<meters>);
static_assert(quantity_interface<square_feet>);
static_assert(quantity_interface<liters>);
static_assert(std::is_same_v<acres::rep, int64_t>);

// What freq::frequency::wavelength<Distance>() does with a distance-like
// type: convert a length in meters into Distance's units via its period.
template<distance_like Distance>
constexpr Distance from_meters(double m) {
    using p = typename Distance::period;
    return Distance(static_cast<typename Distance::rep>(m * p::den / p::num));
}

static_assert(from_meters<millimeters>(2.5) == millimeters(2500));
static_assert(from_meters<inches>(0.254) == inches(10));
static_assert(from_meters<length<double>>(1.5) == length<double>(1.5));
static_assert(from_meters<meters>(1609.344) == meters(1609));

// =============================================================================
// Runtime tests
// =============================================================================

TEST_CASE("compound assignment operators", "[quantity]") {
    millimeters d{1000};
    d += millimeters{500};
    REQUIRE(d.count() == 1500);
    d -= millimeters{300};
    REQUIRE(d.count() == 1200);
    d *= 2;
    REQUIRE(d.count() == 2400);
    d /= 4;
    REQUIRE(d.count() == 600);
    d %= 7;
    REQUIRE(d.count() == 5);
    d %= millimeters{3};
    REQUIRE(d.count() == 2);

    // Lossless implicit conversion on the right-hand side
    millimeters total{0};
    total += meters{1};
    total += centimeters{5};
    REQUIRE(total == 1050_mm);
}

TEST_CASE("increment and decrement operators", "[quantity]") {
    feet f{5};
    REQUIRE((++f).count() == 6);
    REQUIRE((f++).count() == 6);
    REQUIRE(f.count() == 7);
    REQUIRE((--f).count() == 6);
    REQUIRE((f--).count() == 6);
    REQUIRE(f.count() == 5);
}

TEST_CASE("metric and imperial conversions", "[conversion]") {
    // Imperial -> metric
    millimeters a = millimeters(inches(10)); // 254 mm, exact
    REQUIRE(a.count() == 254);
    micrometers b = feet(1); // implicit: exact in micrometers
    REQUIRE(b.count() == 304800);
    meters c = round<meters>(miles(1));
    REQUIRE(c.count() == 1609);

    // Metric -> imperial
    REQUIRE(quantity_cast<inches>(meters(1)).count() == 39);
    REQUIRE(round<feet>(kilometers(1)).count() == 3281);
    REQUIRE(round<yards>(meters(100)).count() == 109);
    REQUIRE(round<mils>(millimeters(1)).count() == 39);

    // Floating-point round trips are exact enough to recover the original
    length<double, std::ratio<127, 5000>> in_d = millimeters(100);
    REQUIRE(in_d.count() == Catch::Approx(3.937007874));
    length<double, std::milli> mm_d = in_d;
    REQUIRE(mm_d.count() == Catch::Approx(100.0));
    length<double> m_d = 1_nmi;
    REQUIRE(m_d.count() == Catch::Approx(1852.0));
}

TEST_CASE("floating-point arithmetic", "[quantity]") {
    length<double> a{1.25};
    length<double, std::milli> b{500.0};
    auto sum = a + b;
    REQUIRE(sum.count() == Catch::Approx(1750.0)); // common type is millimeters
    REQUIRE((a / 2.0).count() == Catch::Approx(0.625));
    REQUIRE(a / b == Catch::Approx(2.5));
}

TEST_CASE("to_string produces unit suffixes", "[format]") {
    REQUIRE(to_string(nanometers(850)) == "850nm");
    REQUIRE(to_string(micrometers(50)) == "50µm");
    REQUIRE(to_string(millimeters(250)) == "250mm");
    REQUIRE(to_string(centimeters(30)) == "30cm");
    REQUIRE(to_string(meters(100)) == "100m");
    REQUIRE(to_string(kilometers(5)) == "5km");
    REQUIRE(to_string(mils(8)) == "8mil");
    REQUIRE(to_string(inches(12)) == "12in");
    REQUIRE(to_string(feet(6)) == "6ft");
    REQUIRE(to_string(yards(100)) == "100yd");
    REQUIRE(to_string(miles(26)) == "26mi");
    REQUIRE(to_string(nautical_miles(12)) == "12nmi");
    REQUIRE(to_string(meters(-3)) == "-3m");

    // Common types that land on a named precision keep its suffix
    REQUIRE(to_string(1_ft + 1_in) == "13in");
    REQUIRE(to_string(1_m + 5_cm) == "105cm");

    // Lengths have no separate power form
    REQUIRE(to_string(millimeters(250), power_form) == "250mm");
    REQUIRE(to_string(feet(6), power_form) == "6ft");
    REQUIRE(to_string(nautical_miles(12), power_form) == "12nmi");
}

// Every area and volume suffix: the type, its default suffix and its power form.
template<typename Q>
struct suffix_case {
    Q q;
    const char* default_suffix;
    const char* power_suffix;
};

template<typename Q>
static void check_suffix(const suffix_case<Q>& c) {
    const std::string count = std::to_string(c.q.count());
    CHECK(to_string(c.q) == count + c.default_suffix);
    CHECK(to_string(c.q, power_form) == count + c.power_suffix);
#if CONFIG_SPATIAL_STD_FORMAT
    CHECK(std::format("{}", c.q) == count + c.default_suffix);
    CHECK(std::format("{:p}", c.q) == count + c.power_suffix);
    CHECK(std::format("{:.1f}", c.q) == count + ".0" + c.default_suffix);
    CHECK(std::format("{:p.1f}", c.q) == count + ".0" + c.power_suffix);
#endif
}

using square_nanometers = area<int64_t, std::atto>;
using square_micrometers = area<int64_t, std::pico>;
using square_mils = area<int64_t, std::ratio<16129, 25000000000000>>;
using square_nautical_miles = area<int64_t, std::ratio<3429904>>;
using cubic_micrometers = volume<int64_t, std::atto>;
using cubic_kilometers = volume<int64_t, std::giga>;
using cubic_miles = volume<int64_t, std::ratio<8140980127813632, 1953125>>;
using cubic_nautical_miles = volume<int64_t, std::ratio<6352182208>>;

static_assert(std::is_same_v<decltype(nanometers(1) * nanometers(1)), square_nanometers>);
static_assert(std::is_same_v<decltype(micrometers(1) * micrometers(1)), square_micrometers>);
static_assert(std::is_same_v<decltype(mils(1) * mils(1)), square_mils>);
static_assert(std::is_same_v<decltype(nautical_miles(1) * nautical_miles(1)), square_nautical_miles>);
static_assert(std::is_same_v<decltype(micrometers(1) * micrometers(1) * micrometers(1)), cubic_micrometers>);
static_assert(std::is_same_v<decltype(kilometers(1) * kilometers(1) * kilometers(1)), cubic_kilometers>);
static_assert(std::is_same_v<decltype(miles(1) * miles(1) * miles(1)), cubic_miles>);
static_assert(std::is_same_v<decltype(nautical_miles(1) * square_nautical_miles(1)), cubic_nautical_miles>);

TEST_CASE("area suffixes", "[format]") {
    check_suffix(suffix_case{square_nanometers(7), "nm²", "nm²"});
    check_suffix(suffix_case{square_micrometers(7), "µm²", "µm²"});
    check_suffix(suffix_case{square_millimeters(7), "mm²", "mm²"});
    check_suffix(suffix_case{square_centimeters(7), "cm²", "cm²"});
    check_suffix(suffix_case{square_meters(7), "m²", "m²"});
    check_suffix(suffix_case{hectares(7), "ha", "hm²"});
    check_suffix(suffix_case{square_kilometers(7), "km²", "km²"});
    check_suffix(suffix_case{square_mils(7), "mil²", "mil²"});
    check_suffix(suffix_case{square_inches(7), "in²", "in²"});
    check_suffix(suffix_case{square_feet(7), "ft²", "ft²"});
    check_suffix(suffix_case{square_yards(7), "yd²", "yd²"});
    check_suffix(suffix_case{acres(7), "ac", "ac"});
    check_suffix(suffix_case{square_miles(7), "mi²", "mi²"});
    check_suffix(suffix_case{square_nautical_miles(7), "nmi²", "nmi²"});
}

TEST_CASE("volume suffixes", "[format]") {
    check_suffix(suffix_case{cubic_micrometers(24), "µm³", "µm³"});
    check_suffix(suffix_case{microliters(24), "µL", "mm³"});
    check_suffix(suffix_case{milliliters(24), "mL", "cm³"});
    check_suffix(suffix_case{liters(24), "L", "dm³"});
    check_suffix(suffix_case{cubic_meters(24), "m³", "m³"});
    check_suffix(suffix_case{cubic_kilometers(24), "km³", "km³"});
    check_suffix(suffix_case{cubic_inches(24), "in³", "in³"});
    check_suffix(suffix_case{cubic_feet(24), "ft³", "ft³"});
    check_suffix(suffix_case{cubic_yards(24), "yd³", "yd³"});
    check_suffix(suffix_case{cubic_miles(24), "mi³", "mi³"});
    check_suffix(suffix_case{cubic_nautical_miles(24), "nmi³", "nmi³"});
    check_suffix(suffix_case{us_gallons(24), "gal", "gal"});
}

TEST_CASE("to_string for areas and volumes", "[format]") {
    REQUIRE(to_string(feet(3) * feet(4)) == "12ft²");
    REQUIRE(to_string(square_feet(12) * feet(2)) == "24ft³");
    REQUIRE(to_string(24_mL) == "24mL");
    REQUIRE(to_string(24_mL, power_form) == "24cm³");
    REQUIRE(to_string(24_cm3) == "24mL"); // same type as milliliters
    REQUIRE(to_string(3_ha, power_form) == "3hm²");
    REQUIRE(to_string(liters(-2), power_form) == "-2dm³");
    REQUIRE(to_string(centimeters(10) * centimeters(10) * centimeters(10)) == "1000mL");
}

// Precisions with no conventional suffix are not formattable at all, rather
// than failing at runtime.
using fifth_millimeters = length<int64_t, std::ratio<1, 5000>>;
template<typename Q>
concept has_to_string = requires(Q q) { to_string(q); };
static_assert(has_to_string<meters>);
static_assert(has_to_string<feet>);
static_assert(!has_to_string<fifth_millimeters>);
static_assert(!has_to_string<std::common_type_t<inches, millimeters>>);

// Areas and volumes have no SI-prefix rule: only the listed precisions have
// suffixes, so e.g. 10⁻³ m² (meters x millimeters) is not formattable.
template<typename Q>
concept has_power_to_string = requires(Q q) { to_string(q, power_form); };
using milli_square_meters = area<int64_t, std::milli>;
using kilo_square_meters = area<int64_t, std::kilo>;
using micro_cubic_meters_x10 = volume<int64_t, std::ratio<1, 100000>>;
static_assert(std::is_same_v<decltype(meters(1) * millimeters(1)), milli_square_meters>);
static_assert(has_to_string<square_feet>);
static_assert(has_to_string<liters>);
static_assert(has_power_to_string<liters>);
static_assert(has_power_to_string<meters>);
static_assert(!has_to_string<milli_square_meters>);
static_assert(!has_power_to_string<milli_square_meters>);
static_assert(!has_to_string<kilo_square_meters>);
static_assert(!has_to_string<micro_cubic_meters_x10>);
static_assert(!has_to_string<volume<int64_t, std::kilo>>);
static_assert(!has_to_string<std::common_type_t<square_feet, square_meters>>);
static_assert(!has_to_string<std::common_type_t<us_gallons, liters>>);
static_assert(!has_power_to_string<fifth_millimeters>);
// Volume precision 10⁻¹⁸ (µm³) and area precision 10⁻¹⁸ (nm²) differ by unit
static_assert(has_to_string<volume<int64_t, std::atto>>);
static_assert(has_to_string<area<int64_t, std::atto>>);
static_assert(!has_to_string<area<int64_t, std::nano>>); // not µL: that is a volume

// The base-unit and scaled forms are available for exactly the same types.
template<typename Q>
concept has_base_to_string = requires(Q q) { to_string(q, base_unit); };
template<typename Q>
concept has_scaled_to_string = requires(Q q) { to_string(q, scaled); };
static_assert(has_base_to_string<meters>);
static_assert(has_base_to_string<square_feet>);
static_assert(has_scaled_to_string<liters>);
static_assert(has_scaled_to_string<acres>);
static_assert(!has_base_to_string<fifth_millimeters>);
static_assert(!has_scaled_to_string<fifth_millimeters>);
static_assert(!has_base_to_string<milli_square_meters>);
static_assert(!has_scaled_to_string<milli_square_meters>);
static_assert(!has_scaled_to_string<micro_cubic_meters_x10>);

// to_string(q, base_unit) and to_string(q, scaled), and the equivalent {:b}
// and {:s} formatting, which must agree exactly.
template<typename Q>
static void check_forms(const Q& q, const char* base_expected, const char* scaled_expected) {
    CAPTURE(to_string(q));
    CHECK(to_string(q, base_unit) == base_expected);
    CHECK(to_string(q, scaled) == scaled_expected);
#if CONFIG_SPATIAL_STD_FORMAT
    CHECK(std::format("{:b}", q) == base_expected);
    CHECK(std::format("{:s}", q) == scaled_expected);
#endif
}

// Floating-point counts with no spec: shortest round-trip fixed notation, never
// scientific, identical between to_string and std::format("{}").
template<typename Q>
static void check_plain(const Q& q, const char* expected, const char* power_expected) {
    CHECK(to_string(q) == expected);
    CHECK(to_string(q, power_form) == power_expected);
#if CONFIG_SPATIAL_STD_FORMAT
    CHECK(std::format("{}", q) == expected);
    CHECK(std::format("{:p}", q) == power_expected);
#endif
}

TEST_CASE("floating-point counts without a spec", "[format]") {
    check_plain(length<double>(1000000.0), "1000000m", "1000000m");
    check_plain(length<double>(2.5), "2.5m", "2.5m");
    check_plain(length<double>(-2.5), "-2.5m", "-2.5m");
    check_plain(length<double>(0.0), "0m", "0m");
    check_plain(length<double>(-0.0), "-0m", "-0m");
    check_plain(length<double>(1e22), "10000000000000000000000m", "10000000000000000000000m");
    check_plain(length<double, std::milli>(0.000001), "0.000001mm", "0.000001mm");
    check_plain(length<double, std::milli>(1.0 / 3.0), "0.3333333333333333mm", "0.3333333333333333mm");
    check_plain(length<float>(0.1f), "0.1m", "0.1m");
    check_plain(volume<double, std::micro>(0.0000001), "0.0000001mL", "0.0000001cm³");
    check_plain(area<double, std::ratio<10000>>(1e7), "10000000ha", "10000000hm²");
    // Integer counts are unchanged.
    check_plain(meters(1000000), "1000000m", "1000000m");
    check_plain(milliliters(-24), "-24mL", "-24cm³");
#if CONFIG_SPATIAL_STD_FORMAT
    // Explicit specs are unchanged.
    REQUIRE(std::format("{:.3e}", length<double>(1e6)) == "1.000e+06m");
    REQUIRE(std::format("{:b.2f}", milliliters(24)) == "0.00m³");
    REQUIRE(std::format("{:b.3e}", milliliters(24)) == "2.400e-05m³");
#endif
}

TEST_CASE("base-unit and scaled forms", "[format]") {
    // Lengths
    check_forms(millimeters(1500), "1.5m", "1.5m");
    check_forms(nanometers(850), "0.00000085m", "850nm");
    check_forms(centimeters(150), "1.5m", "1.5m");
    check_forms(micrometers(-2500), "-0.0025m", "-2.5mm");
    check_forms(meters(0), "0m", "0m");
    check_forms(millimeters(0), "0m", "0m");
    check_forms(kilometers(-3), "-3000m", "-3km");
    check_forms(meters(1500000), "1500000m", "1500km");
    check_forms(length<int64_t, std::femto>(5), "0.000000000000005m", "0.000005nm");
    check_forms(length<int64_t, std::tera>(3), "3000000000000m", "3000000000km");
    check_forms(length<double, std::milli>(2.5), "0.0025m", "2.5mm");
    check_forms(length<int64_t, std::femto>(1), "0.000000000000001m", "0.000001nm");
    check_forms(length<double>(-0.0), "-0m", "-0m"); // as stored: base and scaled agree
    check_forms(length<double, std::milli>(-0.0), "-0m", "-0m");
    check_forms(length<double, std::milli>(0.0), "0m", "0m");

    // Candidate boundaries
    check_forms(length<double, std::nano>(0.5), "0.0000000005m", "0.5nm"); // smallest candidate
    check_forms(length<int64_t, std::pico>(500), "0.0000000005m", "0.5nm");
    check_forms(nanometers(999), "0.000000999m", "999nm");
    check_forms(nanometers(1000), "0.000001m", "1µm");
    check_forms(micrometers(999), "0.000999m", "999µm");
    check_forms(micrometers(1000), "0.001m", "1mm");
    check_forms(millimeters(999), "0.999m", "999mm");
    check_forms(millimeters(1000), "1m", "1m");
    check_forms(millimeters(-999), "-0.999m", "-999mm");
    check_forms(millimeters(-1000), "-1m", "-1m");
    check_forms(centimeters(99), "0.99m", "990mm");
    check_forms(centimeters(100), "1m", "1m");
    check_forms(meters(999), "999m", "999m");
    check_forms(meters(1000), "1000m", "1km");

    // Areas
    check_forms(square_millimeters(2500000), "2.5m²", "2.5m²");
    check_forms(hectares(3), "30000m²", "30000m²");
    check_forms(hectares(100), "1000000m²", "1km²");
    check_forms(square_centimeters(5), "0.0005m²", "500mm²");
    check_forms(square_kilometers(3), "3000000m²", "3km²");
    check_forms(square_meters(-3), "-3m²", "-3m²");
    check_forms(square_millimeters(0), "0m²", "0m²");
    check_forms(area<double, std::atto>(0.25), "0.00000000000000000025m²", "0.25nm²");
    check_forms(area<int64_t, std::atto>(999999), "0.000000000000999999m²", "999999nm²");
    check_forms(area<int64_t, std::atto>(1000000), "0.000000000001m²", "1µm²");
    check_forms(area<int64_t, std::pico>(999999), "0.000000999999m²", "999999µm²");
    check_forms(area<int64_t, std::pico>(1000000), "0.000001m²", "1mm²");
    check_forms(square_millimeters(999999), "0.999999m²", "999999mm²");
    check_forms(square_millimeters(1000000), "1m²", "1m²");
    check_forms(square_meters(999999), "999999m²", "999999m²");
    check_forms(square_meters(1000000), "1000000m²", "1km²");

    // Volumes
    check_forms(liters(2), "0.002m³", "2L");
    check_forms(milliliters(1500), "0.0015m³", "1.5L");
    check_forms(liters(2000), "2m³", "2m³");
    check_forms(cubic_meters(0), "0m³", "0m³");
    check_forms(liters(-1500), "-1.5m³", "-1.5m³");
    check_forms(milliliters(-999), "-0.000999m³", "-999mL");
    check_forms(cubic_centimeters(5), "0.000005m³", "5mL");
    check_forms(microliters(999), "0.000000999m³", "999µL");
    check_forms(microliters(1000), "0.000001m³", "1mL");
    check_forms(milliliters(999), "0.000999m³", "999mL");
    check_forms(milliliters(1000), "0.001m³", "1L");
    check_forms(liters(999), "0.999m³", "999L");
    check_forms(liters(1000), "1m³", "1m³");
    check_forms(
        volume<int64_t, std::atto>(5), "0.000000000000000005m³", "0.000000005µL"
    ); // µm³: smallest candidate is µL
    check_forms(cubic_meters(100000), "100000m³", "100000m³");
    check_forms(volume<int64_t, std::giga>(2), "2000000000m³", "2000000000m³");

    // Non-metric precisions: exact base value, and their own unit when scaled
    check_forms(feet(6), "1.8288m", "6ft");
    check_forms(inches(-1), "-0.0254m", "-1in");
    check_forms(mils(8), "0.0002032m", "8mil");
    check_forms(miles(1), "1609.344m", "1mi");
    check_forms(nautical_miles(2), "3704m", "2nmi");
    check_forms(acres(1), "4046.8564224m²", "1ac");
    check_forms(square_feet(0), "0m²", "0ft²");
    check_forms(cubic_inches(231), "0.003785411784m³", "231in³");
    check_forms(us_gallons(1), "0.003785411784m³", "1gal");
    check_forms(us_gallons(-5), "-0.01892705892m³", "-5gal");
}

#if CONFIG_SPATIAL_STD_FORMAT
static_assert(std::is_default_constructible_v<std::formatter<meters, char>>);
static_assert(std::is_default_constructible_v<std::formatter<nautical_miles, char>>);
static_assert(!std::is_default_constructible_v<std::formatter<fifth_millimeters, char>>);
static_assert(std::is_default_constructible_v<std::formatter<square_feet, char>>);
static_assert(std::is_default_constructible_v<std::formatter<us_gallons, char>>);
static_assert(!std::is_default_constructible_v<std::formatter<milli_square_meters, char>>);
static_assert(!std::is_default_constructible_v<std::formatter<micro_cubic_meters_x10, char>>);

TEST_CASE("std::format support", "[format]") {
    REQUIRE(std::format("{}", nanometers(850)) == "850nm");
    REQUIRE(std::format("{}", micrometers(50)) == "50µm");
    REQUIRE(std::format("{}", millimeters(250)) == "250mm");
    REQUIRE(std::format("{}", centimeters(30)) == "30cm");
    REQUIRE(std::format("{}", meters(100)) == "100m");
    REQUIRE(std::format("{}", kilometers(5)) == "5km");
    REQUIRE(std::format("{}", mils(8)) == "8mil");
    REQUIRE(std::format("{}", inches(12)) == "12in");
    REQUIRE(std::format("{}", feet(6)) == "6ft");
    REQUIRE(std::format("{}", yards(100)) == "100yd");
    REQUIRE(std::format("{}", miles(26)) == "26mi");
    REQUIRE(std::format("{}", nautical_miles(12)) == "12nmi");
    REQUIRE(std::format("{}", meters(-3)) == "-3m");
    REQUIRE(std::format("trace {} over {}", 8_mil, 30_cm) == "trace 8mil over 30cm");
}

TEST_CASE("std::format with spec keeps the type's own unit", "[format]") {
    // A spec changes only the number formatting, never the unit: every
    // precision renders its own count with the same suffix as "{}".
    REQUIRE(std::format("{:.1f}", nanometers(850)) == "850.0nm");
    REQUIRE(std::format("{:.1f}", 850_nm) == "850.0nm");
    REQUIRE(std::format("{:.1f}", micrometers(50)) == "50.0µm");
    REQUIRE(std::format("{:.1f}", millimeters(1500)) == "1500.0mm");
    REQUIRE(std::format("{:.2f}", centimeters(30)) == "30.00cm");
    REQUIRE(std::format("{:.1f}", meters(3)) == "3.0m");
    REQUIRE(std::format("{:.0f}", kilometers(5)) == "5km");
    REQUIRE(std::format("{:.1f}", mils(8)) == "8.0mil");
    REQUIRE(std::format("{:.1f}", inches(12)) == "12.0in");
    REQUIRE(std::format("{:.1f}", feet(6)) == "6.0ft");
    REQUIRE(std::format("{:.1f}", yards(100)) == "100.0yd");
    REQUIRE(std::format("{:.1f}", miles(26)) == "26.0mi");
    REQUIRE(std::format("{:.1f}", nautical_miles(12)) == "12.0nmi");

    // Other numeric presentations apply to the same count.
    REQUIRE(std::format("{:e}", millimeters(1500)) == "1.500000e+03mm");
    REQUIRE(std::format("{:g}", meters(-3)) == "-3m");

    // Width and alignment pass through to the numeric part.
    REQUIRE(std::format("{:8.1f}", millimeters(1500)) == "  1500.0mm");
    REQUIRE(std::format("{:<6.1f}", feet(6)) == "6.0   ft");

    // Floating-point representations format the same way.
    REQUIRE(std::format("{}", length<double, std::milli>(2.5)) == "2.5mm");
    REQUIRE(std::format("{:.2f}", length<double, std::milli>(25.4)) == "25.40mm");
    REQUIRE(std::format("{:.3f}", length<double>(1.5)) == "1.500m");
}

TEST_CASE("std::format power-form marker", "[format]") {
    // 'p' selects the power-form suffix; the rest of the spec is unchanged.
    REQUIRE(std::format("{}", 24_mL) == "24mL");
    REQUIRE(std::format("{:p}", 24_mL) == "24cm³");
    REQUIRE(std::format("{:.1f}", 24_mL) == "24.0mL");
    REQUIRE(std::format("{:p.1f}", 24_mL) == "24.0cm³");
    REQUIRE(std::format("{:p}", 3_ha) == "3hm²");
    REQUIRE(std::format("{:p}", 2_L) == "2dm³");
    REQUIRE(std::format("{:p}", 50_uL) == "50mm³");

    // Width, alignment and fill after the marker apply to the numeric part.
    REQUIRE(std::format("{:p>10.1f}", 24_mL) == "      24.0cm³");
    REQUIRE(std::format("{:p*<8.1f}", 24_mL) == "24.0****cm³");
    REQUIRE(std::format("{:p8}", 2_L) == "       2dm³");
    REQUIRE(std::format("{:*>8.1f}", 24_mL) == "****24.0mL");

    // A leading 'p' is always the marker, so it cannot be a fill character.
    REQUIRE(std::format("{:p>6}", 24_mL) == "    24cm³");

    // Lengths and suffixes without a power form are unaffected by the marker.
    REQUIRE(std::format("{:p}", 250_mm) == "250mm");
    REQUIRE(std::format("{:p.1f}", feet(6)) == "6.0ft");
    REQUIRE(std::format("{:p}", 40_ac) == "40ac");
    REQUIRE(std::format("{:p}", 5_gal) == "5gal");

    // Floating-point representations and computed quantities.
    REQUIRE(std::format("{:p}", volume<double, std::micro>(2.5)) == "2.5cm³");
    REQUIRE(std::format("{:.2f}", area<double>(1.5)) == "1.50m²");
    REQUIRE(std::format("{}", 3_ft * 4_ft) == "12ft²");
    REQUIRE(std::format("{:p}", 10_cm * 10_cm * 10_cm) == "1000cm³");
}

TEST_CASE("std::format base-unit and scaled markers", "[format]") {
    // Without a marker the unit is the type's own, whatever the spec.
    REQUIRE(std::format("{}", millimeters(1500)) == "1500mm");
    REQUIRE(std::format("{:.1f}", millimeters(1500)) == "1500.0mm");

    // With a trailing spec, the spec applies to the converted value.
    REQUIRE(std::format("{:b.1f}", millimeters(1500)) == "1.5m");
    REQUIRE(std::format("{:b.4f}", feet(6)) == "1.8288m");
    REQUIRE(std::format("{:b.3e}", liters(2)) == "2.000e-03m³");
    REQUIRE(std::format("{:s.2f}", millimeters(1500)) == "1.50m");
    REQUIRE(std::format("{:s.1f}", milliliters(1500)) == "1.5L");
    REQUIRE(std::format("{:s.1f}", micrometers(-2500)) == "-2.5mm");
    REQUIRE(std::format("{:s.1f}", feet(6)) == "6.0ft");
    REQUIRE(std::format("{:s.0f}", cubic_meters(0)) == "0m³");

    // Width, alignment and fill after the marker apply to the numeric part.
    REQUIRE(std::format("{:b*>8}", millimeters(1500)) == "*****1.5m");
    REQUIRE(std::format("{:b>8.2f}", millimeters(1500)) == "    1.50m");
    REQUIRE(std::format("{:s<6.1f}", milliliters(1500)) == "1.5   L");
    REQUIRE(std::format("{:s_^7}", square_millimeters(2500000)) == "__2.5__m²");

    // A leading 'b' or 's' is always the marker, never a fill character.
    REQUIRE(std::format("{:s>6}", liters(2000)) == "     2m³");
    REQUIRE(std::format("{:b>6}", meters(2)) == "     2m");

    // Only one marker: a second one reaches the numeric spec and is an error.
    millimeters mm{1500};
    REQUIRE_THROWS_AS(std::vformat("{:sb}", std::make_format_args(mm)), std::format_error);
    REQUIRE_THROWS_AS(std::vformat("{:pb}", std::make_format_args(mm)), std::format_error);

    // The tagged to_string overloads agree exactly with the markers.
    REQUIRE(std::format("{:b}", us_gallons(7)) == to_string(us_gallons(7), base_unit));
    REQUIRE(std::format("{:s}", square_inches(3)) == to_string(square_inches(3), scaled));
    REQUIRE(std::format("{:s}", length<double>(1.0 / 3.0)) == to_string(length<double>(1.0 / 3.0), scaled));
    REQUIRE(
        std::format("{:b}", length<double, std::milli>(0.1)) == to_string(length<double, std::milli>(0.1), base_unit)
    );
}
#endif

TEST_CASE("PCB trace example", "[integration]") {
    // An 8 mil trace with 6 mil clearance on a 1.6 mm board
    auto trace = 8_mil;
    auto clearance = 6_mil;
    auto pitch = trace + clearance;
    REQUIRE(pitch == mils(14));
    REQUIRE(round<micrometers>(pitch) == micrometers(356)); // 355.6 µm

    // Board thickness in mils (1.6 mm = 62.99 mil)
    auto board = 1600_um;
    REQUIRE(round<mils>(board) == mils(63));
}

TEST_CASE("route length example", "[integration]") {
    // A marathon is 42.195 km, about 26.2 miles
    auto marathon = kilometers(42) + meters(195);
    REQUIRE(marathon == meters(42195));
    REQUIRE(floor<miles>(marathon) == miles(26));
    auto remainder = marathon - floor<miles>(marathon);
    REQUIRE(round<yards>(remainder) == yards(385));
}
