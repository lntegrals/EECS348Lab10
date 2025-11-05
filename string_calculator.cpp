// EECS 348: Software Engineering I – Fall 2025
// Lab 10 – String-based calculator
//
// This file contains the implementations for the routines declared in
// string_calculator.h.  The code avoids using built‑in floating point
// types and instead performs all arithmetic and validation using
// std::string and simple character operations.

#include "string_calculator.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>

// Helper to split a normalized numeric string (without sign) into
// integer and fractional parts.  If no decimal point exists, the
// fractional part will be empty.  Returns a pair {intPart, fracPart}.
static std::pair<std::string, std::string> splitParts(const std::string &s) {
    std::string::size_type pos = s.find('.');
    if (pos == std::string::npos) {
        return {s, ""};
    }
    return {s.substr(0, pos), s.substr(pos + 1)};
}

bool isValidDouble(const std::string &s) {
    if (s.empty()) {
        return false;
    }
    std::size_t i = 0;
    // Optional sign
    if (s[i] == '+' || s[i] == '-') {
        i++;
        if (i == s.size()) {
            // String cannot be just a sign
            return false;
        }
    }
    bool hasDot = false;
    int digitsBeforeDot = 0;
    int digitsAfterDot = 0;
    for (; i < s.size(); ++i) {
        char c = s[i];
        if (std::isdigit(static_cast<unsigned char>(c))) {
            if (hasDot) {
                digitsAfterDot++;
            } else {
                digitsBeforeDot++;
            }
        } else if (c == '.') {
            if (hasDot) {
                // Multiple decimal points
                return false;
            }
            hasDot = true;
            // We don't increment digit counters here; digitsAfterDot will be counted after dot
        } else {
            // Invalid character encountered
            return false;
        }
    }
    // At least one digit must appear before any dot
    if (digitsBeforeDot == 0) {
        return false;
    }
    // If decimal point is present, there must be at least one digit after it
    if (hasDot && digitsAfterDot == 0) {
        return false;
    }
    return true;
}

// Remove leading zeros in the integer part and trailing zeros in the
// fractional part.  Preserve sign and decimal point when needed.
std::string normalize(const std::string &s) {
    if (s.empty()) {
        return "";
    }
    // Determine and remove sign
    char sign = 0;
    std::size_t pos = 0;
    if (s[0] == '+' || s[0] == '-') {
        sign = s[0];
        pos = 1;
    }
    std::string unsignedPart = s.substr(pos);
    // Split unsigned part into integer and fractional components
    auto parts = splitParts(unsignedPart);
    std::string intPart = parts.first;
    std::string fracPart = parts.second;
    // Remove leading zeros in integer part
    std::size_t firstNonZero = 0;
    while (firstNonZero < intPart.size() && intPart[firstNonZero] == '0') {
        firstNonZero++;
    }
    if (firstNonZero == intPart.size()) {
        // Entire integer part is zeros
        intPart = "0";
    } else {
        intPart = intPart.substr(firstNonZero);
    }
    // Remove trailing zeros in fractional part
    std::size_t lastNonZero = fracPart.size();
    while (lastNonZero > 0 && fracPart[lastNonZero - 1] == '0') {
        lastNonZero--;
    }
    if (lastNonZero == 0) {
        fracPart.clear();
    } else {
        fracPart = fracPart.substr(0, lastNonZero);
    }
    // If fractional part exists, keep the decimal point
    std::string result;
    if (!fracPart.empty()) {
        result = intPart + "." + fracPart;
    } else {
        result = intPart;
    }
    // Determine sign: do not preserve negative sign if result is zero
    bool isZero = (result == "0");
    if (!isZero && sign == '-') {
        result.insert(result.begin(), '-');
    } else if (!isZero && sign == '+') {
        // Optional '+' is not preserved; positive numbers are returned without '+'
    }
    // For zero, return "0" without any sign
    return result;
}

// Compare magnitudes of two normalized numbers (no sign).  Return -1
// if lhs < rhs, 0 if equal, 1 if lhs > rhs.
int compareMagnitude(const std::string &lhs, const std::string &rhs) {
    auto lhsParts = splitParts(lhs);
    auto rhsParts = splitParts(rhs);
    // Compare integer part lengths
    if (lhsParts.first.size() != rhsParts.first.size()) {
        return (lhsParts.first.size() < rhsParts.first.size()) ? -1 : 1;
    }
    // Lengths equal; lexicographically compare integer parts
    if (lhsParts.first != rhsParts.first) {
        return (lhsParts.first < rhsParts.first) ? -1 : 1;
    }
    // Integer parts equal; compare fractional parts by padding zeros
    std::size_t maxFracLen = std::max(lhsParts.second.size(), rhsParts.second.size());
    for (std::size_t i = 0; i < maxFracLen; ++i) {
        char lhsDigit = (i < lhsParts.second.size()) ? lhsParts.second[i] : '0';
        char rhsDigit = (i < rhsParts.second.size()) ? rhsParts.second[i] : '0';
        if (lhsDigit != rhsDigit) {
            return (lhsDigit < rhsDigit) ? -1 : 1;
        }
    }
    return 0;
}

// Add magnitudes of two non‑negative decimal strings (without sign).
static std::string addMagnitudes(const std::string &aAbs, const std::string &bAbs) {
    auto aParts = splitParts(aAbs);
    auto bParts = splitParts(bAbs);
    std::string aInt = aParts.first;
    std::string bInt = bParts.first;
    std::string aFrac = aParts.second;
    std::string bFrac = bParts.second;
    // Equalize fractional lengths by padding zeros to the right
    std::size_t maxFracLen = std::max(aFrac.size(), bFrac.size());
    aFrac.append(maxFracLen - aFrac.size(), '0');
    bFrac.append(maxFracLen - bFrac.size(), '0');
    // Equalize integer lengths by padding zeros to the left
    std::size_t maxIntLen = std::max(aInt.size(), bInt.size());
    aInt.insert(aInt.begin(), maxIntLen - aInt.size(), '0');
    bInt.insert(bInt.begin(), maxIntLen - bInt.size(), '0');
    // Prepare result containers
    std::string resultFrac(maxFracLen, '0');
    std::string resultInt(maxIntLen, '0');
    int carry = 0;
    // Add fractional part from right to left
    for (int i = static_cast<int>(maxFracLen) - 1; i >= 0; --i) {
        int sum = (aFrac[i] - '0') + (bFrac[i] - '0') + carry;
        resultFrac[i] = static_cast<char>('0' + (sum % 10));
        carry = sum / 10;
    }
    // Add integer part from right to left
    for (int i = static_cast<int>(maxIntLen) - 1; i >= 0; --i) {
        int sum = (aInt[i] - '0') + (bInt[i] - '0') + carry;
        resultInt[i] = static_cast<char>('0' + (sum % 10));
        carry = sum / 10;
    }
    // If there's leftover carry, prepend it to integer result
    if (carry > 0) {
        resultInt.insert(resultInt.begin(), static_cast<char>('0' + carry));
    }
    // Assemble the result
    std::string result;
    if (maxFracLen > 0) {
        result = resultInt + "." + resultFrac;
    } else {
        result = resultInt;
    }
    return normalize(result);
}

// Subtract magnitudes: compute (aAbs - bAbs) where aAbs >= bAbs.  Both
// strings should represent non‑negative decimal numbers without sign.
static std::string subtractMagnitudes(const std::string &aAbs, const std::string &bAbs) {
    auto aParts = splitParts(aAbs);
    auto bParts = splitParts(bAbs);
    std::string aInt = aParts.first;
    std::string bInt = bParts.first;
    std::string aFrac = aParts.second;
    std::string bFrac = bParts.second;
    // Equalize fractional lengths by padding zeros to the right
    std::size_t maxFracLen = std::max(aFrac.size(), bFrac.size());
    aFrac.append(maxFracLen - aFrac.size(), '0');
    bFrac.append(maxFracLen - bFrac.size(), '0');
    // Equalize integer lengths by padding zeros to the left
    std::size_t maxIntLen = std::max(aInt.size(), bInt.size());
    aInt.insert(aInt.begin(), maxIntLen - aInt.size(), '0');
    bInt.insert(bInt.begin(), maxIntLen - bInt.size(), '0');
    // Prepare result containers
    std::string resultFrac(maxFracLen, '0');
    std::string resultInt(maxIntLen, '0');
    int borrow = 0;
    // Subtract fractional part from right to left
    for (int i = static_cast<int>(maxFracLen) - 1; i >= 0; --i) {
        int diff = (aFrac[i] - '0') - (bFrac[i] - '0') - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultFrac[i] = static_cast<char>('0' + diff);
    }
    // Subtract integer part from right to left
    for (int i = static_cast<int>(maxIntLen) - 1; i >= 0; --i) {
        int diff = (aInt[i] - '0') - (bInt[i] - '0') - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultInt[i] = static_cast<char>('0' + diff);
    }
    // Remove any leading zeros from integer part will be handled by normalize
    std::string result;
    if (maxFracLen > 0) {
        result = resultInt + "." + resultFrac;
    } else {
        result = resultInt;
    }
    return normalize(result);
}

std::string add(const std::string &a, const std::string &b) {
    // Normalize inputs first to simplify sign handling and zero detection
    std::string na = normalize(a);
    std::string nb = normalize(b);
    // Early return if one of the numbers is zero
    if (na == "0") {
        return nb;
    }
    if (nb == "0") {
        return na;
    }
    // Extract sign
    bool aNeg = false;
    bool bNeg = false;
    std::string aAbs = na;
    std::string bAbs = nb;
    if (na[0] == '-') {
        aNeg = true;
        aAbs = na.substr(1);
    }
    if (nb[0] == '-') {
        bNeg = true;
        bAbs = nb.substr(1);
    }
    if (aNeg == bNeg) {
        // Same sign: add magnitudes and preserve sign if negative
        std::string sumMag = addMagnitudes(aAbs, bAbs);
        if (aNeg) {
            // Both negative; result should carry '-' unless zero
            if (sumMag != "0") {
                sumMag.insert(sumMag.begin(), '-');
            }
        }
        return sumMag;
    } else {
        // Different signs: subtract smaller magnitude from larger
        int cmp = compareMagnitude(aAbs, bAbs);
        if (cmp == 0) {
            // Magnitudes equal -> result is zero
            return "0";
        } else if (cmp > 0) {
            // |a| > |b| -> result sign is sign of a
            std::string diff = subtractMagnitudes(aAbs, bAbs);
            if (aNeg) {
                if (diff != "0") {
                    diff.insert(diff.begin(), '-');
                }
            }
            return diff;
        } else {
            // |b| > |a| -> result sign is sign of b
            std::string diff = subtractMagnitudes(bAbs, aAbs);
            if (bNeg) {
                if (diff != "0") {
                    diff.insert(diff.begin(), '-');
                }
            }
            return diff;
        }
    }
}