import itertools
import kbest_intervals

import pytest
import numpy as np
from scipy.ndimage.measurements import label

np.random.seed(15)


@pytest.mark.parametrize("size,k",
                         list(
                             itertools.product([
                                 5,
                                 13,
                                 135,
                                 388,
                                 2349,
                             ], [1, 2, 3, 10, 100, 200])))
def test_kbest(size, k):
    '''sanity checks.

    '''
    repeats = 5
    for _ in range(repeats):
        weights = np.random.randn(size)

        kbest = kbest_intervals.calculate_kbest_intervals(weights, k)

        assert np.sum(weights * kbest) >= np.sum(weights)
        assert label(kbest)[1] <= k


@pytest.mark.parametrize("vals,k,expected",
                         [([
                             0.70, 2.66, 2.49, 0.23, 0.14, 0.72, -1.22, 0.36,
                             1.44, -0.69, 0.41, 1.44, 2.10
                         ], 3, [1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1]),
                          ([
                             0.70, 2.66, 2.49, 0.23, 0.14, 0.72, -1.22, 0.36,
                             1.44, -0.69, 0.41, 1.44, 2.10
                          ], 2, [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1]),
                          ([5, -1, 3, -3, 1], 3, [1, 0, 1, 0, 1]),
                          (-np.ones(5), 2, np.zeros(5)),
                          (-np.ones(5), 1, np.zeros(5)),
                          (np.ones(5), 2, np.ones(5))])
def test_ending(vals, k, expected):
    '''
    '''
    assert len(vals) == len(expected)

    kbest = kbest_intervals.calculate_kbest_intervals(vals, k)

    print(kbest)
    print(expected)
    print(np.round(vals, 1))

    assert all(x == y for x, y in zip(kbest, expected))
