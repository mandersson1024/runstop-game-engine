
SamplerState Sampler : register(s0);
Texture2D<float4> EquirectangularInputTexture : register(t0);
RWTexture2DArray<float4> OutputTextureCube : register(u0);

float2 CubeCoordinateToEquirectangularUV(float3 direction)
{
    static const float2 invAtan = float2(0.15915494309, 0.31830988618);

    float2 uv = float2(atan2(direction.x, direction.z), asin(direction.y));
    uv *= invAtan;
    uv += 0.5;

    return uv;
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float width, height, arraySize;
    OutputTextureCube.GetDimensions(width, height, arraySize);
    float d = (width - 1) / 2;

    float x = DTid.x;
    float y = DTid.y;
    float3 cubePixelPos = float3(0, 0, 0);

    switch (DTid.z)
    {
    case 0: // right
        cubePixelPos = float3(d, y - d, d - x);
        break;
    case 1: // left
        cubePixelPos = float3(-d, y - d, x - d);
        break;
    case 2: // up
        cubePixelPos = float3(x - d, -d, y - d);
        break;
    case 3: // down
        cubePixelPos = float3(x - d, d, d - y);
        break;
    case 4: // forward
        cubePixelPos = float3(x - d, y - d, d);
        break;
    case 5: // back
        cubePixelPos = float3(d - x, y - d, -d);
        break;
    }

    //float3 direction = normalize(cubePixelPos + float3(0.5, 0.5, 0.5));
    float3 direction = normalize(cubePixelPos);
    float2 uv = CubeCoordinateToEquirectangularUV(direction);
    OutputTextureCube[DTid.xyz] = EquirectangularInputTexture.SampleLevel(Sampler, uv, 0);
}

