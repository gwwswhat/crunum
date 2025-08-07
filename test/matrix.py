import sys
sys.path = ['']

import crunum as crn

mat1 = crn.matrix.new(3, 3)

assert mat1 == 0, f"mat1 isn't zero, error={mat1}"

mat1.set(0, 0, 2.2)
mat1.set(2, 1, 3.4)
mat1.set(1, 0, 11.55)

assert mat1 == crn.matrix.from_list([[2.2, 0, 0], [11.55, 0, 0], [0, 3.4, 0]]), f"should be [[2.2, 0, 0], [11.55, 0, 0], [0, 3.4, 0]], error={mat1}"

assert mat1.rows == 3, f"mat2 row size isn't 3, error={mat2.rows}"
assert mat1.cols == 3, f"mat2 col size isn't 3, error={mat2.rows}"

fibo = crn.matrix.new(2, 2)

fibo.set(0, 0, 1)
fibo.set(0, 1, 1)
fibo.set(1, 0, 1)

assert fibo == crn.matrix.from_list([[1, 1], [1, 0]]), f"wrong base fibonacci, error={fibo}"

res_fibo = fibo ** 5

assert res_fibo == crn.matrix.from_list([[8, 5], [5, 3]]), f"wrong result of F(5) fibonacci, error={res_fibo}"

base = crn.matrix.new(2, 2)

base.set(0, 0, 1)
base.set(0, 1, 2)
base.set(1, 0, 3)
base.set(1, 1, 4)

assert base == crn.matrix.from_list([[1, 2], [3, 4]]), f"should be [[1, 2], [3, 4]], error={base}"

res_add = base + base
res_sub = base - base
res_mul = base * base
res_div = base / base

assert res_add == crn.matrix.from_list([[2, 4], [6, 8]]), f"should be [[2, 4], [6, 8]], error={res_add}"
assert res_sub == 0, f"should be all zero, error={res_add}"
assert res_mul == crn.matrix.from_list([[7, 10], [15, 22]]), f"should be [[7, 10], [15, 22]], error={res_mul}"
assert res_div == 1, f"should be all 1, error={res_div}"

res_add_scalar = base + 2
res_sub_scalar = base - 5
res_mul_scalar = base * 3
res_div_scalar = base / 2

assert res_add_scalar == crn.matrix.from_list([[3, 4], [5, 6]]), f"should be [[3, 4], [5, 6]], error={res_add_scalar}"
assert res_sub_scalar == crn.matrix.from_list([[-4, -3], [-2, -1]]), f"should be [[-4, -3], [-2, -1]], error={res_sub_scalar}"
assert res_mul_scalar == crn.matrix.from_list([[3, 6], [9, 12]]), f"should be [[3, 6], [9, 12]], error={res_mul_scalar}"
assert res_div_scalar == crn.matrix.from_list([[0.5, 1], [1.5, 2]]), f"should be [[0.5, 1], [1.5, 2]], error={res_div_scalar}"

print("[SUCCESS]")
