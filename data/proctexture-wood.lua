function ScriptName()
	return "wood"
end

function GetRGBA( x, y )
	d = Perlin( x, y, 1, 16, 100 ) * 16
	return 140+d, 110, 70, 255
end


