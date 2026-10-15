# Rooted plant interaction

Use proximity-driven WPO for visual bending around a moving object. Use a fluid field when
its velocity or persistent wakes are needed, first checking that its bounds and height mapping
cover the plants. A surface simulation does not automatically affect deep seabed vegetation.

## Geometry and coordinate spaces

- Add interaction displacement to existing wind WPO. Use project-owned material copies and
  assign overrides only to the intended plant foliage types.
- Use vertex position before WPO. Transform world position into **instance space** for ISM/HISM
  foliage; component local space can describe the entire component instead of one plant.
  UE 5.8 TransformPosition supports Instance & Particle Space.
- For a roughly upright mesh, start with `RootZ = bounds.origin.z - bounds.extent.z` and
  `PlantHeight = 2 * bounds.extent.z`. Inspect the mesh: a known stem base or painted vertex
  mask is better when bounds include loose leaves, buried geometry, or multiple stalks.
- Transform `(RootX, RootY, RootZ)` and `(RootX, RootY, RootZ + PlantHeight)` from instance
  space to world space. RootX/RootY can be zero when the mesh pivot is on the stem.

Per-vertex object distance can produce almost no visible motion on a tall plant: tips lie
outside the radius while nearby low vertices are suppressed by the root mask. Measure distance
to the root-to-tip segment to influence the whole stalk; use vertex height to weight bending.

## Example displacement

Inputs: unoffset `InstancePosition`, world `RootPosition`, `TipPosition`, and `SourcePosition`,
positive `RadiusCm`, `BendCm`, mesh-local `RootZ` and `PlantHeight`, `Enabled`, and horizontal
`FlowXY` with length clamped to 0..1. This approximates upright vegetation, not rigid
length-preserving deformation. World XY is horizontal and world Z is up.

```hlsl
float h = saturate((InstancePosition.z - RootZ) / max(PlantHeight, 1.0));
float3 stem = TipPosition - RootPosition;
float stemLength = max(length(stem), 1.0);
float t = saturate(dot(SourcePosition - RootPosition, stem) / (stemLength * stemLength));
float3 delta = RootPosition + t * stem - SourcePosition;
float influence = saturate(1.0 - length(delta) / max(RadiusCm, 1.0));
influence = influence * influence * (3.0 - 2.0 * influence);
float2 away = length(delta.xy) > 1.0 ? normalize(delta.xy) : float2(1.0, 0.0);
float2 direction = away * 0.85 + FlowXY * 0.15;
float bend = min(BendCm, stemLength * 0.45) * pow(h, 1.35) * influence * Enabled;
float drop = bend * bend / (2.0 * max(stemLength * h, 1.0));
float3 interactionWPO = float3(direction * bend, -drop);
// Final WPO = existing wind WPO + interactionWPO.
```

These blend weights, exponent, and cap are tuning examples. A 180 cm bend cap and 400 cm radius
made interaction visible for large underwater plants in one tested scene. Tune to the mesh and
vehicle. The fixed centerline fallback avoids undefined normalization but can change direction
across the stem; use a motion-derived fallback or smoothed field if visible. Distance fading
alone provides no inertia or lingering wake.

## Runtime ownership

A Material Parameter Collection suits one shared source per world. Two vector parameters can
carry `(position.xyz, radius)` and `(flow.xy, 0, enabled)`. Write after movement; clear enabled
when the source disappears. Inspect values in the PIE world, not only the asset defaults.
Each client can drive a local response; multiple vehicles need a different representation.

## Diagnose missing movement

1. Inspect the material on the actual foliage component and every mesh slot. A saved parent
   graph does not prove that its instances or component overrides are assigned.
2. Check component WPO evaluation and disable distance. Expand render bounds for maximum
   displacement, considering shadow/culling cost. WPO does not move collision geometry.
3. Apply/compile the changed graph successfully before saving. Programmatically saving graph
   edits alone can leave the previous shader map active.
4. Read live interaction values. Briefly exaggerate amplitude through a transient runtime
   instance. If that works, investigate spatial masks and scale before rewriting input plumbing.
5. When gameplay validation is authorized, compare enabled/disabled output with fixed wind and
   camera. Include tall plants, rotated/scaled instances, close passage, departure, and distant
   plants. Establish the view before pausing: camera movement may not update the paused view.
   Keep diagnostic overrides transient.

## References & source material

- UE 5.8: `Engine/Source/Runtime/Engine/Public/Materials/MaterialExpressionTransformPosition.h`
  (`EMaterialPositionTransformSource`, `TRANSFORMPOSSOURCE_Instance`).
- UE 5.8: `Engine/Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h`
  (`bEvaluateWorldPositionOffset`, `WorldPositionOffsetDisableDistance`).
- UE 5.8: `Engine/Source/Runtime/Engine/Classes/Components/PrimitiveComponent.h` (`BoundsScale`).
- Applied example: Proteus TrainingPool, 2026-09-29; per-instance height masking and
  root-to-tip proximity produced visible bending while preserving original plant wind.
