function ScriptName()
	return "mess"
end

stripes = 4

function GetStripes( x, y, xr, yr )
	a = (x % xr) / xr
	if a > 0.5 then
		a = 1
	else
		a = 0
	end
	b = (y % yr) / yr
	if b > 0.5 then
		b = 1
	else
		b = 0
	end
	if a+b > 1 then
		return 0
	end
	return a+b
end


function GetStripey( x, y )
	c = GetStripes( x, y, 256/stripes, 256/stripes )
	if c > 0.5 then
		return 255,0,255,255
	else
		return 255,255,0,255
	end
end

function GetRGBA( x, y )
	return GetStripey(x,y)
end


