uniform mat4 matrix2D;
attribute vec2 vert2D;
attribute float vertexColor;
varying vec4 color;

void main()
{
    gl_Position = matrix2D * vec4(vert2D, 0.0f, 1.0f);
    color = vec4(vertexColor, 0, 1.0f - vertexColor, 1.0f);
}
