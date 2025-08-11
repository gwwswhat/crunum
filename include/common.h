#ifndef CRUNUM_COMMON_H
#define CRUNUM_COMMON_H

#define NEAR_ZERO 1e-6
#define NEON_ALIGNMENT 16

typedef unsigned int uint;
typedef unsigned long ulong;

#if HAVE_NEON
#include <arm_neon.h>

static inline float p_vaddvq_f32(float32x4_t v){
#if defined(__aarch64__)
	return vaddvq_f32(v);
#else
	float32x2_t vtemp = vadd_f32(vget_low_f32(v), vget_high_f32(v));
	return vget_lane_f32(vtemp, 0) + vget_lane_f32(vtemp, 1);
#endif
}

static inline uint is_lanes_eq(float32x4_t v1, float32x4_t v2){
	return vmaxvq_u32(vceqq_f32(v1, v2));
}

static inline uint is_lanes_neq(float32x4_t v1, float32x4_t v2){
	return !is_lanes_eq(v1, v2);
}

static inline uint is_lanes_gt(float32x4_t v1, float32x4_t v2){
	return vmaxvq_u32(vcgtq_f32(v1, v2));
}

static inline uint is_lanes_ge(float32x4_t v1, float32x4_t v2){
	return vmaxvq_u32(vcgeq_f32(v1, v2));
}

static inline uint is_lanes_lt(float32x4_t v1, float32x4_t v2){
	return vmaxvq_u32(vcltq_f32(v1, v2));
}

static inline uint is_lanes_le(float32x4_t v1, float32x4_t v2){
	return vmaxvq_u32(vcleq_f32(v1, v2));
}

static inline uint any_lane_is(float32x4_t v, float scalar){
	return is_lanes_eq(v, vdupq_n_f32(scalar));
}

static inline uint any_lane_is_zero(float32x4_t v){
	return any_lane_is(v, 0.0f);
}

#endif

#endif
