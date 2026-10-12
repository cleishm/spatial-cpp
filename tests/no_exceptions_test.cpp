// Regression test: the std::formatter specialization must be usable when
// compiled with exceptions disabled (-fno-exceptions), as on embedded targets
// such as ESP-IDF with CONFIG_COMPILER_CXX_EXCEPTIONS unset.
//
// This is a separate target from spatial_tests because Catch2 requires
// exceptions. It deliberately avoids any test framework so the whole
// translation unit builds without them.

#include <cstdio>
#include <spatial/spatial>
#include <string>

#ifdef __cpp_exceptions
#error "this test must be compiled with exceptions disabled"
#endif

#if !CONFIG_SPATIAL_STD_FORMAT

// Without std::format the formatter is not compiled and there is nothing to
// test; older standard libraries (e.g. g++-12's) lack <format> entirely.
int main() {
    std::printf("std::format unavailable; nothing to test\n");
    return 0;
}

#else

using namespace spatial;

namespace {

int failures = 0;

void check(const std::string& actual, const std::string& expected, const char* what) {
    if (actual != expected) {
        std::printf("FAIL: %s: expected \"%s\", got \"%s\"\n", what, expected.c_str(), actual.c_str());
        ++failures;
    }
}

} // namespace

int main() {
    // Formatting with no spec: instantiating parse() must not pull in a throw.
    check(std::format("{}", meters(50)), "50m", "{} meters");
    check(std::format("{}", millimeters(1500)), "1500mm", "{} millimeters");
    check(std::format("{}", centimeters(30)), "30cm", "{} centimeters");
    check(std::format("{}", micrometers(25)), "25µm", "{} micrometers");
    check(std::format("{}", feet(6)), "6ft", "{} feet");
    check(std::format("{}", nautical_miles(12)), "12nmi", "{} nautical_miles");

    // Formatting with an explicit spec.
    check(std::format("{:.1f}", millimeters(1500)), "1500.0mm", "{:.1f} millimeters");
    check(std::format("{:.1f}", kilometers(5)), "5.0km", "{:.1f} kilometers");
    check(std::format("{:.1f}", inches(12)), "12.0in", "{:.1f} inches");
    check(std::format("{:.1f}", mils(8)), "8.0mil", "{:.1f} mils");

    // Areas and volumes, with and without the power-form marker.
    check(std::format("{}", square_feet(12)), "12ft²", "{} square_feet");
    check(std::format("{}", milliliters(24)), "24mL", "{} milliliters");
    check(std::format("{:p}", milliliters(24)), "24cm³", "{:p} milliliters");
    check(std::format("{:.1f}", hectares(3)), "3.0ha", "{:.1f} hectares");
    check(std::format("{:p.1f}", hectares(3)), "3.0hm²", "{:p.1f} hectares");
    check(std::format("{:p>8.1f}", liters(2)), "     2.0dm³", "{:p>8.1f} liters");
    check(to_string(liters(2), power_form), "2dm³", "to_string(liters, power_form)");

    // Base-unit and scaled markers, and the matching to_string tags.
    check(std::format("{:b}", feet(6)), "1.8288m", "{:b} feet");
    check(std::format("{:b.1f}", millimeters(1500)), "1.5m", "{:b.1f} millimeters");
    check(std::format("{:s}", milliliters(1500)), "1.5L", "{:s} milliliters");
    check(std::format("{:s>6.1f}", micrometers(-2500)), "  -2.5mm", "{:s>6.1f} micrometers");
    check(std::format("{:s}", feet(6)), "6ft", "{:s} feet");
    check(to_string(hectares(3), base_unit), "30000m²", "to_string(hectares, base_unit)");
    check(to_string(liters(2000), scaled), "2m³", "to_string(liters, scaled)");
    check(std::format("{:b}", milliliters(24)), "0.000024m³", "{:b} milliliters");
    check(std::format("{}", length<double>(1e6)), "1000000m", "{} length<double>");
    check(to_string(length<double>(1e6)), "1000000m", "to_string(length<double>)");

    if (failures != 0) {
        std::printf("%d check(s) failed\n", failures);
        return 1;
    }
    std::printf("all checks passed\n");
    return 0;
}

#endif // CONFIG_SPATIAL_STD_FORMAT
