/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_util.h
 * @brief Caffeine Framework Universal Utilities
 */

#ifndef CAFFEINE_HAL_CFN_HAL_UTIL_H
#define CAFFEINE_HAL_CFN_HAL_UTIL_H
#include <stdint.h>
#include <stdbool.h>

/* --- 1. Multi-Byte Reconstruction (Endianness) --- */

/**
 * @brief Reconstructs a signed 16-bit integer from two unsigned bytes (Little Endian).
 * @details This function prevents signed integer promotion errors during shifts.
 * * @param[in] msb Most Significant Byte.
 * @param[in] lsb Least Significant Byte.
 * @return int16_t The reconstructed signed 16-bit value.
 */
static inline int16_t cfn_util_bytes_to_int16_le(uint8_t msb, uint8_t lsb)
{
    /* Use 32-bit unsigned math internally to prevent signed promotion during shifts */
    const uint32_t COMBINED = ((uint32_t) msb << 8U) | (uint32_t) lsb;
    return (int16_t) COMBINED;
}

/**
 * @brief Reconstructs an unsigned 32-bit integer from four bytes (Little Endian).
 * * @param[in] b3 Byte 3 (Most Significant).
 * @param[in] b2 Byte 2.
 * @param[in] b1 Byte 1.
 * @param[in] b0 Byte 0 (Least Significant).
 * @return uint32_t The reconstructed unsigned 32-bit value.
 */
static inline uint32_t cfn_util_bytes_to_uint32_le(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    return ((uint32_t) b3 << 24U) | ((uint32_t) b2 << 16U) | ((uint32_t) b1 << 8U) | (uint32_t) b0;
}

/**
 * @brief Reconstructs a signed 32-bit integer from four bytes (Big Endian).
 * * @param[in] b0 Byte 0 (Most Significant).
 * @param[in] b1 Byte 1.
 * @param[in] b2 Byte 2.
 * @param[in] b3 Byte 3 (Least Significant).
 * @return int32_t The reconstructed signed 32-bit value.
 */
static inline int32_t cfn_util_bytes_to_int32_be(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
    const uint32_t COMBINED = ((uint32_t) b0 << 24U) | ((uint32_t) b1 << 16U) | ((uint32_t) b2 << 8U) | (uint32_t) b3;
    return (int32_t) COMBINED;
}

/* --- 2. Bitwise Manipulation (Explicit Logic) --- */

/**
 * @brief Extracts a bitfield, shifts it to the LSB, and returns it.
 * @param[in] val   The source register value.
 * @param[in] mask  The bitmask for the field.
 * @param[in] shift The number of positions to shift right.
 * @return uint32_t The extracted and shifted field.
 */
static inline uint32_t cfn_util_extract_field(uint32_t val, uint32_t mask, uint32_t shift)
{
    /* Mask first, then shift down so the result fits in the smallest possible type */
    return (uint32_t) ((val & mask) >> shift);
}

/**
 * @brief Checks if a specific bit is set in an 8-bit register.
 * * @param[in] reg_val  The raw register value.
 * @param[in] bit_mask The mask of the bit to check.
 * @return true if the bit is set, false otherwise.
 */
static inline bool cfn_util_bit_is_set(uint8_t reg_val, uint8_t bit_mask)
{
    return (bool) ((reg_val & bit_mask) != 0U);
}

/* --- 3. Saturated & Safe Arithmetic --- */

/**
 * @brief Saturated addition for signed 16-bit integers.
 * @details Prevents roll-over by clipping at INT16_MAX (32767) or INT16_MIN (-32768).
 * * @param[in] a First operand.
 * @param[in] b Second operand.
 * @return int16_t The saturated result of (a + b).
 */
static inline int16_t cfn_util_sat_add_s16(int16_t a, int16_t b)
{
    const int32_t RESULT = (int32_t) a + (int32_t) b;
    int16_t       out;
    if (RESULT > 32767)
    {
        out = 32767;
    }
    else if (RESULT < -32768)
    {
        out = -32768;
    }
    else
    {
        out = (int16_t) RESULT;
    }
    return out;
}

/**
 * @brief Clamps a float value within a specified range.
 * * @param[in] val The value to clamp.
 * @param[in] min The minimum allowable value.
 * @param[in] max The maximum allowable value.
 * @return float  The clamped value.
 */
static inline float cfn_util_clamp_f32(float val, float min, float max)
{
    float res = val;
    if (val < min)
    {
        res = min;
    }
    else if (val > max)
    {
        res = max;
    }
    return res;
}

/* --- 4. Floating Point Utilities --- */

/**
 * @brief Compares two floats for equality within a small epsilon.
 * * @param[in] a       First operand.
 * @param[in] b       Second operand.
 * @param[in] epsilon The maximum difference for equality.
 * @return true if the difference is less than epsilon, false otherwise.
 */
static inline bool cfn_util_f32_is_equal(float a, float b, float epsilon)
{
    float diff = a - b;
    if (diff < 0.0F)
    {
        diff = -diff;
    }
    return (bool) (diff <= epsilon);
}

/* --- 5. Conversion Utilities --- */

/**
 * @brief Converts a float to Q1.15 fixed-point format.
 * @note Assumes input is in range [-1.0, 1.0].
 * * @param[in] val The float value to convert.
 * @return int16_t The Q1.15 representation.
 */
static inline int16_t cfn_util_f32_to_q15(float val)
{
    const float SCALED = val * 32767.0F;
    return (int16_t) SCALED;
}

/**
 * @brief Extracts the Most Significant Byte from a 16-bit value.
 * @param[in] val The 16-bit source value.
 * @return uint8_t The MSB.
 */
static inline uint8_t cfn_util_get_msb16(uint16_t val)
{
    return (uint8_t) (((uint32_t) val >> 8U) & 0xFFU);
}

/**
 * @brief Extracts the Least Significant Byte from a 16-bit value.
 * @param[in] val The 16-bit source value.
 * @return uint8_t The LSB.
 */
static inline uint8_t cfn_util_get_lsb16(uint16_t val)
{
    return (uint8_t) ((uint32_t) val & 0xFFU);
}

#endif // CAFFEINE_HAL_CFN_HAL_UTIL_H