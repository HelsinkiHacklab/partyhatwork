include <bezier.scad>

measured_points = [
    [0, 82.0/2], // x,y (mug sideways, opening to left, tracking *outside* dimensions)
    [114, 50.0/2],
    [114.0/2, 75.0/2],
    [114.0/4, 78.0/2],
    [114.0/4*3, 63.0/2],
];

// The fitted points from curve_fitting.py
//    (65, 40), # P1
//    (95, 30), # P2

module half_mug()
{
    union()
    {
        BezCubicFillet([measured_points[0], [65, 40], [95, 30], measured_points[1] ], [0,0]);
        cube([measured_points[1][0], measured_points[1][1], gHeight], center = false);
    }
}

half_mug();