#!/usr/bin/python
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

measured_points = [
    (0, 82.0/2), # x,y (mug sideways, opening to left, tracking *outside* dimensions)
    (114, 50.0/2),
    (114.0/2, 75.0/2),
    (114.0/4, 78.0/2),
    (114.0/4*3, 63.0/2),
]
measured_xs, measured_ys = zip(*measured_points)




fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot(measured_xs, measured_ys, 'ro')



ax.set_xlim(-10, 120)
ax.set_ylim(-10, 120)
plt.show()
