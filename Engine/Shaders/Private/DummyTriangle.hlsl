struct DummyTriangleVSInput {
    float2 Position : POSITION;
    float3 Color : COLOR;
};

struct DummyTriangleVSOutput {
    float4 Position : SV_Position;
    float3 Color : TEXCOORD0;
};

DummyTriangleVSOutput DummyTriangleVS(DummyTriangleVSInput Input) {
    DummyTriangleVSOutput Output;
    Output.Position = float4(Input.Position, 0.0, 1.0);
    Output.Color = Input.Color;
    return Output;
}

float4 DummyTriangleFS(DummyTriangleVSOutput Input) : SV_Target0 {
    return float4(Input.Color, 1.0);
}