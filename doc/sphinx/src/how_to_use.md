# How to Use

This library can be used via [vcpkg](https://vcpkg.io/) using following configurations:

- Add a vcpkg registry
  `https://gitlab.com/MusicScience37Projects/vcpkg-registry`
  in `vcpkg-configuration.json`.

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg-configuration.schema.json",
    "default-registry": {
      "kind": "git",
      "repository": "https://github.com/Microsoft/vcpkg",
      "baseline": "78b61582c9e093fda56a01ebb654be15a0033897"
    },
    "registries": [
      {
        "kind": "git",
        "repository": "https://gitlab.com/MusicScience37Projects/vcpkg-registry",
        "baseline": "f7fcc04f34f351a91796eaa0b386017e961a51b4",
        "packages": ["cpp-hash-tables", "numerical-collection-cpp"]
      }
    ]
  }
  ```

- Add `numerical-collection-cpp` in `vcpkg.json`

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
    "dependencies": ["numerical-collection-cpp"]
  }
  ```
