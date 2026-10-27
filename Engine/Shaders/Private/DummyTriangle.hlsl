struct DummyTriangleVSInput {
    float2 LocalPosition : POSITION;
};

struct DummyTriangleVSOutput {
    float4 Position : SV_Position;
};

DummyTriangleVSOutput DummyTriangleVS(DummyTriangleVSInput Input) {
    DummyTriangleVSOutput Output;
    Output.Position = float4(Input.LocalPosition, 0.0, 1.0);
    return Output;
}

float4 DummyTriangleFS(DummyTriangleVSOutput Input) : SV_Target0 {
    return float4(1.0, 0.0, 0.0, 1.0);
}