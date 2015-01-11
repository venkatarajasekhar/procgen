#include "geom.h"

Vec3 gXVec3( 1.0f, 0.0f, 0.0f );
Vec3 gYVec3( 0.0f, 1.0f, 0.0f );
Vec3 gZVec3( 0.0f, 0.0f, 1.0f );
Vec3 gZeroVec3( 0.0f, 0.0f, 0.0f );
Vec3 gOneVec3( 1.0f, 1.0f, 1.0f );

Vec4 gXVec4( 1.0f, 0.0f, 0.0f, 0.0f );
Vec4 gYVec4( 0.0f, 1.0f, 0.0f, 0.0f );
Vec4 gZVec4( 0.0f, 0.0f, 1.0f, 0.0f );
Vec4 gWVec4( 0.0f, 0.0f, 0.0f, 1.0f );
Vec4 gZeroVec4( 0.0f, 0.0f, 0.0f, 0.0f );

Mat44 gIdentityMat( gXVec4, gYVec4, gZVec4, gWVec4 );

float Mat44::Determinant() const
{
	return 0
		+ x.x *
		( 0 + y.y * ( z.z * w.w - w.z * z.w )
		- y.z * ( z.y * w.w - w.y * z.w )
		+ y.w * ( z.y * w.z - w.y * z.z )
		)
		- x.y *
		( 0 + y.x * ( z.z * w.w - w.z * z.w )
		- y.z * ( z.x * w.w - w.x * z.w )
		+ y.w * ( z.x * w.z - w.x * z.z )
		)
		+ x.z *
		( 0 + y.x * ( z.y * w.w - w.y * z.w )
		- y.y * ( z.x * w.w - w.x * z.w )
		+ y.w * ( z.x * w.y - w.x * z.y )
		)
		- x.w *
		( 0 + y.x * ( z.y * w.z - w.y * z.z )
		- y.y * ( z.x * w.z - w.x * z.z )
		+ y.z * ( z.x * w.y - w.x * z.y )
		);
}

Mat44 Mat44::Inverse() const {
	float det = 1.0f / Determinant();

	Mat44 tRV(
			Vec4(
				det * ( y.y * z.z - y.z * z.y ),
				det * ( x.z * z.y - x.y * z.z ),
				det * ( x.y * y.z - x.z * y.y ),
				0.0f ),

			Vec4(
				det * ( y.z * z.x - y.x * z.z ),
				det * ( x.x * z.z - x.z * z.x ),
				det * ( x.z * y.x - x.x * y.z ),
				0.0f ),

			Vec4(
				det * ( y.x * z.y - y.y * z.x ),
				det * ( x.y * z.x - x.x * z.y ),
				det * ( x.x * y.y - x.y * y.x ),
				0.0f ),
			gZeroVec4 );

	tRV.w = Vec4(
			- ( w.x * tRV.x.x + w.y * tRV.y.x + w.z * tRV.z.x ),
			- ( w.x * tRV.x.y + w.y * tRV.y.y + w.z * tRV.z.y ),
			- ( w.x * tRV.x.z + w.y * tRV.y.z + w.z * tRV.z.z ),
			1.0f );

	return tRV;
}
