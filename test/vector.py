import sys
import math
sys.path = ['']

import crunum as crn

def assert_eq_list(vector, list):
    try:
        assert vector == crn.vector.from_list(list), f"should be {list}, error={vector}"
    except:
        raise ValueError(f"{vector}, len={vector.len}")

def assert_eq_scalar(vector, scalar):
    assert vector == scalar, f"should be all {scalar}, error={vector}"

def main():
    vec1 = crn.vector.new(2)

    assert vec1.len == 2, f"vec1 length isn't 2, error={vec1.len}"

    assert_eq_scalar(vec1, 0)

    vec1[0] = 2.2
    vec1[1] = 3.3

    assert_eq_list(vec1, [2.2, 3.3])

    vec1.push(5.5)
    vec1.push(6.7)

    assert vec1.len == 4, f"vec1 length isn't 4, error={vec1.len}"

    assert_eq_list(vec1, [2.2, 3.3, 5.5, 6.7])

    last_value = vec1.pop()

    assert math.isclose(last_value, 6.7, rel_tol=1e-6), f"should be 6.7, error={last_value}"

    assert vec1.len == 3, f"vec1 length isn't 3, error={vec1.len}"

    print("[SUCCESS]")

if __name__ == "__main__":
    main()
