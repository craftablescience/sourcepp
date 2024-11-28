__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE| CLK_ADDRESS_REPEAT | filter;

__kernel void processFace(read_only image2d_t HdriImg, write_only image2d_t faceImg, __global float* startRightUp)//, __global float3 right, __global float3 up)
{
	int2 pixelCoordFace = (int2)(get_global_id(0), get_global_id(1));
	int resolutionFace = get_global_size(0);

	float3 start = (float3)(
		startRightUp[0],
		startRightUp[1],
		startRightUp[2]
	);
	float3 right = (float3)(
		startRightUp[3],
		startRightUp[4],
		startRightUp[5]
	);
	float3 up = (float3)(
		startRightUp[6],
		startRightUp[7],
		startRightUp[8]
	);

	float3 direction = (float3)(
		start.x + (pixelCoordFace.x  * 2.0f + 0.5f)/(float)resolutionFace * right.x + (pixelCoordFace.y * 2.0f + 0.5f)/(float)resolutionFace * up.x,
		start.y + (pixelCoordFace.x  * 2.0f + 0.5f)/(float)resolutionFace * right.y + (pixelCoordFace.y * 2.0f + 0.5f)/(float)resolutionFace * up.y,
		start.z + (pixelCoordFace.x  * 2.0f + 0.5f)/(float)resolutionFace * right.z + (pixelCoordFace.y * 2.0f + 0.5f)/(float)resolutionFace * up.z
	);

	float azimuth = atan2(direction.x, -direction.z) + radians(180.0f);
	float elevation = atan(direction.y / sqrt(pow(direction.x, 2) + pow(direction.z, 2))) + radians(90.0f);

	float2 pixelCoordHdri = (float2)(
		azimuth / radians(360.0f) * get_image_width(HdriImg),
		elevation / radians(180.0f) * get_image_height(HdriImg)
	);

	uint4 pixel = read_imageui(HdriImg, sampler, pixelCoordHdri);
	write_imageui(faceImg, pixelCoordFace, pixel);
}