/*!	@header		glslProgramSimplePointsDensity_fragment.glsl
	@discussion	Fragment shader program for basic drawing without lighting etc.
	@author		Thomas Hoellt
	@updated	2013-09-21 */

#version 330 core

//uniform sampler1D colormapSampler;	///< the colormap texture, used to convert the raw value to color
uniform sampler2D gaussSampler;			///< the gauss blob texture, used to splat the points to kdes

in	vec3 texCoords;						///< incoming value for the lookup in the gauss plot texture
out vec4 fragmentColor;					///< outgoing value for the fragment color

/*!	@method		main
	@discussion	The main routine: does nothing but pass on the interpolated vertex color.*/
void main(void)
{
	float density = texture(gaussSampler, texCoords.xy).r;

	fragmentColor = vec4(density* texCoords.z);
}
