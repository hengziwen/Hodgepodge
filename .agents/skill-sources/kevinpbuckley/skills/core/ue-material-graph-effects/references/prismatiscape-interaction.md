# Prismatiscape interaction notes

Inspected Prismatiscape 1.03.1 for UE 5.3, installed locally with the PissahPewPew project.
These are source and asset-graph findings, not a claim of runtime compatibility with UE 5.8.
Check the installed plugin version before copying an integration. Preserve vendor asset paths.

## Input and material response are separate

`APrismatiscapeManager::Tick` updates its follow location and visibility, clears prior frame
arrays, gathers shapes from registered components, then calls Blueprint `PostTick` and debug
drawing. The C++ constructor leaves ticking disabled initially; inspect the Blueprint defaults
and runtime activation when a manager exists but no input is processed.

Deform and wind components supply capsule endpoint positions/radii plus velocities/strengths.
Interaction bubble components supply position/radius and velocity/strength. Character deform
components gather configurable bone chains from a skeletal mesh. For a rigid vehicle, use
suitable capsule endpoints rather than assuming humanoid bone names exist.

The inspected `Prismatiscape_SampleTrampleDeform` material function samples
`Prismatiscape_Deform_RT`. Its `Prismatiscape_MPC` inputs map world position or pivot into render
target UVs through `Prismatiscape_SampleUVs`; `Prismatiscape_UVMask` limits coverage and optional
`Prismatiscape_CheapBoxBlur` filters it. Outputs are deformation RG and deformation velocity BA,
both labeled as signed -1..1 vectors. A moving source alone is insufficient: the plant material
must sample the output with the matching world-space mapping and feed deformation into WPO.

`EasyMode/Pris_LargeSingleBushWPO` combines object pivot data, a height mask, foliage bending,
pivot pushing, downward pushing, wiggle, wind, and interaction bubble sampling. It includes a
corrected-normal output. This supports two useful design choices: influence the plant through
its pivot/stalk while masking the root, and consider normal correction when large bends leave
lighting inconsistent. Do not infer the complete simulation algorithm from these consumers;
manager Blueprint/Niagara behavior was not fully inspected in this pass.

For multiple actors or persistent deformation, this field-based architecture is relevant.
For simple one-ship proximity bending, the smaller WPO example in the sibling reference may
be sufficient. Check vertical coverage before reusing a ground/surface interaction field for
deep underwater plants.

## References & source material

Paths below are relative to the inspected Prismatiscape plugin root:

- `Prismatiscape.uplugin` (version and engine association).
- `Source/Prismatiscape/Private/Manager/PrismatiscapeManager.cpp` (tick/gather order).
- `Source/Prismatiscape/Private/Components/PrismatiscapeDeformComponentCharacter.cpp`
  (bone-chain capsules, velocity, profile strength).
- `Content/MaterialFunctions/Prismatiscape_SampleTrampleDeform.uasset` (sampling graph).
- `Content/MaterialFunctions/EasyMode/Pris_LargeSingleBushWPO.uasset` (WPO composition).
