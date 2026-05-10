#version 460 core

in vec3 v_Colour;
in vec2 v_TexCoord;

out vec4 o_FragColour;

uniform sampler2D u_Texture;
uniform bool      u_UseTexture = false;

void main()
{
    if (u_UseTexture) {
        o_FragColour = texture(u_Texture, v_TexCoord);
    } else {
        o_FragColour = vec4(v_Colour, 1.0);
    }
}
