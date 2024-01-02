This is a repro-case for a bug with msvc modules:
- module `yyy` depends on `xxx`
- `xxx` exports a struct which have a `entt::registry` as a member
- the error points to expressions in `entt::registry` which dont seem interpreted as expressions
- the issue only occurs when all these conditions are met:
  - the `xxx`'s type is used;
  - the `entt::registry` is a member of the type (it doesnt appear if it's used in a function or as a namespace scope object);
  - there needs to be 2 module interface files one dependign on the other which exports the type owning the `entt::registry`, the interface file can be either primary or not, it will still happen;
  - there needs to be usage of the type which have the `entt::registry` member, that type needs at least ot be constructed somewhere outside the module partition it have been defined in;

The issue does not appear with clang with the msvc toolchain, so it probably is a msvc issue OR a mix of msvc issue and build2 handling it's output incorrectly or something like that. It requires 2 compilations to misinterpret the code from `entt` anyway.

