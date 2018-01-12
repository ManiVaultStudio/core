/*!	@header		DensitySplat.glsl
	@discussion	Vertex shader program for basic drawing without lighting etc.
	@author		Thomas Hoellt
	@created	2013-07-26
	@updated	2018-01-10 by Julian Thijssen */

#version 330 core

uniform mat4 modelViewProjectionMatrix;	///< incoming value for the mvp matrix

uniform isampler1D activeSampleSampler;	///< the texture containing flags for the active state of each sample 

uniform vec4 advancedParams;	///< parameters for additional scaling <is scale active, ?, ?, ?>

layout(location = 0) in vec4 position;			///< incoming value for the vertex position
layout(location = 1) in int sampleIdBuffer;		///< incoming value for the sample id the point originates from
layout(location = 2) in float scaleBuffer;		///< incoming value for the current point scale

//out vec4 vertexPosition;	///< outgoing value for the vertex color
flat out float densityScale;		///< outgoing value indicating the quad position and size
flat out int activeState;	///< outgoing value for the active state

/*!	@method		main
	@discussion	The main routine: sets the vertex color to the given color and
				transforms the given vertex position into view space using the
				Model View Projection matrix.*/
void main (void)
{
	activeState = texelFetch(activeSampleSampler, sampleIdBuffer, 0).r;
	activeState = activeState * ((advancedParams.x > -0.5 && scaleBuffer < 0.0) ? 0 : 1);
	
	densityScale = (advancedParams.x > -0.5) ? max(0.0, scaleBuffer) : 1.0;
//	vertexPosition = position.xyz;
	gl_Position = modelViewProjectionMatrix * position;
}
