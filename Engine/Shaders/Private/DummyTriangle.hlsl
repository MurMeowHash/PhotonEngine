static const float2 kTriangleVertices[3] = {
    float2(0.0, -0.5),
    float2(-0.5, 0.5),
    float2(0.5, 0.5)
};

static const float3 kTriangleColors[3] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

struct DummyTriangleVSInput {
    uint VertexIndex  : SV_VertexID;
};

struct DummyTriangleVSOutput {
    float4 Position : SV_Position;
    float3 Color : COLOR0;
};

DummyTriangleVSOutput DummyTriangleVS(DummyTriangleVSInput Input) {
    DummyTriangleVSOutput Output;
    Output.Position = float4(kTriangleVertices[Input.VertexIndex], 0.0, 1.0);
    Output.Color = kTriangleColors[Input.VertexIndex];
    return Output;
}

float4 DummyTriangleFS(DummyTriangleVSOutput Input) : SV_Target0 {
    return float4(Input.Color, 1.0);
}
