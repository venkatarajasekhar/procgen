function ScriptName()
	return "sand"
end

function GetLand( x, y )
	d = Perlin( x, y, 1, 16, 100 ) * 16
	return 220+d, 180+d, 100+d, 255
end

function GetRGBA( x, y )
	return GetLand(x,y)
end


