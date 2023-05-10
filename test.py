#!/usr/bin/env python3

import unittest
from lb_matching_tools.tests.test_cleaner import suite

if __name__ == '__main__':
    unittest.TextTestRunner().run(suite())
