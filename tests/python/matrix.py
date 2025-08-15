#!/bin/python3

import sys
import vector
sys.path = ['']

import crunum as crn

def assert_eq_list(matrix, list):
    try:
        assert matrix == crn.matrix.from_list(list), f"should be {list}, error={matrix}"
    except AssertionError:
        raise
    except Exception:
        raise ValueError(f"{matrix}, rows={matrix.rows}, cols={matrix.cols}")

def assert_eq_scalar(matrix, scalar):
    assert matrix == scalar, f"should be all {scalar}, error={matrix}"

def main():
    mat1 = crn.matrix.new(3, 3, value=3.3)

    assert mat1.rows == 3, f"mat2 row size isn't 3, error={mat2.rows}"
    assert mat1.cols == 3, f"mat2 col size isn't 3, error={mat2.rows}"

    assert_eq_scalar(mat1, 3.3)

    mat1.set(0, 0, 2.2)
    mat1.set(2, 1, 3.4)
    mat1.set(1, 0, 11.55)

    assert_eq_list(mat1, [[2.2, 0, 0], [11.55, 0, 0], [0, 3.4, 0]])

    fibo = crn.matrix.new(2, 2)

    fibo.set(0, 0, 1)
    fibo.set(0, 1, 1)
    fibo.set(1, 0, 1)

    assert_eq_list(fibo, [[1, 1], [1, 0]])
    assert_eq_list(fibo ** 5, [[8, 5], [5, 3]])

    base = crn.matrix.new(2, 2)

    base.set(0, 0, 1)
    base.set(0, 1, 2)
    base.set(1, 0, 3)
    base.set(1, 1, 4)

    assert_eq_list(base, [[1, 2], [3, 4]])

    assert_eq_list(base + base, [[2, 4], [6, 8]])
    assert_eq_scalar(base - base, 0)
    assert_eq_list(base * base, [[7, 10], [15, 22]])
    assert_eq_scalar(base / base, 1)

    assert_eq_list(base + 2, [[3, 4], [5, 6]])
    assert_eq_list(base - 5, [[-4, -3], [-2, -1]])
    assert_eq_list(base * 3, [[3, 6], [9, 12]])
    assert_eq_list(base / 2, [[0.5, 1], [1.5, 2]])

    assert_eq_list(2 + base, [[3, 4], [5, 6]])
    assert_eq_list(5 - base, [[4, 3], [2, 1]])
    assert_eq_list(3 * base, [[3, 6], [9, 12]])
    assert_eq_list(6 / base, [[6, 3], [2, 1.5]])

    base.push_row(crn.vector.from_list([5, 6]))

    assert_eq_list(base, [[1, 2], [3, 4], [5, 6]])

    assert base.rows == 3, f"base row size isn't 3, error={base.rows}"
    assert base.cols == 2, f"base col size isn't 2, error={base.rows}"

    identity = crn.matrix.identity(3)

    assert identity.rows == 3, f"identity row size isn't 3, error={identity.rows}"
    assert identity.cols == 3, f"identity col size isn't 3, error={identity.rows}"

    assert_eq_list(identity, [[1, 0, 0], [0, 1, 0], [0, 0, 1]])

    vector.assert_eq_list(base.pop_row(), [5, 6])

    assert_eq_list(base, [[1, 2], [3, 4]])

    base_inverse = base.inverse()

    assert_eq_list(base * base_inverse, [[1, 0], [0, 1]])

    print("[SUCCESS]")

if __name__ == "__main__":
    main()
