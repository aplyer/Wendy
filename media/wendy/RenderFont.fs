
uniform sampler2D glyphs;
uniform vec4 color;

varying vec2 mapping;

void main()
{
  gl_FragColor = vec4(color.rgb, color.a * texture2D(glyphs, mapping).r);
}

