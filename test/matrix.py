import sys
import pydoc
import subprocess
sys.path = ['']

import crunum as crn

print(f"Version: {crn.__version__}")

mat1 = crn.matrix.new(10, 10)

print(mat1)

mat1.set(0, 0, 2.2)
mat1.set(2, 3, 3.4)
mat1.set(5, 6, 11.55)

print(mat1)

mat2 = crn.matrix.randinit(10, 10)

print(mat2)

print(f"Rows: {mat1.rows}, Cols: {mat1.cols}")

print(f"Matrix 1 * Matrix 2: {mat1 * mat2}")

print(f"Identity: {crn.matrix.identity(2)}")

fibo = crn.matrix.new(2, 2)

fibo.set(0, 0, 1)
fibo.set(0, 1, 1)
fibo.set(1, 0, 1)

print(f"F(5): {fibo ** 5}")
