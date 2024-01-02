This is a repro-case for a bug with msvc modules.

Reported to Visual Studio team: https://developercommunity.visualstudio.com/t/Same-member-function-using-noexcept-expr/10551687

## How To Reproduce

To reproduce with Visual Studio 2022 (preview  or not) check the `.bat` scripts provided and change the msvc toolchain to match your VS install, then run them:
- `./repro-success.bat` or ( `b liba{success}` using `build2` ) : no problem, as expected
    - what it does: compiles `source_ok.cxx` which includes `yyy.hxx` which provides the type `Y` which have a `Q<int>` as member, coming from `someheader.hxx`
- `./repro-fail.bat` or ( `b liba{success}` using `build2` ) : build errors (with msvc), unexpected
    - what it does: compiles `source_failure.cxx` which imports module `xxx` defined in `xxx.mxx` which provides the type `X` which have a `Q<int>` as member, coming from `someheader.hxx`
(note that `Y` and `X` are structurally exactly the same, the only difference is that one is defined and exported from a named module, the other is just in header).

Here is what I observe (which is unexpected):
```
$ ./repro-fail.bat

E:\tests\repro-entt-in-modules\mylib>"C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.39.33321\bin\Hostx64\x64\cl" /nologo /std:c++latest /Zc:__cplusplus /permissive- /I "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.39.33321\include" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\ucrt" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\shared" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\um" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\winrt" /utf-8 /EHsc /MD /ifcOutput xxx.lib.ifc /Fo: xxx.lib.ifc.obj /c /TP /interface E:\tests\repro-entt-in-modules\mylib\xxx.mxx
xxx.mxx

E:\tests\repro-entt-in-modules\mylib>"C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.39.33321\bin\Hostx64\x64\cl" /nologo /std:c++latest /Zc:__cplusplus /permissive- /I "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.39.33321\include" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\ucrt" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\shared" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\um" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\winrt" /utf-8 /EHsc /MD /reference xxx=xxx.lib.ifc /Fo: source_fail.lib.obj /c /TP E:\tests\repro-entt-in-modules\mylib\source_fail.cxx
source_fail.cxx
E:\tests\repro-entt-in-modules\mylib\someheader.hxx(8): error C2760: syntax error: ')' was unexpected here; expected 'expression'
E:\tests\repro-entt-in-modules\mylib\someheader.hxx(8): note: the template instantiation context (the oldest one first) is
E:\tests\repro-entt-in-modules\mylib\xxx.mxx(9): note: see reference to class template instantiation 'Q<int>' being compiled
E:\tests\repro-entt-in-modules\mylib\someheader.hxx(8): error C2057: expected constant expression
E:\tests\repro-entt-in-modules\mylib\someheader.hxx(8): error C2059: syntax error: ')'
```

Important steps that reproduces the issue:
- see `someheader.hpp` for this:
    ```
    template<class T>
    struct Q
   {
       constexpr Q() noexcept(std::is_nothrow_copy_constructible_v<T>) = default; // HERE
    };
    ```
- When `Q<int>` is a member of a type which is defined in a module, then exported, then used somewhere -> error
- If `X` was not exported and used, there would be no error.


## Additional Information

This issue was first discovered in a `build2` project which uses only C++ modules for it's own code. The bat files were crafted using the verbose output from `build2` specifying the build commands. I did not remove or add any flag so far.
The project in question builds with both `clang` (17 and 18-trunk versions) and `msvc` (preview) when using `build2`.
The issue appeared after including [this header](https://github.com/skypjack/entt/blob/v3.12.2/src/entt/entt.hpp) in a module interface and using `entt::registry` as a member of any type.
The code from `someheader.hpp` is a minimal reproduction of [this line in this header](https://github.com/skypjack/entt/blob/v3.12.2/src/entt/core/compressed_pair.hpp#L121) where the same "expected an expression" error  is pointed by the compiler as soon as `entt::registry` is a member of the type exported from a module.

This also means that anyone attempting to use the `entt` library in C++ module code with `msvc` will immediately fail to build, as `entt::registry` is the main type to use with this library.

The issue does not appear with clang-17 with the msvc toolchain/stl (you can build successfully with `build2` on windows with clang/msvc-toolchain using `b config.cxx=clang++` for example). This and the (at least apparent to me ) simplicity of the commands to invoke the error leads me to think it is not a build-system issue but a compiler issue, in the context of C++ modules usage.

