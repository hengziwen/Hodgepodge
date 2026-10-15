# Unreal architecture decision matrices

Use these matrices while planning systems that cross object lifetime, networking, persistence,
data authoring, or presentation. They are selection aids, not substitutes for inspecting the
project's established architecture.

## Runtime owner and lifetime

| Need | Prefer | Important constraint |
|---|---|---|
| World presence, transform, collision, or independent replication | `AActor` | World-bound; spawning many tiny Actors has real overhead |
| Reusable behavior owned by an Actor | `UActorComponent` | Lifetime follows owner; no transform |
| Reusable behavior with attachment/transform | `USceneComponent` | Belongs in an Actor's component hierarchy |
| Lightweight structured runtime object | `UObject` | No world transform or independent Actor replication |
| Server-only match rules and spawn policy | `AGameModeBase` / `AGameMode` | Does not exist on clients |
| Replicated match-wide observable state | `AGameStateBase` / `AGameState` | Exists on server and clients |
| Per-connection commands, input-facing orchestration, owner-only state | `APlayerController` | Each client normally sees only its own controller |
| Replicated per-player state that survives pawn replacement | `APlayerState` | Appropriate for identity, score, team, and durable match state |
| Replaceable in-world player embodiment | `APawn` / `ACharacter` | Can be destroyed and repossessed |
| Local state that survives map travel | `UGameInstance` | One per process; not replicated |
| Engine-wide service | `UEngineSubsystem` | Lives as long as the engine instance |
| Per-process/game service across travel | `UGameInstanceSubsystem` | Local; not automatically replicated |
| Per-world service | `UWorldSubsystem` | Recreated with the world |
| Per-local-player service or presentation state | `ULocalPlayerSubsystem` | One per local player, including split-screen |
| Static designer-authored definition | `UDataAsset` / `UPrimaryDataAsset` | Treat as definition data, not mutable session state |
| Persistent serialized snapshot | `USaveGame` | Explicitly copy runtime state in and out |

### Ownership test

Ask these in order:

1. Does it need a world transform, collision, independent relevancy, or independent spawning?
   If yes, consider an Actor.
2. Is it behavior inseparable from one Actor instance? Consider an Actor Component.
3. Is it a service whose lifetime matches Engine, GameInstance, World, or LocalPlayer? Consider
   the matching Subsystem.
4. Is it gameplay state already owned by a framework role? Put it on that role before inventing a
   service.
5. Is it immutable authored data or mutable runtime state? Keep those separate.

## State placement

| State kind | Typical home | Notes |
|---|---|---|
| Authoritative rules | GameMode or authoritative domain Actor/Component | Never expect a client to read GameMode |
| Match-wide replicated state | GameState or replicated match-state Actor | Optimize update frequency and relevancy |
| Per-player replicated state surviving death | PlayerState or its replicated component | Avoid Pawn for durable match state |
| Pawn-local transient state | Pawn/Character or component | Movement and current embodiment belong here |
| Owner-only local presentation state | PlayerController, LocalPlayerSubsystem, or widget model | Do not replicate presentation-only values |
| Cross-level local session state | GameInstance or GameInstanceSubsystem | Not a substitute for server state |
| Cross-session persistence | SaveGame/profile/backend | Define versioning and migration |
| Static item/ability/enemy definitions | Primary Data Asset/Data Table | Runtime instances refer to stable IDs or soft assets |

For every mutable field, identify one authority. Caches and UI projections may duplicate values,
but their source and invalidation path must be explicit.

## Communication choices

| Relationship | Prefer | Avoid |
|---|---|---|
| Owner calling a known child/component | Direct typed call | Global event bus for a local relationship |
| Caller needs a capability across unrelated classes | Unreal interface | Casting through a long class list |
| One local source, one or many observers | Native/dynamic delegate or Event Dispatcher | Polling every frame |
| Blueprint needs to subscribe | Dynamic multicast delegate with `BlueprintAssignable` | A C++-only delegate Blueprint cannot see |
| Decoupled local feature broadcast by tag | Gameplay Message Subsystem when already adopted | Introducing it for a single direct relationship |
| Client asks authority to perform an action | Server RPC on an owned replicated object | Client mutating authoritative state |
| Server communicates transient owner-specific result | Client RPC when a property is not the right model | Multicasting private data to everyone |
| Clients need durable shared state | Replicated property, RepNotify, or replicated subobject | RPC-only state that late joiners miss |
| Large frequently changing collection | Fast Array when measurement/requirements justify it | Replicating an entire array for every small delta |

Document event order and teardown. A good communication map answers what happens if the receiver
does not yet exist, the asset is still loading, the sender is destroyed, or a client joins late.

## C++ and Blueprint boundary

Prefer C++ for:

- authority validation, replication declarations, RPCs, and lifecycle-sensitive state;
- reusable components, interfaces, and stable data schemas;
- performance-sensitive loops and code requiring automated tests;
- APIs intended to support several Blueprint implementations.

Prefer Blueprint or assets for:

- composition of concrete Actor and Component setups;
- animation, VFX, sound, UI layout, and presentation flow;
- selecting assets, curves, classes, and designer-tuned values;
- narrow extension events intentionally exposed from a stable C++ base.

Define the seam precisely: base class, exposed properties, callable functions, implementable
events, assignable delegates, and which side owns the invariant. Avoid duplicating the same rule
in C++ and Blueprint.

## Data and loading choices

| Need | Prefer | Trade-off |
|---|---|---|
| Many uniform rows edited in bulk | Data Table | Simple schema; weaker fit for rich inheritance |
| Rich individual definitions and asset references | Data Asset | One asset per definition; easy designer workflow |
| Asset Manager discovery, bundles, controlled loading | Primary Data Asset | Requires Primary Asset configuration |
| Project-wide programmer-facing settings | Developer Settings/config | Global config rather than content data |
| Optional or large content | Soft object/class reference | Requires an explicit async loading and failure path |
| Always-needed small dependency | Hard reference | Simple access but expands load dependencies |
| Runtime state | Plain/USTRUCT/UObject/component state | Do not mutate shared definition assets |
| Disk persistence | SaveGame or external profile/backend | Requires schema versioning and migration policy |

## Multiplayer review

For a networked design, answer all of the following:

- Which machine may initiate the request?
- Which object is owned by that connection and can legally send the Server RPC?
- Where does authority validate range, cost, cooldown, inventory, and permissions?
- Which state is owner-only, relevant to nearby players, or globally replicated?
- How do late joiners reconstruct current state?
- What happens under prediction, latency, rejection, disconnect, death, and seamless travel?
- Are replicated collections or subobjects needed, and is their cost proportional to scale?
- Which notifications are local reactions to replicated state rather than extra multicast RPCs?

## 2D, 3D, and UI mapping

Unreal does not require a separate gameplay ownership model for a 2D game:

- A world-space 2D or 2.5D game still uses Actors, Components, Game Framework classes, and the
  normal networking model. Paper2D assets or an orthographic camera affect representation.
- A 3D world representation uses Scene/Primitive Components attached to Actors.
- Screen-space UI belongs in UMG/Slate and observes gameplay state; it should not become the
  authoritative owner of that state.
- World-space widgets are presentation components attached to Actors, not replacements for the
  underlying gameplay model.

## Plan review checklist

Before handing the design to an implementation agent, confirm:

- Each responsibility has one owner whose lifetime matches it.
- Client-visible state does not live only in GameMode or GameInstance.
- State that must survive pawn replacement is not owned only by the Pawn.
- Static definitions, runtime state, replicated state, and saved state are distinct.
- Network requests originate from an owned object and are authority-validated.
- Durable state uses replication; transient local observation uses delegates/events.
- The C++/Blueprint seam names concrete APIs and extension points.
- Asset references have an intentional loading strategy.
- The first implementation slice proves the riskiest assumption end to end.
- Every phase has an observable verification step, including lifecycle and failure cases.

## Source anchors

The selection rules above are grounded in these UE 5.8 headers:

- `Engine/Source/Runtime/Engine/Classes/GameFramework/Actor.h`
- `Engine/Source/Runtime/Engine/Classes/Components/ActorComponent.h`
- `Engine/Source/Runtime/Engine/Classes/Components/SceneComponent.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/GameModeBase.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/GameStateBase.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/PlayerController.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/PlayerState.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/Pawn.h`
- `Engine/Source/Runtime/Engine/Classes/Engine/GameInstance.h`
- `Engine/Source/Runtime/Engine/Public/Subsystems/Subsystem.h`
- `Engine/Source/Runtime/Engine/Classes/Engine/DataAsset.h`
- `Engine/Source/Runtime/Engine/Classes/GameFramework/SaveGame.h`
- `Engine/Source/Runtime/Net/Core/Classes/Net/Serialization/FastArraySerializer.h`
