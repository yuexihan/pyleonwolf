========================================================================
                          pyleonwolf
========================================================================

**pyleonwolf** is an implementation of Lazy Propagation Segment Tree for
python users.

An brief introduction to Lazy Propagation Tree can be found here
http://www.geeksforgeeks.org/lazy-propagation-in-segment-tree/

Lazy Propagation Segment Tree is optimized for quickly retrieving the 
summation of a given interval, or  adding the same value to all elements 
in the interval.

It is implemented in C and tested on Python 2.7. It works on Linux. Mac, 
Python 3 and Windows are not tested yet.

Download and source code
========================

You can fetch **pyleonwolf** from:
    - GitHub https://github.com/yuexihan/pyleonwolf

Quick start
===========

This module is written in C. You need a C compiler installed to compile 
native CPython extensions. To install, go to the downloaded folder::

    python setup.py install

Then create an LPSTree::

    >>> import leonwolf
    >>> tree = leonwolf.LPSTree(100, dtype=int)

All elements are set to 0 automatically. The default dtype is int. You 
can also set it to float::

    >>> float_tree = leonwolf.LPSTree(100, dtype=float)

You can create a new tree from an already existed tree. All elements 
will be copied to the new tree::

    >>> new_tree = leonwolf.LPSTree(tree)

You can retrieve element and set value by using common sequence operations::

    >>> len(tree)
    100
    >>> tree[1]
    0
    >>> tree[2] = 1000
    >>> tree[2]
    1000

Add a value to each element of interval [i, j)::

    >>> tree.update(1, 4, -2)
    >>> tree[1]
    -2
    >>> tree[2]
    998
    >>> tree[3]
    -2

Above has the same effect as tree[1]+=-2, tree[2]+=-2, tree[3]+=-2, but much
faster.

Get the sum of elements in a given interval with the sum method::

    >>> tree.sum()
    944
    >>> tree.sum(3)
    966
    >>> tree.sum(1, 2)
    -1

sum() returns sum of all elements. sum(n) returns sum of the first n elements.
sum(i, j) returns sum of interval [i, j).
