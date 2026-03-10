# shapr3D C++ Take-Home

A 3D mesh file converter with point containment, surface area, and volume analysis - built for Shapr3D. Built on Arch Linux with GCC and C++20.

## Getting Started

First, clone this repository, and once inside, run the following commands:

```bash
git submodule update --init --recursive
make
```

If you utilize clangd for your coding setup, you can run `make configure` to generate a `compile_commands.json` file.

## Dependencies

All dependencies are vendored under `vendor/`

Libraries used:
[GLM](https://github.com/g-truc/glm) - Vector/matrix math
[tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) - OBJ parsing
[Catch2](https://github.com/catchorg/Catch2) - Testing framework

## Building

```bash
make          # debug build
make release  # release build
make test     # build + run tests
make clean    # remove build artifacts
make format   # clang-format all sources
```

## Usage

```bash
# Convert OBJ to binary STL
shapr --input model.obj --output model.stl

# With transforms (can be combined in any order)
shapr --input model.obj --output model.stl --translate 1,0,0
shapr --input model.obj --output model.stl --rotate 45,0,1,0    # angle,x,y,z
shapr --input model.obj --output model.stl --scale 2.0          # uniform
shapr --input model.obj --output model.stl --scale 1,2,1        # per-axis

# Analysis (output file is optional)
shapr --input model.obj --surface-area
shapr --input model.obj --volume
shapr --input model.obj --inside 0.5,0.5,0.5

# All together
shapr --input model.obj --output model.stl --translate 0,1,0 --surface-area --volume
```

## Architecture

### Converter

The core of the design is an `IMeshReader` / `IMeshWriter` interface pair. Adding support for a new format requires only implementing one of these interfaces and registering it with `MeshConverter` — no existing code needs to change.
Writers and readers are registered to the MeshConverter and then used based on read-file type and output-file type.

```
IMeshReader ──> MeshConverter ──> IMeshWriter
```

`MeshConverter` maintains two `std::unordered_map` registries keyed by format name. Format names are inferred from file extensions at the CLI level, so the converter itself has no knowledge of the filesystem.

### Internal Representation

`Mesh` is the internal representation (IR) passed between reader, converter, and writer:

```cpp
struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<std::array<uint32_t, 3>> triangles;
    std::optional<std::vector<glm::vec3>> normals;
    std::optional<std::vector<glm::vec2>> texcoords;
};
```

`uint32_t` indices match the binary STL format directly, avoiding any conversion. Normals and texcoords are `std::optional` to distinguish between "this mesh has no normals" and "this mesh has an empty normal list".

### Two `Convert` Overloads

`MeshConverter` exposes two overloads: one that reads and transforms only, and one that also writes. This allows callers to perform analysis on the transformed mesh before (or instead of) writing — which is exactly what the CLI does when `--surface-area`, `--volume`, or `--inside` are used without `--output`.

### Transform

`Transform` uses a GLM `mat4` builder pattern initialized to the identity matrix. Calls to `Translate`, `Rotate`, and `Scale` accumulate into a single matrix, which is applied to vertex positions in homogeneous coordinates (`w=1`). Normals are transformed with the inverse-transpose of the model matrix — this is necessary because normals are not points; they are directions perpendicular to surfaces, and non-uniform scaling would otherwise distort them.

### `MeshAnalyzer` as a Namespace

`MeshAnalyzer` is a free-function namespace rather than a static class. A static class would communicate the wrong intent and add unnecessary boilerplate.

## Algorithms

### Point Containment (`IsInside`)

Uses ray casting: fire a ray from the query point and count how many times it intersects the mesh surface. An odd count means inside; even means outside.

The naive approach is fragile — a ray that passes exactly through a shared edge between two triangles gets counted twice, producing a false "outside" result. Two mitigations are applied:

1. **Möller–Trumbore intersection** with an epsilon guard on the `t` parameter, rejecting intersections behind the ray origin.
2. **Majority vote across three rays** with asymmetric direction perturbations `{1, 1e-4, 2e-4}`, `{3e-4, 1, 1e-4}`, `{2e-4, 3e-4, 1}`. The directions are intentionally not symmetric (e.g. not `{1, 1e-4, 1e-4}`) to avoid hitting shared diagonals in axis-aligned meshes. A point is considered inside if at least 2 of 3 rays agree.

### Surface Area

Sum of `|e1 × e2| / 2` over all triangles, where `e1` and `e2` are edge vectors from vertex 0. The magnitude of the cross product gives the area of the parallelogram formed by the two edges; halving gives the triangle area.

### Volume

Uses the signed tetrahedron method (divergence theorem): for each triangle, compute `dot(v0, cross(v1, v2)) / 6`. Summing over a closed, watertight mesh and taking the absolute value gives the enclosed volume. This requires no special origin placement — the signed contributions cancel correctly regardless of where the origin is relative to the mesh.

### OBJ → Binary STL

OBJ faces are arbitrary convex n-gons. Fan triangulation is used: fix vertex 0 and emit triangles `(v0, v[i], v[i+1])` for `i = 1..n-2`. This is correct for convex faces.

Vertices are deduplicated via an `std::unordered_map` keyed on `(vertex_index, normal_index, texcoord_index)`. The hash uses the boost::hash_combine technique to avoid collisions between permutations that naive XOR would treat as identical.

Binary STL is written as: 80-byte zero header, `uint32` triangle count, then 50 bytes per triangle (12-byte normal, three 12-byte vertices, 2-byte attribute count). All values are written in little-endian byte order with compile-time endian detection via `std::endian`.

Per-face normals are recomputed from the cross product of the triangle edges rather than using mesh normals, matching the STL convention of one normal per face.

## Testing

17 tests across 5 suites using Catch2:

`ArgParserTests` - Flag parsing, key-value args, edge cases

`MeshAnalyzerTests` - Surface area, volume, inside/outside on a unit cube

`ObjReaderTests` - Valid OBJ load, malformed input exception

`StlWriterTests` - Output size, header, triangle count

`MeshConverterTests` - Full conversion, unknown reader/writer exceptions

