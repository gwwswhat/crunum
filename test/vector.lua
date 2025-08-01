package.cpath = "./?.so"

local cn = require "crunum"

local vec1 = cn.vector.new(10)

print("Vector 1: ", vec1)

local vec2 = cn.vector.randinit(10)

print("Vector 2: ", vec2)

vec1[1] = 1.1
vec1[2] = 2.2
vec1[3] = 3.3
vec1[4] = 4.4
vec1[5] = 5.5
vec1[6] = 6.6
vec1[7] = 7.7
vec1[8] = 8.8
vec1[9] = 9.9
vec1[10] = 10.1

print("Changed Vector 1: ", vec1)

print("vec1 + vec2: ", vec1 + vec2)

print("vec1 * vec2: ", vec1 * vec2)

print("vec1 + 2.4: ", vec1 + 2.4)

print("vec2 * 3.4: ", vec2 * 3.4)

local empty_vec = cn.vector.new(0)

print("Empty Vector: ", empty_vec)

empty_vec:push(5)
empty_vec:push(2.3)
empty_vec:push(44)

print("Push: ", empty_vec)

print("Last number: ", empty_vec:pop())
