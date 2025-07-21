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

local mat4 = crunum.matrix.randinit(1, 100)

print(mat4)

print(mat4:rows())
print(mat4:cols())

mat2:set(1, 1, 2.0)

print("Matrix 2: ", mat2)

mat4:reshape(10, 10)

print("Reshape: ", mat4)

mat3:push_row(crunum.vector.randinit(mat3:cols()))

print(mat3)

local vec = crunum.vector.new(0)

vec:push(2)
vec:push(5)
vec:push(6)

print(vec)

print("Last number: ", vec:pop())

print(vec)

local mat = crunum.matrix.new(0, 0)

mat:push_row(crunum.vector.from({3, 3, 3}))

print(mat)

mat:pop_row()

print(mat)
