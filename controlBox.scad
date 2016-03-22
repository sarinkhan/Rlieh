include<../parts/textLcdBezels.scad>


//bezel4x20X=110;
bezel4x20Y=80;

boxWallsThickness=1.5;
buttonsAreaX=30;

buttonsHoleRadius=16.2/2;
beamsThickness=3;

boxPart1Z=25;
screwHoles1Radius=3/2;
pillarsWidth1=5;

module fixationPillar(pillarWidth=pillarsWidth1,pillarHeight=boxPart1Z,pillarScrewRadius=screwHoles1Radius)
{
    difference()
    {
    cube([pillarWidth,pillarWidth,pillarHeight]);
        translate([pillarWidth/2,pillarWidth/2,1])
        cylinder(r=pillarScrewRadius,h=pillarHeight,$fn=12);
    } 
}

nbButtons=3;
buttonDecalX=(buttonsAreaX-beamsThickness*2-1)/2;
buttonDecalY=(bezel4x20Y-beamsThickness*2-nbButtons*buttonsHoleRadius*2)/(nbButtons+1);
echo (buttonDecalY);
bezel4x20(bezel4x20X,bezel4x20Y,boxWallsThickness,3/2);
translate([bezel4x20X-0.5,0,0])
{
    difference()
    {
        cube([buttonsAreaX,bezel4x20Y,boxWallsThickness]);
        for(i = [1 : 1 : nbButtons])
        {
            translate([buttonDecalX+1,beamsThickness+buttonDecalY*i+buttonsHoleRadius+buttonsHoleRadius*2*(i-1),-boxWallsThickness/2])
                cylinder(r=buttonsHoleRadius,h=boxWallsThickness*2);
        }
        
        /*translate([buttonDecalX+1,27,-boxWallsThickness/2])
            cylinder(r=buttonsHoleRadius,h=boxWallsThickness*2);
        translate([buttonDecalX+1,55,-boxWallsThickness/2])
            cylinder(r=buttonsHoleRadius,h=boxWallsThickness*2);*/
        
    }
}

//beams
translate([0,0,boxWallsThickness])
    cube([bezel4x20X+buttonsAreaX-0.5,beamsThickness,beamsThickness]);
translate([0,bezel4x20Y-beamsThickness,boxWallsThickness])
    cube([bezel4x20X+buttonsAreaX-0.5,beamsThickness,beamsThickness]);

translate([0,0,boxWallsThickness])
    cube([beamsThickness,bezel4x20Y,beamsThickness]);
translate([bezel4x20X+buttonsAreaX-beamsThickness-0.5,0,boxWallsThickness])
    cube([beamsThickness,bezel4x20Y,beamsThickness]);

translate([bezel4x20X-beamsThickness+1,0,boxWallsThickness])
    cube([beamsThickness,bezel4x20Y,beamsThickness]);

//walls
translate([0,0,boxWallsThickness])
    cube([bezel4x20X+buttonsAreaX-0.5,boxWallsThickness,boxPart1Z]);
translate([0,bezel4x20Y-boxWallsThickness,boxWallsThickness])
    cube([bezel4x20X+buttonsAreaX-0.5,boxWallsThickness,boxPart1Z]);
translate([0,0,boxWallsThickness])
    cube([boxWallsThickness,bezel4x20Y,boxPart1Z]);
translate([bezel4x20X+buttonsAreaX-boxWallsThickness-0.5,0,boxWallsThickness])
    cube([boxWallsThickness,bezel4x20Y,boxPart1Z]);

//pillars
translate([boxWallsThickness,boxWallsThickness,boxWallsThickness])    
    fixationPillar();
translate([bezel4x20X+buttonsAreaX-pillarsWidth1-boxWallsThickness,boxWallsThickness,boxWallsThickness])    
    fixationPillar();


translate([(bezel4x20X+buttonsAreaX)/2-pillarsWidth1-boxWallsThickness,bezel4x20Y-pillarsWidth1-boxWallsThickness,boxWallsThickness])    
    fixationPillar();
translate([(bezel4x20X+buttonsAreaX)/2-pillarsWidth1-boxWallsThickness,boxWallsThickness,boxWallsThickness])    
    fixationPillar();

    
translate([boxWallsThickness,bezel4x20Y-pillarsWidth1-boxWallsThickness,boxWallsThickness])    
    fixationPillar();
translate([bezel4x20X+buttonsAreaX-pillarsWidth1-boxWallsThickness,bezel4x20Y-pillarsWidth1-boxWallsThickness,boxWallsThickness])    
    fixationPillar();
