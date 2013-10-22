#ifdef GL_ES															
precision lowp float;													
#endif																	
																		
varying vec2 v_texCoord;
varying vec4 v_color;

uniform sampler2D u_texture;

void main()																
{																		
	gl_FragColor =  vec4(v_color.rgb, v_color.a * texture2D(u_texture, v_texCoord).a);
}
