# KitsuneEngine

An engine made using raylib and CMake.

## Build out of the box (local vendored dependencies)

Dependencies are committed as git submodules in `third_party/`:
- `raylib`
- `raygui`
- `lua`
- `sol2`
- `nlohmann_json`

Pinned versions:
- `raylib` `6.0`
- `raygui` `5.0`
- `lua` `5.4.8`
- `sol2` `3.5.0`
- `nlohmann_json` `3.12.0`

Clone with submodules:

```bash
git clone --recurse-submodules <repo-url>
```

If you already cloned:

```bash
git submodule update --init --recursive
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

Open in IDEs:
- Visual Studio: `File -> Open -> CMake...` and select the repository root.
- CLion: open the project root, CMake will pick vendored deps automatically.

## Optional: use system/package-manager dependencies

You can use package-managed dependencies for `raylib`, `nlohmann_json`, `lua`, and `sol2` by enabling:

```bash
cmake -S . -B build -DKITSUNE_USE_SYSTEM_DEPS=ON
```

Note: `raygui` is still used from `third_party/raygui`, so submodules are still required.
