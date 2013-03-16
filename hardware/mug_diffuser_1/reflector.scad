// This is from http://www.thingiverse.com/thing:8454
// See also: http://williamaadams.wordpress.com/2011/05/12/mr-bezier-goes-to-openscad/
include <bezier.scad>

measured_points = [
    [0, 80.0/2], // x,y (mug sideways, opening to left, tracking *inside* dimensions)
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
bottomcube_xy = [measured_points[1][0]+2, measured_points[1][1]-4];
module half_mug()
{
    $fn=100;
    union()
    {
        half_mug_maincurve();
        // Cube indicating full depth
        cube([bottomcube_xy[0], bottomcube_xy[1] , gHeight], center = false);
        // Bezier merging the end of the main curve to the bottom cube
        BezCubicFillet([measured_points[1], [bottomcube_xy[0]-1, bottomcube_xy[1]+3.5], [bottomcube_xy[0]+0, bottomcube_xy[1]+2], bottomcube_xy ], [measured_points[1][0]-6,measured_points[1][1]-6]);
    }
}

//half_mug();

module reflector()
{
    $fn=100;
    difference()
    {
        union()
        {
            // Two mug halves mirrored
            half_mug();
            translate([0,0.5,0])
            {
                mirror([0,1,0])
                {
                    half_mug();
                }
            }
            // The semicircle that will be bent 90degrees to form the bottom of the reflector
            translate([0,0.25,0])
            {
                cylinder(r=measured_points[0][1]-0.25, h=gHeight);
            }
        }
        // Two small notches to indicate the bend-line
        translate([-0.5, measured_points[0][1]-2, -0.5])
        {
            cube([1, 2.5, gHeight+1], center = false);
        }
        translate([-0.5, (-1*measured_points[0][1]), -0.5])
        {
            cube([1, 2.5, gHeight+1], center = false);
        }
    }
}

// Output the shape, TODO: project to 2D (since we will be exporting DXF for CNC)
reflector();