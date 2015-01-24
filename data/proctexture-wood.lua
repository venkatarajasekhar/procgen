function ScriptName()
	return "wood"
end

function RingColour( radius )
	radius = radius + Perlin( radius,7, 2,64,100 ) * 1.7
	ringPos = radius % 1
	a = ringPos * ringPos
	a = a * a
	b = 1 - a

	return 240 -140*a, 160-110*a, 70-50*a, 255
end
	
function RingLookup( x, y )
	return RingColour( 20*Sqrt(x*x+y*y) )
end

function UVToWorld( x, y )
	return x/256,y/256
end

function GetRGBA( u, v )
	x,y = UVToWorld( u,v )

	X,Y = x-0.2, y * 0.1

	X = X + Perlin( u,v,64,256,101 ) * 0.05
	Y = Y + Perlin( u,v,64,256,101 ) * 0.05
	
	return RingLookup( X,Y )
end


