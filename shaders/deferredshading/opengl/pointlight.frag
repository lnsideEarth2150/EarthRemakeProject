#version 330 compatibility

uniform sampler2D DiffuseTex;
uniform sampler2D NormalTex;
uniform sampler2D PositionTex;

//uniform vec3 CameraPos;

uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform float strength;
uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadAttenuation;

vec2 invResolution = vec2(1.0 / 1024.0, 1.0 / 768.0);

void main (void) {
	gl_FragColor = vec4(LightColor, 1);
	//return;
	
	vec2 texCoord = (gl_FragCoord.xy + vec2(0.5, 0.5)) * invResolution;

	vec3 position = texture2D(PositionTex, texCoord).rgb;
	// Farbe des Untergrundes auf welchen das Licht leutet
	vec4 groundColor = vec4(texture2D(DiffuseTex, texCoord).rgb, 1);
	vec3 normal = texture2D(NormalTex, texCoord).xyz;
	
	float distance = length(position - LightPosition);
	
	vec3 lightDirection = normalize(LightPosition - position);
	
	float diffuseFactor = dot(normal, lightDirection);
	
	if (diffuseFactor < 0)
		discard;
	
	//float lightStrength = strength * 5 / (distance * distance + 1);
	
	//float lightStrength = 1.f - (distance / strength);
	//float lightStrength = 1.f - (distance / strength);
	//float lightStrength = max(0, cos(distance * 1.570796f / strength));
	//float lightStrength = 1.f - ((distance * distance) / (strength * strength));
	float lightStrength = max(0, 1.f - (distance / strength));
	
	//float lightStrength = max(0, cos(distance * 1.570796f / (strength * 0.75)));
	
	if (lightStrength <= 0.00) {
		//gl_FragColor = vec4(1,0,0,1);
		//return;
	}
		
	
	/*float lightStrength = strength / (constantAttenuation 
		+ distance * linearAttenuation
		+ distance * distance * quadAttenuation);*/
	
	groundColor.rgb *= LightColor.rgb * lightStrength * lightStrength;
	groundColor.rgb *= diffuseFactor;
	//groundColor.a = 1;
	//groundColor.a = lightStrength * diffuseFactor;
	//groundColor.rgb = LightColor;
	//groundColor = vec4(position, 1);
	
	gl_FragColor = groundColor;
	//gl_FragColor.rgb = 0;
	//gl_FragColor.r = lightStrength * diffuseFactor;
}
