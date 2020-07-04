out vec4 FragColor;
varying vec4 v_Color;
varying vec2 v_Texcoord;
uniform sampler2D gm_BaseTexture;

uniform vec2 u_size;
uniform vec4 u_col;

void main() {
  vec2 uv = v_Texcoord;
  float offsetx = 1.0/u_size.x;
  float offsety = 1.0/u_size.y;

  vec4 color = u_col/255.0;

	vec4 col = texture2D(gm_BaseTexture, v_Texcoord);

	float a = texture2D(gm_BaseTexture, vec2(uv.x + offsetx, uv.y)).a +
            texture2D(gm_BaseTexture, vec2(uv.x - offsetx, uv.y)).a +
            texture2D(gm_BaseTexture, vec2(uv.x, uv.y + offsety)).a +
            texture2D(gm_BaseTexture, vec2(uv.x, uv.y - offsety)).a;

  // if (col.a < 1.0 && a > 0.0) col = color;
  col.a = a;

  FragColor = v_Color * col;
}