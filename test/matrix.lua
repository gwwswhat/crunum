package.cpath = "./?.so"

local cn = require "crunum"

local mat1 = cn.matrix.new(10, 2)
local mat2 = cn.matrix.randinit(2, 10)

print("Matrix 1: ", mat1)
print("Matrix 2: ", mat2)

mat1:set(1, 1, 2)
mat1:set(1, 2, 67)
mat1:set(3, 1, 77)

print("Matrix 1(new): ", mat1)
print("matrix + matrix: ", mat1 + mat2)
print("matrix * matrix: ", mat1 * mat2)
print("matrix + scalar(42): ", mat1 + 42)
print("matrix * scalar(33): ", mat1 * 33)

print(mat1:rows())
print(mat2:cols())

local empty_mat = cn.matrix.new(0, 0)

print("Empty matrix: ", empty_mat)

empty_mat:push_row(cn.vector.randinit(4))
empty_mat:push_row(cn.vector.randinit(4))
empty_mat:push_row(cn.vector.randinit(4))

print("Pushed matrix: ", empty_mat)

print("Last row: ", empty_mat:pop_row())

local fibo = cn.matrix.new(2, 2)
fibo:set(1, 1, 1)
fibo:set(1, 2, 1)
fibo:set(2, 1, 1)

print("First: ", fibo)

print("F(5): ", fibo ^ 5)

print("Transpose: ", empty_mat:transpose())

local rand_mat = cn.matrix.randinit(1, 100)

print("Before: ", rand_mat)

rand_mat:reshape(10, 10)

print("Reshape: ", rand_mat)

rand_mat:inverse()

print("Inverse of reshaped matrix: ", rand_mat)

print("Identity matrix: ", cn.matrix.identity(10))
