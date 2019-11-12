#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3

// Point properties
uniform float pointSize;
uniform int   scalarEffect;

/** Projection matrix from bounds space to clip space */
uniform mat3 orthoM;
/** Whether a highlight buffer is used */
uniform bool hasHighlights;
/** Whether a scalar buffer is used */
uniform bool hasScalars;
/** Range of scalar buffer if defined */
uniform vec3 scalarRange;
/** Whether a color buffer is used */
uniform bool hasColors;

// Input attributes
// vertex    - Vertex input, always a [-1, 1] quad
// position  - 2-Dimensional positions of points
// highlight - Mask of highlights over the points
// scalar    - Auxiliary scalar data, useful for visualization properties
// color     - Color associated with point
layout(location = 0) in vec2  vertex;
layout(location = 1) in vec2  position;
layout(location = 2) in int   highlight;
layout(location = 3) in float scalar;
layout(location = 4) in vec3  color;

// Output variables
smooth out vec2  vTexCoord;
flat   out int   vHighlight;
smooth out float vScalar;
smooth out vec3  vColor;

void main()
{
    // The texture coordinates match vertex coordinates
    vTexCoord = vertex;
    // Pass input attributes to fragment shader if they are defined
    vHighlight = hasHighlights ? highlight : 0;
    vScalar = hasScalars ? (scalar - scalarRange.x) / scalarRange.z : 1;
    vColor = hasColors ? color : vec3(0.5);
    
    // Point properties
    float scale = 1.0;

    // Scale up the point if highlighted
    if (hasHighlights && highlight == 1)
        scale *= 1.2;

    // If the scalar values affect size, scale the point appropriately
    if (scalarEffect == EFFECT_SIZE) {
        scale *= scalar;
    }
    
    // Transform position to clip space
    vec2 pos = (orthoM * vec3(position, 1)).xy;
    
    // Resize point quad according to properties
    vec2 scaledVertex = vertex * pointSize * scale;
    
    // Move quad by position and output
    gl_Position = vec4(scaledVertex + pos, 0, 1);
}
