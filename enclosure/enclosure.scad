battSlotsSpacingX=1;
beamsThickness2=5;
firstStageHeight=28;

include<../parts/lipoBattHolder.scad>
include<waterproofBox.scad>
include<../parts/rpiPlate5_halfFlatMount_mod.scad>




//constraints : these are the minimal internal dimensions of the box
insideBoxMinX=basicBattHolderLength;
insideBoxMinY=83+2*beamsThickness2;
insideBoxMinZ=72;




//if you want to alter the box size, change the values here, keeping the min sizes and adding a value : 
insideBoxX1=insideBoxMinX+battSlotsSpacingX*2+25;
insideBoxY1=insideBoxMinY+0;
insideBoxZ1=insideBoxMinZ+0;


boxSealingLipWidth=8;
boxLipThickness=2;




echo ("inside box X : ");
echo(insideBoxX1);
echo ("inside box Y : ");
echo(insideBoxY1);
echo ("inside box Z : ");
echo(insideBoxZ1);

//basicBattHolderWidth
boxWallsThickness1=1;
boxSealingLipWidth=beamsThickness2;
screwHoles1Radius=3/2;



secondStageDecal=0.5;


cameraHoleRadius=20/2;
cameraHoleZDecal=5;
cameraSkirtSupportThickness=7;
cameraSkirtSupportBaseWidth=cameraHoleRadius*2+20;
cameraSkirtSupportBaseHeight=cameraSkirtSupportBaseWidth;
cameraSkirtSupportDecalY=(cameraSkirtSupportBaseWidth-cameraHoleRadius*2)/2;
cameraSkirtSupportDecalZ=(cameraSkirtSupportBaseHeight-cameraHoleRadius*2)/2;
cameraSkirtSupportScrewHolesDistFromEdge=4;

cameraHole=1;   //set to 1 for a camera hole, 0 for no hole

buttonsHoles=1; //set to 1 for button holes, 0 for no hole
buttonsHolesRadius=16.8/2;
buttonsCount1=3;
buttonsMargin1=5;

totalBoundingBoxX=insideBoxX1+2*boxWallsThickness1+2*boxSealingLipWidth;
totalBoundingBoxY=insideBoxY1+2*boxWallsThickness1+2*boxSealingLipWidth;
totalBoundingBoxZ=insideBoxZ1+boxWallsThickness1+0;//define


frontOpeningRadius=buttonsHolesRadius;
frontOpeningsPlateX=cameraSkirtSupportThickness;
frontOpeningsPlateY=frontOpeningRadius*2+10;
frontOpeningsPlateZ=frontOpeningsPlateY;
frontOpeningsholesDistFromEdge=3;
frontOpening1=1;
frontOpening2=1;
frontOpeningDecalZ=firstStageHeight+beamsThickness2+5;


frontOpeningsGasketThickness=1.5;
frontOpeningsLidThickness=4;



sideOpeningRadius=frontOpeningRadius;
sideOpeningsPlateX=frontOpeningsPlateX;
sideOpeningsPlateY=frontOpeningsPlateY;
sideOpeningsPlateZ=frontOpeningsPlateZ;
sideOpeningsholesDistFromEdge=frontOpeningsholesDistFromEdge;
sideOpening1=1;
sideOpening2=1;
sideOpeningDecalZ=frontOpeningDecalZ;


backOpenings1=1;
backOpenings2=1;
backOpeningsCount=3;
backOpeningRadius=frontOpeningRadius;
backOpeningsPlateX=frontOpeningsPlateX;
backOpeningsPlateY=frontOpeningsPlateY;
backOpeningsPlateZ=frontOpeningsPlateZ;
backOpeningsholesDistFromEdge=frontOpeningsholesDistFromEdge;
backOpening1DecalZ=frontOpeningDecalZ;
backOpening2DecalZ=2;
backOpenings1Count=3;
backOpenings2Count=3;



/**
 * This module constructs a circular opening with a support base, and 4 screwholes in each corner
 */
module openingWithSupport1(centralOpeningRadius=buttonsHolesRadius,frFixHolesRad=screwHoles1Radius,baseX,baseY,baseZ,holesDistFromEdge)
{
    difference()
    {
        cube([baseX,baseY,baseZ]);
        translate([-baseX/2,baseY/2,baseZ/2])
            rotate([0,90,0])
                cylinder(r=centralOpeningRadius,h=baseX*2,$fn=64);
        
       translate([-baseX/2,holesDistFromEdge,holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,baseY-holesDistFromEdge,holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,holesDistFromEdge,baseZ-holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,baseY-holesDistFromEdge,baseZ-holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
    }
}
module openingWithSupport1WithSlope(centralOpeningRadius=buttonsHolesRadius,frFixHolesRad=screwHoles1Radius,baseX,baseY,baseZ,holesDistFromEdge)
{
   openingWithSupport1(centralOpeningRadius,frFixHolesRad,baseX,baseY,baseZ,holesDistFromEdge);
    translate([baseX,baseY,0])
            rotate([90,-180,0])
                triangleSupport(baseX,baseX,baseY);
    
}

module openingLid(frFixHolesRad=screwHoles1Radius,baseX,baseY,baseZ,holesDistFromEdge)
{
    difference()
    {
        cube([baseX,baseY,baseZ]);
       translate([-baseX/2,holesDistFromEdge,holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,baseY-holesDistFromEdge,holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,holesDistFromEdge,baseZ-holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
       translate([-baseX/2,baseY-holesDistFromEdge,baseZ-holesDistFromEdge])
            rotate([0,90,0])
                cylinder(r=frFixHolesRad,h=baseX*2,$fn=16);
    }
}

module cameraSkirtBaseShape()
{
    openingWithSupport1(buttonsHolesRadius,screwHoles1Radius,cameraSkirtSupportThickness,cameraSkirtSupportBaseWidth,cameraSkirtSupportBaseHeight,cameraSkirtSupportScrewHolesDistFromEdge);
}

module cameraSkirt()
{
    cameraSkirtBaseShape();
}


module cameraSkirtBaseShapeWithSlope()
{
    cameraSkirtBaseShape();
    translate([cameraSkirtSupportThickness,cameraSkirtSupportBaseWidth,0])
            rotate([90,-180,0])
                triangleSupport(cameraSkirtSupportThickness,cameraSkirtSupportThickness,cameraSkirtSupportBaseWidth);
}



module mainBox()
{
            
            backOpeningsDecalY=(insideBoxY1-backOpeningsPlateY*backOpenings1Count-beamsThickness*2)/2;
difference()
{
    union()
    {
        boxWithLidAndBeams(insideBoxX1,insideBoxY1,insideBoxZ1,boxWallsThickness1,boxSealingLipWidth,boxLipThickness,3,2,screwHoles1Radius, beamsThickness2);
         if(cameraHole==1)
        {
            translate([-boxWallsThickness1-cameraSkirtSupportThickness,insideBoxY1/2-cameraHoleRadius-cameraSkirtSupportDecalY,firstStageHeight+cameraHoleZDecal+beamsThickness2-cameraSkirtSupportDecalZ])
            {
                cameraSkirtBaseShapeWithSlope();
            }
        }
        
        //the two front openings that can be used for LED (like a flash), buttons or anything else)
            if (frontOpening1==1)
            {
                translate([-frontOpeningsPlateX-boxWallsThickness1,0,frontOpeningDecalZ])
                openingWithSupport1WithSlope(frontOpeningRadius,screwHoles1Radius,frontOpeningsPlateX,frontOpeningsPlateY,frontOpeningsPlateZ,frontOpeningsholesDistFromEdge);
            }
            
            if (frontOpening2==1)
            {
                translate([-frontOpeningsPlateX-boxWallsThickness1,insideBoxY1-frontOpeningsPlateY,frontOpeningDecalZ])
                openingWithSupport1WithSlope(frontOpeningRadius,screwHoles1Radius,frontOpeningsPlateX,frontOpeningsPlateY,frontOpeningsPlateZ,frontOpeningsholesDistFromEdge);
            }
            
        //the side openings
            if (sideOpening1==1)
            {
                translate([insideBoxX1-beamsThickness2+boxWallsThickness1,-sideOpeningsPlateX-boxWallsThickness1,frontOpeningDecalZ])
                    rotate([0,0,90])
                        openingWithSupport1WithSlope(sideOpeningRadius,screwHoles1Radius,sideOpeningsPlateX,sideOpeningsPlateY,sideOpeningsPlateZ,sideOpeningsholesDistFromEdge);
            }
            
            if (sideOpening2==1)
            {
                translate([insideBoxX1-beamsThickness2+boxWallsThickness1-sideOpeningsPlateY,insideBoxY1+sideOpeningsPlateX+boxWallsThickness1,frontOpeningDecalZ])
                    rotate([0,0,270])
                        openingWithSupport1WithSlope(sideOpeningRadius,screwHoles1Radius,sideOpeningsPlateX,sideOpeningsPlateY,sideOpeningsPlateZ,sideOpeningsholesDistFromEdge);
            }
            
            //top back openings
            if (backOpenings1==1)
            {
                for(i = [1 : 1 : backOpenings1Count])
                {
                    translate([insideBoxX1+backOpeningsPlateX+boxWallsThickness1,backOpeningsDecalY+backOpeningsPlateY*i+beamsThickness,backOpening1DecalZ])
                    rotate([0,0,180])
                    openingWithSupport1WithSlope(backOpeningRadius,screwHoles1Radius,backOpeningsPlateX,backOpeningsPlateY,backOpeningsPlateZ,backOpeningsholesDistFromEdge);
                }
            }
            
            //bottom back openings
            if (backOpenings2==1)
            {
                for(i = [1 : 1 : backOpenings2Count])
                {
                    translate([insideBoxX1+backOpeningsPlateX+boxWallsThickness1,backOpeningsDecalY+backOpeningsPlateY*i+beamsThickness,backOpening2DecalZ])
                    rotate([0,0,180])
                    openingWithSupport1(backOpeningRadius,screwHoles1Radius,backOpeningsPlateX,backOpeningsPlateY,backOpeningsPlateZ,backOpeningsholesDistFromEdge);
                }
                
                translate([insideBoxX1+boxWallsThickness1,backOpeningsDecalY+beamsThickness,0])
                    cube([backOpeningsPlateX,backOpenings2Count*backOpeningsPlateY,backOpening2DecalZ]);
            }
            
            
            
            
            
            
    }
    if(cameraHole==1)
    {
        translate([-boxWallsThickness1*2-cameraSkirtSupportThickness,0+insideBoxY1/2,firstStageHeight+cameraHoleZDecal+cameraHoleRadius+beamsThickness2])
            rotate([0,90,0])                 
                cylinder(r=cameraHoleRadius,h=boxWallsThickness1*4+cameraSkirtSupportThickness*2,$fn=64);
    }
    if (frontOpening1==1)
    {
        translate([-boxWallsThickness1*2,frontOpeningsPlateY/2,frontOpeningDecalZ+frontOpeningsPlateY/2])
            rotate([0,90,0])
                cylinder(r=frontOpeningRadius,h=frontOpeningsPlateX*2+boxWallsThickness1*4,$fn=64);
        }
    if (frontOpening2==1)
    {
        translate([-boxWallsThickness1*2,insideBoxY1-frontOpeningsPlateY/2,frontOpeningDecalZ+frontOpeningsPlateY/2])
            rotate([0,90,0])
                cylinder(r=frontOpeningRadius,h=frontOpeningsPlateX*2+boxWallsThickness1*4,$fn=64); 
    }
    
    if (sideOpening1==1)
    {
        translate([insideBoxX1-beamsThickness2+boxWallsThickness1-sideOpeningsPlateY/2,-sideOpeningsPlateX-boxWallsThickness1,frontOpeningDecalZ+sideOpeningsPlateZ/2])
            rotate([270,0,00])
                cylinder(r=sideOpeningRadius,h=frontOpeningsPlateX*2+boxWallsThickness1*4,$fn=64);
    }
            
    if (sideOpening2==1)
    {
        translate([insideBoxX1-beamsThickness2+boxWallsThickness1-sideOpeningsPlateY/2,insideBoxY1+sideOpeningsPlateX+boxWallsThickness1,frontOpeningDecalZ+sideOpeningsPlateZ/2])
            rotate([90,00,0])
                cylinder(r=sideOpeningRadius,h=frontOpeningsPlateX*2+boxWallsThickness1*4,$fn=64);
    }
    
    if(backOpenings1==1)
    {
        for(i = [1 : 1 : backOpenings1Count])
        {
            translate([insideBoxX1-boxWallsThickness1,backOpeningsDecalY+backOpeningsPlateY*i+beamsThickness - backOpeningsPlateY/2,frontOpeningDecalZ+backOpeningsPlateZ/2])
            rotate([0,90,0])                 
                cylinder(r=buttonsHolesRadius,h=boxWallsThickness1*4,$fn=64);
        }
    }
    
    if(backOpenings1==1)
    {
        for(i = [1 : 1 : backOpenings1Count])
        {
            translate([insideBoxX1-boxWallsThickness1,backOpeningsDecalY+backOpeningsPlateY*i+beamsThickness - backOpeningsPlateY/2,backOpening1DecalZ+backOpeningsPlateZ/2])
            rotate([0,90,0])                 
                cylinder(r=buttonsHolesRadius,h=boxWallsThickness1*4,$fn=64);
        }
    }
    
        if(backOpenings2==1)
    {
        for(i = [1 : 1 : backOpenings1Count])
        {
            translate([insideBoxX1-boxWallsThickness1,backOpeningsDecalY+backOpeningsPlateY*i+beamsThickness - backOpeningsPlateY/2,backOpening2DecalZ+backOpeningsPlateZ/2])
            rotate([0,90,0])                 
                cylinder(r=buttonsHolesRadius,h=boxWallsThickness1*4,$fn=64);
        }
    }
    

   
    
 
    
    //buttonDecalY*i+buttonsHolesRadius+buttonsHolesRadius*2*(i-1)
    /*buttonDecalY=(insideBoxY1-buttonsHolesRadius*2*buttonsCount1-beamsThickness*2 - buttonsMargin1*2)/buttonsCount1;
    if(buttonsHoles==1)
    {
        for(i = [0 : 1 : buttonsCount1-1])
        {
            translate([insideBoxX1-boxWallsThickness1*2, beamsThickness2+buttonsHolesRadius+buttonDecalY*i  +buttonsHolesRadius*2*(i)+buttonsMargin1,buttonsHolesRadius+boxWallsThickness1+buttonsMargin1])
                rotate([0,90,0])                 
                cylinder(r=buttonsHolesRadius,h=boxWallsThickness1*4,$fn=64);
        }
    }*/
}

maxBattSlots=floor((insideBoxY1-2*beamsThickness2)/basicBattHolderWidth);
nbBattSlots=maxBattSlots;


battSlotsSpacingY=(insideBoxY1-(nbBattSlots*basicBattHolderWidth)-2*beamsThickness2)/nbBattSlots;

    for(i = [0 : 1 : nbBattSlots-1])
    {
        translate([battSlotsSpacingX,beamsThickness2+battSlotsSpacingY/2+(basicBattHolderWidth+battSlotsSpacingY)*i,boxWallsThickness1])
            basicBattHolder();
    }
    
    translate([0,0,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
    translate([0,insideBoxY1-beamsThickness2,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
    
    translate([insideBoxX1/2-beamsThickness2,0,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
    translate([insideBoxX1/2-beamsThickness2,insideBoxY1-beamsThickness2,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
    
    translate([insideBoxX1-beamsThickness2,0,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
    translate([insideBoxX1-beamsThickness2,insideBoxY1-beamsThickness2,boxWallsThickness1])
            fixationPillar(beamsThickness1,firstStageHeight,screwHoles1Radius);
}    


cameraPiCameraAxisZ=14;
cameraSupportX=4;
cameraSupportY=25;
cameraSupportZ=24;
//cameraSupportZ=cameraPiCameraAxisZ;
cameraSupportBeamsY=7;
cameraHolesRadius=2/2;
cameraHolesDistFromEdge=2;
cameraHoles1DistFromBottom=2+0.5;
cameraHoles2DistFromBottom=cameraHoles1DistFromBottom+12.5;

cameraSupportBaseX=beamsThickness2;
cameraSupportBaseZ=cameraHoleZDecal+cameraHoleRadius-cameraPiCameraAxisZ;

module cameraSupport()
{
    difference()
    {
        union()
        {
            cube([cameraSupportBaseX,cameraSupportY,cameraSupportBaseZ]);
            translate([0,0,cameraSupportBaseZ])
            cube([cameraSupportX,cameraSupportBeamsY,cameraSupportZ]);
            translate([0,cameraSupportY-cameraSupportBeamsY,cameraSupportBaseZ])
            cube([cameraSupportX,cameraSupportBeamsY,cameraSupportZ]);
        }
                translate([-cameraSupportBaseX/2, cameraHolesDistFromEdge, cameraSupportBaseZ+cameraHoles1DistFromBottom])
            rotate([0,90,0])
                cylinder(r=cameraHolesRadius,h=cameraSupportBaseX*2,$fn=64);
        translate([-cameraSupportBaseX/2, cameraSupportY-cameraHolesDistFromEdge, cameraSupportBaseZ+cameraHoles1DistFromBottom])
            rotate([0,90,0])
                cylinder(r=cameraHolesRadius,h=cameraSupportBaseX*2,$fn=64);
        
        translate([-cameraSupportBaseX/2, cameraHolesDistFromEdge, cameraSupportBaseZ+cameraHoles2DistFromBottom])
            rotate([0,90,0])
                cylinder(r=cameraHolesRadius,h=cameraSupportBaseX*2,$fn=64);
        translate([-cameraSupportBaseX/2, cameraSupportY-cameraHolesDistFromEdge, cameraSupportBaseZ+cameraHoles2DistFromBottom])
            rotate([0,90,0])
                cylinder(r=cameraHolesRadius,h=cameraSupportBaseX*2,$fn=64);
    }
}


module electronicsPlate()
{
    piSupportSpacingX=beamsThickness2+secondStageDecal+8;
    piSupportSpacingY=(insideBoxY1-beamsThickness2*2-chassisY)/2;
    
    beam1XDecal=round((insideBoxX1-chassisX-secondStageDecal*2-10-beamsThickness2*3)/2);
    
    translate([0,0,boxWallsThickness1+firstStageHeight])
    {
        difference()
        {
            union()
            {
                translate([secondStageDecal,secondStageDecal,0])
                    cube([insideBoxX1-2*secondStageDecal,beamsThickness2,beamsThickness2]);
                translate([secondStageDecal,secondStageDecal,0])
                    cube([beamsThickness2,insideBoxY1-2*secondStageDecal,beamsThickness2]);
                translate([secondStageDecal,insideBoxY1-beamsThickness2-secondStageDecal,0])
                    cube([insideBoxX1-2*secondStageDecal,beamsThickness1,beamsThickness2]);
                translate([insideBoxX1-beamsThickness2-secondStageDecal,secondStageDecal,0])
                    cube([beamsThickness2,insideBoxY1-2*secondStageDecal,beamsThickness2]);
                translate([insideBoxX1-beamsThickness2*3/2-secondStageDecal-beam1XDecal,secondStageDecal,0])
                    cube([beamsThickness2,insideBoxY1-2*secondStageDecal,beamsThickness2]);
                translate([piSupportSpacingX+chassisX+2,secondStageDecal,0])
                    cube([beamsThickness1,insideBoxY1-2*secondStageDecal,beamsThickness2]);
                
                translate([secondStageDecal,secondStageDecal+piSupportSpacingY,0])
                    cube([piSupportSpacingX,insideBoxY1-2*secondStageDecal-piSupportSpacingY*2,beamsThickness2]);
            }
            //front
            translate([beamsThickness2/2,beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            translate([beamsThickness2/2,insideBoxY1-beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            //middle
            translate([insideBoxX1/2-beamsThickness2/2,beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            translate([insideBoxX1/2-beamsThickness2/2,insideBoxY1-beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            //back
            translate([insideBoxX1-beamsThickness2/2,beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            translate([insideBoxX1-beamsThickness2/2,insideBoxY1-beamsThickness2/2,-beamsThickness2/2])
                cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            
            
            beam1ScrewsCount=6;
            screwsDecalBeamY=(insideBoxY1-beamsThickness2-secondStageDecal*2)/(beam1ScrewsCount-1);
            
            for(i = [0 : 1 : beam1ScrewsCount-1])
            {
                translate([insideBoxX1-beamsThickness2-secondStageDecal-beam1XDecal,secondStageDecal+beamsThickness2/2+i*screwsDecalBeamY,-beamsThickness2/2])
                    cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
                
                translate([piSupportSpacingX+chassisX+2+beamsThickness2/2+secondStageDecal,secondStageDecal+beamsThickness2/2+i*screwsDecalBeamY,-beamsThickness2/2])
                    cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            }
            
            for(i = [1 : 1 : beam1ScrewsCount-2])
            {
                translate([insideBoxX1-beamsThickness2/2-secondStageDecal,secondStageDecal+beamsThickness2/2+i*screwsDecalBeamY,-beamsThickness2/2])
                    cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            }
            
            
            beam2ScrewsCount=4;
            screws2DecalBeamX=(insideBoxX1/2-5)/beam2ScrewsCount;
            for(i = [0 : 1 : beam2ScrewsCount-1])
            {
                translate([secondStageDecal+beamsThickness2+screws2DecalBeamX*i+2.5,secondStageDecal+beamsThickness2/2,-beamsThickness2/2])
                    cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
                translate([secondStageDecal+beamsThickness2+screws2DecalBeamX*i+2.5,insideBoxY1-secondStageDecal-beamsThickness2/2,-beamsThickness2/2])
                    cylinder(r=screwHoles1Radius, h=beamsThickness2*2,$fn=12);
            }
        }
        
        translate([2,(insideBoxY1-cameraSupportY)/2,beamsThickness2])
        cameraSupport();
        
        translate([piSupportSpacingX,beamsThickness2+piSupportSpacingY,0])
        color([1,0.5,0])
            piSupport2(1,1,h0);
        
        
    }
}   
    //electronicsPlate();
//mainBox();
/*difference()
{
electronicsPlate();
    /*translate([2*0,0,-5])
    cube([200,200,200]);
}*/

//front and side openings lids : 
/*translate([0,-frontOpeningsPlateY-beamsThickness2-boxWallsThickness1-5,0])
rotate([0,90,0])
openingLid(screwHoles1Radius,frontOpeningsLidThickness,frontOpeningsPlateY,frontOpeningsPlateZ,frontOpeningsholesDistFromEdge);
*/


//front and side openings gaskets : 
//frontOpeningsGasketThickness=7;
translate([0,-frontOpeningsPlateY*2-beamsThickness2-boxWallsThickness1-5*2,0])
rotate([0,90,0])
openingWithSupport1(buttonsHolesRadius,screwHoles1Radius,frontOpeningsGasketThickness,frontOpeningsPlateY,frontOpeningsPlateZ,frontOpeningsholesDistFromEdge);
  
    
//gasket();

//translate([0,0,+insideBoxZ1+2])
//flatBoxLid();

    
    
    
    
    
    
    
    
    

