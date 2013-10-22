#ifdef GL_ES															
precision lowp float;													
#endif																	
																		
varying vec2 v_texCoord;
uniform sampler2D u_texture;

void main()																
{
    float a = texture2D(u_texture, v_texCoord).a;
	gl_FragColor = vec4(0,0,0,a*a);
	//gl_FragColor = vec4(0,0,0,a);
}
