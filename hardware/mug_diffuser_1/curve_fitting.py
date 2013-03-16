#!/usr/bin/python
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

measured_points = [
    (0, 80.0/2), # x,y (mug sideways, opening to left, tracking *outside* dimensions)
    (110, 46.0/2),
    (110.0/2, 73.0/2),
    (110.0/4, 76.0/2),
    (110.0/4*3, 61.0/2),
]
measured_xs, measured_ys = zip(*measured_points)


# Create a plot and add the measured points
fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(measured_xs, measured_ys, 'ro')

# Bezier code from http://matplotlib.org/users/path_tutorial.html
verts = [
    measured_points[0],  # P0
    (62, 41), # P1
    (97, 27), # P2
    measured_points[1], # P3
    ]

codes = [Path.MOVETO,
         Path.CURVE4,
         Path.CURVE4,
         Path.CURVE4,
         ]

path = Path(verts, codes)
patch = patches.PathPatch(path, facecolor='none', lw=2)
ax.add_patch(patch)

xs, ys = zip(*verts)
ax.plot(xs, ys, 'x--', lw=2, color='black', ms=10)

ax.text(xs[0]-0.05, ys[0]-0.05, 'P0')
ax.text(xs[1]+0.05, ys[1]+0.05, 'P1')
ax.text(xs[2]+0.05, ys[2]+0.05, 'P2')
ax.text(xs[3]-0.05, ys[3]-0.05, 'P3')



ax.set_xlim(-10, 120)
ax.set_ylim(-10, 120)
plt.show()
