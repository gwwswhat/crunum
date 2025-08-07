import sys
sys.path = ['']

import crunum as crn

vec1 = crn.vector.new(2)

assert vec1 == 0, f"should be all 0, error={vec1}"

vec1[0] = 2.2
vec1[1] = 3.3

assert vec1 == crn.vector.from_list([2.2, 3.3]), f"should be [2.2, 3.3], error={vec1}"

print("[SUCCESS]")
