import numpy as np
from pyplasmaopt import CartesianFourierCurve, BiotSavart, StelleratorSymmetricCylindricalFourierCurve

coil = CartesianFourierCurve(3)
coil.coefficients[1][0] = 1.
coil.coefficients[1][1] = 0.5
coil.coefficients[2][2] = 0.5


ma = StelleratorSymmetricCylindricalFourierCurve(3, 2)
ma.coefficients[0][0] = 1.
ma.coefficients[0][1] = 0.1
ma.coefficients[1][0] = 0.1

bs = BiotSavart([coil], [1], 100)

points = ma.gamma(np.linspace(0., 1., 10000))


import time
start = time.time()
for i in range(3):
    bs.B(points, use_cpp=True)
end = time.time()
print('Time for B', (end-start)*1000)

start = time.time()
for i in range(3):
    bs.dB_by_dX(points, use_cpp=True)
end = time.time()
print('Time for dB_by_dX', (end-start)*1000)
