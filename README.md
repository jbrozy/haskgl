# Haskell-Like OpenGL

As obvious as it sounds, "haskgl" is a mix of common haskell style combined with openGL.
I wanted to make a language that transforms into either OpenGL or Vulkan.

As for common structs that exist in OpenGL:

## data types
``` haskell
data vec2 :: {
    x -> [u] :: float,
    y -> [v] :: float,
}
```
The characters inside ```x -> [r, a]``` are supposed to make everything a bit more verbose. Furthermore, vec4/vec3/vec2 already exist in OpenGL, therefore there is no need to define them again.
However, this is for the sake of completeness.

## includes (Pythonian import style)
```haskell
@include (vec3, vec4, mat4) -> types
```
## in-variables in vertex shader
```haskell
in vertex :: {
  position :: vec3,
  normal   :: vec3,
  uniforms -> {
      modelMatrix      :: mat4,
      viewMatrix       :: mat4,
      projectionMatrix :: mat4,
      lightPos         :: vec3,
      cameraPos        :: vec3
  }
}
```
## out-variables in vertex shader
```haskell
out vertex :: {
  vPosition :: vec3,
  vNormal   :: vec3,
}
```

Further instructions

```haskell
@internal data vec3 :: { ... } // Supposed to not transpile this into a struct, since it already exists in either OpenGL or Vulkan.
@main // Define the main function -> Entry Point
```
