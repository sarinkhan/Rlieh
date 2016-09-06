lcd2x16_screenX=71.5;
lcd2x16_screenY=24.5;
lcd2x16_screenZ=8;

lcd2x16_circuitX=80.5;
lcd2x16_circuitY=36.5;
lcd2x16_circuitZ=1.7;


lcd2x16_screenDecalX=4.5;
lcd2x16_screenDecalY=5.5;

lcdHolesDistFromEdge=2;
lcdHolesRadius=3;

bezelX=90;
bezelY=40;
bezelWallsThickness=1;


bezel4x20X=110;
bezel4x20Y=70;
bezel4x20WallsThickness=1;
lcd4x20HolesDistFromEdge=1.1;
lcd4x20HolesRadius=3.2/2;

lcd4x20_screenX=98.3;
lcd4x20_screenY=40.5;
lcd4x20_screenZ=8.5;

lcd4x20_circuitX=98.5;
lcd4x20_circuitY=60.3;
lcd4x20_circuitZ=1.7;


lcd4x20_screenDecalX=0;
lcd4x20_screenDecalY=10;



module screwSupport(rp1,rp2,h1,h2,holeR)
{
    
    difference()
    {
    union()
    {
    cylinder(r1=rp1,r2=rp2,h=h1,$fn=16);
    cylinder(r=rp2,h=h2,$fn=16);
    }
    translate([0,0,-h2/2])
    cylinder(r=holeR,h=h2*2,$fn=16);
    }
    
}


module bezel2x16(bx,by,bThick,lcdHolesR)
{
    pcbXDecal=(bx-lcd2x16_circuitX)/2;
    pcbYDecal=(by-lcd2x16_circuitY)/2;
    
    lcdXDecal=pcbXDecal+lcd2x16_screenDecalX;
    lcdYDecal=pcbYDecal+lcd2x16_screenDecalY;
    
    /*lcdXDecal=(bx-lcd2x16_screenX)/2;
    lcdYDecal=(by-lcd2x16_screenY)/2;*/
 
    difference()
    {
        cube([bx,by,bThick]);
        translate([lcdXDecal,lcdYDecal,-bThick])
            cube([lcd2x16_screenX,lcd2x16_screenY,lcd2x16_screenZ]);
    }
    
    /*color("green")
    {
        translate([pcbXDecal,pcbYDecal,+bThick])
            cube([lcd2x16_circuitX,lcd2x16_circuitY,lcd2x16_circuitZ]);
    }*/
    
    rp1=(lcdHolesR+bThick)*0.95;
    rp2=(lcdHolesR+bThick)*0.9;
    h1=lcd2x16_screenZ/2;
    h2=lcd2x16_screenZ;
    holeR=lcdHolesR;
    translate([pcbXDecal,pcbYDecal,+bThick])
    {
        translate([lcdHolesR+lcdHolesDistFromEdge,lcdHolesR+lcdHolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcd2x16_circuitX-lcdHolesR-lcdHolesDistFromEdge,lcdHolesR+lcdHolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcdHolesR+lcdHolesDistFromEdge,lcd2x16_circuitY-lcdHolesR-lcdHolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcd2x16_circuitX-lcdHolesR-lcdHolesDistFromEdge,lcd2x16_circuitY-lcdHolesR-lcdHolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
    }
}


module bezel4x20(bx,by,bThick,lcdHolesR)
{
    pcbXDecal=(bx-lcd4x20_circuitX)/2;
    pcbYDecal=(by-lcd4x20_circuitY)/2;
    
    lcdXDecal=pcbXDecal+lcd4x20_screenDecalX;
    lcdYDecal=pcbYDecal+lcd4x20_screenDecalY;
    
    /*lcdXDecal=(bx-lcd2x16_screenX)/2;
    lcdYDecal=(by-lcd2x16_screenY)/2;*/
 
    difference()
    {
        cube([bx,by,bThick]);
        translate([lcdXDecal,lcdYDecal,-bThick])
            cube([lcd4x20_screenX,lcd4x20_screenY,lcd4x20_screenZ]);
    }
    
    /*color("green")
    {
        translate([pcbXDecal,pcbYDecal,+bThick])
            cube([lcd2x16_circuitX,lcd2x16_circuitY,lcd2x16_circuitZ]);
    }*/
    
    rp1=(lcdHolesR+bThick)*1.5;
    rp2=(lcdHolesR+bThick)*1;
    h1=lcd4x20_screenZ/2;
    h2=lcd4x20_screenZ;
    holeR=lcdHolesR;
    translate([pcbXDecal,pcbYDecal,+bThick])
    {
        translate([lcdHolesR+lcd4x20HolesDistFromEdge,lcdHolesR+lcd4x20HolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcd4x20_circuitX-lcdHolesR-lcd4x20HolesDistFromEdge,lcdHolesR+lcd4x20HolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcdHolesR+lcd4x20HolesDistFromEdge,lcd4x20_circuitY-lcdHolesR-lcd4x20HolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
        
        translate([lcd4x20_circuitX-lcdHolesR-lcd4x20HolesDistFromEdge,lcd4x20_circuitY-lcdHolesR-lcd4x20HolesDistFromEdge,0])
            //cylinder(r=lcdHolesR*bThick,h=lcd2x16_screenZ-bThick,$fn=16);
        screwSupport(rp1,rp2,h1,h2,holeR);
    }
}




//bezel4x20(bezel4x20X,bezel4x20Y,bezel4x20WallsThickness,3/2);




