function ScriptName()
	return "wood"
end

function BaseColour( ring )
	tint = Noise( ring, 1, 1 )
	return 240 - 20 * tint, 160 - 20 * tint, 80 + 20 * tint
end

function RingColour( radius )
	radius = radius + Perlin( radius*16,7, 32,256,100 ) * 1.9
	ring = math.floor(radius)
	ringPos = radius - ring
	a = ringPos * ringPos
	a = a * a * a
	b = 1 - a

	R,G,B = BaseColour( ring )

	return R -140*a, G-110*a, B-50*a, 255
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


