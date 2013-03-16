include <bezier.scad>

measured_points = [
    [0, 80.0/2], // x,y (mug sideways, opening to left, tracking *outside* dimensions)
    [110, 46.0/2],
    [110.0/2, 73.0/2],
    [110.0/4, 76.0/2],
    [110.0/4*3, 61.0/2],
];

/**
 * The fitted control points from curve_fitting.py
 *
    (62, 41), # P1
    (97, 27), # P2
 */
/**
 * This is the main shape of half a mug, lacking the steep curve at the very bottom
 */
module half_mug_maincurve()
{
    // Calculate and cache results, will speed up rendering especially when modifying other parameters.
    render()
    {
        union()
        {
            BezCubicFillet([measured_points[0], [62, 41], [97, 27], measured_points[1] ], [0,0]);
            cube([measured_points[1][0], measured_points[1][1], gHeight], center = false);
        }
    }
}
/**
 * Half a mug
 */
module half_mug()
{
    half_mug_maincurve();
    // TODO: Add union for the last curve and a few mm more of x-dimension lenght
}

module reflector()
{
    union()
    {
        half_mug_maincurve();
        translate([0,0.5,0])
        {
            mirror([0,1,0])
            {
                half_mug_maincurve();
            }
        }
        // TODO: Add the "bottom" of the reflector that will be bent 90degrees.
    }
}

// Output the shape, TODO: project to 2D (since we will be exporting DXF for CNC)
reflector();