function ScriptName()
	return "bananaskin"
end

xstripes = 60
ystripes = 67

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
	c = GetStripes( x, y, xstripes, ystripes ) * 128;
	return c,c,c,255;
end

function clamp( x, a, b )
	if x < a then
		return a
	else
		if x > b then
			return b
		else
			return x
		end
	end
end

function GetRGBA( x, y )
	yellow = { 240, 210, 60 }
	brown = { 130, 80, 50 }
	ripe = 0
	if x < 3 or y < 3 or x > 250 or y > 250 then
		ripe = 1
	end
	ripe = clamp( ripe + clamp( Perlin( x, y*3, 1, 8, 100 ) * 3 - 4, 0, 1 ), 0, 1 )
	ripe = clamp( ripe + clamp( Perlin( x, y*1, 2, 16, 100 ) * 9 - 11, 0, 1 ), 0, 1 )
	unripe = 1 - ripe
	r = unripe*yellow[1] + ripe*brown[1]
	g = unripe*yellow[2] + ripe*brown[2]
	b = unripe*yellow[3] + ripe*brown[3]
	return r,g,b,255
end


