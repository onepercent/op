#ifdef GL_ES															
precision lowp float;													
#endif																	
																		
uniform vec4 u_color;
																		
void main()																
{																		
	gl_FragColor = u_color;
}																		
