local crunum = require "crunum"

local mat1 = crunum.matrix.from({
	{3.3, 2.2, 1.5},
	{2.2, 1.5, 0.2}
})

local mat2 = crunum.matrix.from({
	{-2.2, 1.51}, 
	{4.5, 34},
	{-34.5, -2.3}
})

local mat3 = mat1 * mat2

print(mat1)

print(mat2)

print(mat3)

local mat4 = crunum.matrix.randinit(1, 100, 30.0, 100.0)

print(mat4)

print(mat4:rows())
print(mat4:cols())

mat2[1][1] = 2.0

print("Matrix 2: ", mat2)
