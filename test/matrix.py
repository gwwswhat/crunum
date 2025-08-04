import sys
import pydoc
import subprocess
sys.path = ['']

import crunum as crn

print(f"Version: {crn.__version__}")

mat1 = crn.matrix.new(10, 10)

print(mat1)

print(crn.matrix.randinit(10, 10))

print(f"Rows: {mat1.rows}, Cols: {mat1.cols}")
