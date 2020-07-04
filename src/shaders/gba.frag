out vec4 FragColor;
varying vec4 v_Color;
varying vec2 v_Texcoord;
uniform sampler2D gm_BaseTexture;

const int count = 4;
uniform vec4 u_col[count];

void main() {
	vec4 col = texture2D(gm_BaseTexture, v_Texcoord);

	vec4 colors[count];
  float dist[count];
  for (int i = 0; i < count; i++) {
    colors[i] = u_col[i]/255.0;

    float dist_r =  (colors[i].r - col.r) * (colors[i].r - col.r);
    float dist_g =  (colors[i].g - col.g) * (colors[i].g - col.g);
    float dist_b =  (colors[i].b - col.b) * (colors[i].b - col.b);

    dist[i] = dist_r + dist_g + dist_b;
  }

  float d_min = min(dist[0], dist[1]);
  for (int i = 2; i < count; i++) {
    d_min = min(d_min, dist[i]);
  }

  for (int i = 0; i < count; i++) {
    colors[i].a = 1;
  }

  for (int i = 0; i < count; i++) {
    if (d_min == dist[i]) FragColor = colors[i];
  }

  // FragColor = effect(v_Color, gm_BaseTexture, v_Texcoord);
  //FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
}