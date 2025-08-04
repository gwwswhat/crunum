import sys
import pydoc
import subprocess
sys.path = ['']

import crunum as crn

print(f"Version: {crn.__version__}")

vec = crn.vector.new(10)

print(vec)

empty_vec = crn.vector.new(0)

empty_vec.push(5)
empty_vec.push(6)
empty_vec.push(11.4)

print(empty_vec)

print(empty_vec.pop())

print(empty_vec)
