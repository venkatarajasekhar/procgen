function ScriptName()
	return "grass"
end

function GetLand( x, y )
	d = 128
	dx = Perlin( x, y, 32, 256, 100 )*d
	dy = Perlin( x, y, 32, 256, 101 )*d
	x = x + dx
	y = y + dy
	a = Perlin( x, y, 2, 16, 101 )
	b = Perlin( x, y, 2, 16, 100 )
	h = a-b
	if a < b then
		h = b-a
	end
	c = Sin( h*4 ) * 32
	h = Sin( h ) * 128 + 128
	return 0.8*(h+c),h+c,0.5*h,255
end

function GetRGBA( x, y )
	--return GetStripey(x,y)
	return GetLand(x,y)
end


