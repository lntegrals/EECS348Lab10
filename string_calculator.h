// EECS 348: Software Engineering I – Fall 2025
// Lab 10 – String-based calculator
//
// This header declares helper routines for validating and performing
// arithmetic on floating point values represented as strings.  The
// functions defined here operate entirely on std::string objects and
// never convert the values to native numeric types.  They are
// intentionally self‑contained and avoid using third‑party libraries
// outside of the C++ standard library.  See string_calculator.cpp for
// implementations and additional comments.

#ifndef STRING_CALCULATOR_H
#define STRING_CALCULATOR_H

#include <string>
#include <utility>

// Check whether the provided string represents a valid double‑precision
// floating point literal according to the assignment specification.  A
// valid string may begin with an optional '+' or '-' sign.  It must
// contain at least one digit.  An optional decimal point may appear,
// provided that there is at least one digit on both sides of the
// decimal point.  Leading zeros are permitted.  Examples of valid
// inputs: "1", "1.0", "+0001.23", "-0.5".  Examples of invalid
// inputs: "A", "+-1", "5.", ".5", "1..0".
bool isValidDouble(const std::string &s);

// Normalize the representation of a numeric string.  Leading zeros in
// the integer portion are removed (except when the number is zero),
// and trailing zeros in the fractional portion are trimmed.  If the
// fractional portion becomes empty after trimming zeros, the decimal
// point is removed entirely.  The sign is preserved.  For example,
// input "+00012.3400" becomes "+12.34", while "-000.000" becomes
// "0".  This routine may be called internally by the arithmetic
// functions but is provided here for completeness.
std::string normalize(const std::string &s);

// Compare the absolute values of two normalized numeric strings
// (without signs) representing non‑negative decimal numbers.  Returns
// -1 if lhs < rhs, 0 if equal, and 1 if lhs > rhs.  Both strings
// should already conform to the valid format checked by
// isValidDouble().  The comparison is performed without converting
// them to numeric types.
int compareMagnitude(const std::string &lhs, const std::string &rhs);

// Add two valid numeric strings and return the resulting sum in
// normalized form.  The inputs must conform to the specification of
// isValidDouble() and may include signs and decimal points.  The
// returned string will include a leading '-' sign when appropriate.
std::string add(const std::string &a, const std::string &b);

#endif // STRING_CALCULATOR_H