
//chassisInternalX=140;
//chassisInternalY=72;

//radius of generic screw holes
genericHoleRadius=3/2;
//piAX=61.5;
piAX=65; //real value
piBX=85;
piBY=56;
piBZ=1.6;
piBPortsHeight=16;
piBPortsX=22;
piBPortsXShift=2;

piHolesRadius=2.75/2;
piHoleCenterDistFromEdge=3.5;
piDistFrontBackHoles=58;

piHolesZ=20;


piHolesRadius=3/2;
//piHolesRadius=2.75/2;
piHoleCenterDistFromEdge=3.5;
piDistFrontBackHoles=58;




beamsThickness=2.5;
wallsThickness=0.8;


//the rack was too high for the case.
//We squished it by this much to make it fit.
// Set it to 0 if not needed.
chassisYSizeReduction=7;

chassisInternalX=piAX-beamsThickness+0.5;
chassisInternalY=piBY-chassisYSizeReduction;

motorBracketHoleRadius=3/2;

padding=0.1;

sideWallsHeight=beamsThickness;
sideWalls2Height=beamsThickness;

chassisX=chassisInternalX-beamsThickness;
chassisY=beamsThickness+chassisInternalY;

screwHolesRadius=2/2;

circuit1X=piBX;
circuit1Y=piBY;
circuit1ScrewsRadius=2/2;
circuit1ScrewsDistFromEdge=piHoleCenterDistFromEdge;
circuit1ScrewPillarsH=8;


h1Shift=15;
h2Shift=30;

fixationHoles1Radius=3.1/2;
fixationHoles2Radius=3/2;

h0=5;
screwBlockPadding=1.5;
screwBlockY=5;
screwBlockY2=7;
screwBlockX=fixationHoles1Radius*2+screwBlockPadding*2;

supportBackThickness=2;



zShift01=supportBackThickness+1;

//motor bracket holes
module motorBracketHole() 
{
	translate([0,0,-padding/2]) 
	{
    	cylinder(r=motorBracketHoleRadius, beamsThickness+padding, center = false,$fn=16);
	}
}

module rpiPlusHoles()
{
	translate([piHoleCenterDistFromEdge,piHoleCenterDistFromEdge,-padding/2]) 
		{
			cylinder(r=piHolesRadius, h=piHolesZ+padding,$fn=16);
		}

		translate([piHoleCenterDistFromEdge,piBY-piHoleCenterDistFromEdge,-padding/2]) 
		{
			cylinder(r=piHolesRadius, h=piHolesZ+padding,$fn=16);
		}

		translate([piHoleCenterDistFromEdge+piDistFrontBackHoles,piHoleCenterDistFromEdge,-padding/2]) 
		{
			cylinder(r=piHolesRadius, h=piHolesZ+padding,$fn=16);
		}

		translate([piHoleCenterDistFromEdge+piDistFrontBackHoles,piBY-piHoleCenterDistFromEdge,-padding/2]) 
		{
			cylinder(r=piHolesRadius, h=piHolesZ+padding,$fn=16);
		}
}

module rpiPlusCommonPorts(portsLength01=10)
{
	portsClearance=2;
	usbPortX=7.2;
	usbPortY=6;
	usbPortZ=3.2;
	usbPortXShift=10.6;
	hdmiPortXShift=32;
	hdmiPortX=15;
	hdmiPortY=portsLength01+3;
	hdmiPortZ=6;
	audioPortRadius=6;
	audioPortXShift=53.5;

	translate([usbPortXShift,4,usbPortZ/2])
		rotate([90,90,0])
			color("gray")
				portCutOut(usbPortZ/2, usbPortX/2, usbPortY);

	translate([hdmiPortXShift,11,hdmiPortZ/2])
		rotate([90,90,0])
			color("gray")
				portCutOut(hdmiPortZ/2, hdmiPortX/2, hdmiPortY);

	translate([audioPortXShift,11,hdmiPortZ/2])
		rotate([90,90,0])
			color("gray")
				portCutOut(audioPortRadius/2, 0, hdmiPortY);

}

module portCutOut(radius01, centersDist01, thickness01)
{
	hull()
	{
	translate([0,-centersDist01,0])
		cylinder(r=radius01, thickness01);

	translate([0,centersDist01,0])
		cylinder(r=radius01, thickness01);
	}
}

module rpiBplusPCB() 
{
		difference()
		{
    		cube([piBX, piBY, piBZ], center = false);
			rpiPlusHoles();
		}
}

module rpiBplus() 
{
	color("green")
		{rpiBplusPCB();}
	translate([piBX-piBPortsX+piBPortsXShift,0,piBZ-padding])
	color("gray")
		{cube([piBPortsX,piBY,piBPortsHeight+padding]);}
	translate([0,0,piBZ])
	rpiPlusCommonPorts();
}

module openBasePlate2(nbBeamsX=3,nbBeamsY=2,thickness01=5)
{
	xShift=chassisX/(nbBeamsX+1);
	for (i =[0:nbBeamsX+1])
	{
		translate([xShift*i,0,0])
			cube([beamsThickness,chassisY,thickness01]);
	}

	yShift=chassisY/(nbBeamsY+1);
	for (i =[0:nbBeamsY+1])
	{
		translate([0,yShift*i,0])
			cube([chassisX+beamsThickness,beamsThickness,thickness01]);
	}
}

module circuit1Plate()
{
   //top holes
	translate([0,circuit1ScrewsDistFromEdge,0])
		difference()
		{
			cylinder(r=beamsThickness,h=circuit1ScrewPillarsH,$fn=32);
			motorBracketHole();
		}

	translate([0,circuit1Y-circuit1ScrewsDistFromEdge,0])
		difference()
		{
			cylinder(r=beamsThickness,h=circuit1ScrewPillarsH,$fn=32);
			motorBracketHole();
		}

	screwBeamYShift=0;
	screwBeamYShift=max(chassisYSizeReduction-5,0);

  translate([0,screwBeamYShift,0])
		cube([beamsThickness*2+(circuit1ScrewsDistFromEdge-beamsThickness),chassisY,beamsThickness]);
			
        
        //back holes
	translate([piDistFrontBackHoles+0,circuit1ScrewsDistFromEdge,0])
		difference()
		{
			cylinder(r=beamsThickness,h=circuit1ScrewPillarsH,$fn=32);
			motorBracketHole();
		}

	translate([piDistFrontBackHoles+0,circuit1Y-circuit1ScrewsDistFromEdge,0])
		difference()
		{
			cylinder(r=beamsThickness,h=circuit1ScrewPillarsH,$fn=32);
			motorBracketHole();
		}

     translate([piDistFrontBackHoles-beamsThickness*2-1,screwBeamYShift,0])
			cube([beamsThickness*2+circuit1ScrewsDistFromEdge-beamsThickness,chassisY,beamsThickness]);		
}



module standOffSimple(internalRadius=1.5,wallsThickness=1.5,height01=5,baseThickness=1)
{
	difference()
	{
		cylinder(r=internalRadius+wallsThickness,h=height01,$fn=32);
		translate([0,0,-padding/2])
			cylinder(r=internalRadius,h=height01+padding,$fn=32);
	}
	if(baseThickness>0)
	{
		cylinder(r=internalRadius,h=baseThickness,$fn=32);
	}
}

module standOffWithBaseCone(internalRadius=1.5,wallsThickness=1.5,height01=6,baseThickness=1,coneRadiusAdd=2,coneHeight=4)
{
	standOffSimple(internalRadius,wallsThickness,height01,baseThickness);
	difference()
	{
		cylinder(r1=internalRadius+wallsThickness+coneRadiusAdd,r2=internalRadius+wallsThickness,h=coneHeight,$fn=32);
		translate([0,0,-padding/2])
			cylinder(r=internalRadius,h=height01+padding,$fn=32);
	}
}



module piSupport1(nbBeamsX=3,nbBeamsY=2,supportThickness=3)
{
 difference()
 {
  union()
  {
	openBasePlate2(nbBeamsX,nbBeamsY,supportThickness);
	translate([beamsThickness,beamsThickness-chassisYSizeReduction/2,0])
	{
		circuit1Plate();
	}
  }
	translate([-1,beamsThickness-chassisYSizeReduction/2,0.5])
	{
		rpiPlusHoles();
	}
 }
}




module piSupport2(nbBeamsX=3,nbBeamsY=2,supportThickness=3)
{
	difference()
	{
		piSupport1(nbBeamsX,nbBeamsY,supportThickness);
        
        
        translate([beamsThickness*2,15,supportThickness/2])
        cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);
        
        translate([beamsThickness*2,chassisY-15+beamsThickness,supportThickness/2])
        cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);
        
        
        translate([chassisX-beamsThickness,15,supportThickness/2])
        cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);
        
        translate([chassisX-beamsThickness,chassisY-15+beamsThickness,supportThickness/2])
        cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);
        
		
		translate([0+h1Shift,chassisY,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);

		/*translate([0+h2Shift,chassisY,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness-h2Shift,chassisY,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);*/	

		translate([chassisX+beamsThickness-h1Shift,chassisY,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);


		translate([0+h1Shift,0,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);

		/*translate([0+h2Shift,0,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness-h2Shift,0,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);*/	

		translate([chassisX+beamsThickness-h1Shift,0,supportThickness/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=beamsThickness*2+padding,$fn=16,center=true);
	}
}


module fixationSupportNoHoles()
{
	color([1,0,1])
	{
		translate([h1Shift-screwBlockPadding-fixationHoles1Radius,chassisY-screwBlockY,0])
		cube([screwBlockX,screwBlockY,h0]);

		translate([h2Shift-screwBlockPadding-fixationHoles1Radius,chassisY-screwBlockY,0])
		cube([screwBlockX,screwBlockY,h0]);

		translate([chassisX+beamsThickness-h1Shift-screwBlockPadding-fixationHoles1Radius,chassisY-screwBlockY,0])
		cube([screwBlockX,screwBlockY,h0]);

		translate([chassisX+beamsThickness-h2Shift-screwBlockPadding-fixationHoles1Radius,chassisY-screwBlockY,0])
		cube([screwBlockX,screwBlockY,h0]);

		translate([0,chassisY-screwBlockY,-supportBackThickness])
		cube([chassisX,screwBlockY,supportBackThickness]);

		translate([0,0,-1*supportBackThickness])
			openBasePlate2(2,2,supportBackThickness);

		translate([-1,-supportBackThickness,-supportBackThickness])
			cube([chassisX+beamsThickness+2,supportBackThickness,supportBackThickness+h0]);


		//BOTTOM FIXATION BLOCKS
		translate([-screwBlockX-1,-supportBackThickness,-supportBackThickness])
		{
			cube([screwBlockX,screwBlockY2+supportBackThickness,h0+supportBackThickness]);
			cube([screwBlockX+1,screwBlockY2+supportBackThickness,supportBackThickness]);
		}

		translate([chassisX+1+beamsThickness,-supportBackThickness,-supportBackThickness])
		{
			cube([screwBlockX,screwBlockY2+supportBackThickness,h0+supportBackThickness]);
			translate([-1,0,0])
				cube([screwBlockX+1,screwBlockY2+supportBackThickness,supportBackThickness]);
		}


		translate([0+h1Shift,1,h0/2])
			rotate([-90,0,0])
				cylinder(r1=fixationHoles1Radius,r2=0, h=beamsThickness,$fn=16,center=true);

		translate([0+h2Shift,1,h0/2])
			rotate([-90,0,0])
				cylinder(r1=fixationHoles1Radius,r2=0, h=beamsThickness,$fn=16,center=true);

		translate([chassisX+beamsThickness-h2Shift,1,h0/2])
			rotate([-90,0,0])
				cylinder(r1=fixationHoles1Radius,r2=0, h=beamsThickness,$fn=16,center=true);	

		translate([chassisX+beamsThickness-h1Shift,1,h0/2])
			rotate([-90,0,0])
				cylinder(r1=fixationHoles1Radius,r2=0, h=beamsThickness,$fn=16,center=true);


		//TOP REINFORCEMENT FIXATION BLOCKS
		translate([-screwBlockX-1,chassisY-screwBlockY2,-supportBackThickness])
		{
			cube([screwBlockX,screwBlockY2+supportBackThickness,h0+supportBackThickness]);
			cube([screwBlockX+1,screwBlockY2+supportBackThickness,supportBackThickness]);
		}

		translate([chassisX+1+beamsThickness,chassisY-screwBlockY2,-supportBackThickness])
		{
			cube([screwBlockX,screwBlockY2+supportBackThickness,h0+supportBackThickness]);
				translate([-1,0,0])
			cube([screwBlockX+1,screwBlockY2+supportBackThickness,supportBackThickness]);
		}
	}
}

module fixationSupport()
{
	difference()
	{
		fixationSupportNoHoles();

		translate([0+h1Shift,chassisY,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles2Radius, h=screwBlockX*2+padding,$fn=16,center=true);

		translate([0+h2Shift,chassisY,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles2Radius, h=screwBlockX*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness-h2Shift,chassisY,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles2Radius, h=screwBlockX*2+padding,$fn=16,center=true);	

		translate([chassisX+beamsThickness-h1Shift,chassisY,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles2Radius, h=screwBlockX*2+padding,$fn=16,center=true);


		//---------------------------------------------------------------------------------//
		translate([0+h1Shift+(h2Shift-h1Shift)/2,0,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=supportBackThickness*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness-h2Shift+(h2Shift-h1Shift)/2,0,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=supportBackThickness*2+padding,$fn=16,center=true);	

		translate([-screwBlockX/2-1,0,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=screwBlockY2*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness+screwBlockX/2+1,0,h0/2])
			rotate([90,0,0])
				cylinder(r=fixationHoles1Radius, h=screwBlockY2*2+padding,$fn=16,center=true);	


		translate([-screwBlockX/2-1,chassisY-screwBlockY2/2,h0/2])
			rotate([0,90,0])
				cylinder(r=fixationHoles1Radius, h=screwBlockY2*2+padding,$fn=16,center=true);

		translate([chassisX+beamsThickness+screwBlockX/2+1,chassisY-screwBlockY2/2,h0/2])
			rotate([0,90,0])
				cylinder(r=fixationHoles1Radius, h=screwBlockY2*2+padding,$fn=16,center=true);	
	}
}

/*
spaceBetweenRpis=50;

translate([chassisX+screwBlockX+10,chassisY-screwBlockY2,0])
cube([spaceBetweenRpis*2+(h0+supportBackThickness)*3,screwBlockY2,beamsThickness]);
*/


/*
color([1,0.5,0])
piSupport2(1,1,h0);
*/
/*
translate([0,circuit1Y+15,0])
piSupport2(1,1,h0);*/





//cube([6,circuit1Y*2+10+20,3]);


//translate([4,-6,h0])
//standOffWithBaseCone(1.5,1.5,3,1,2,2);


//fixationSupport();


	
	/*translate([beamsThickness+piAX,beamsThickness-chassisYSizeReduction/2+piBY,h0])
		rotate(180,0,0)
		rpiBplus() ;*/
