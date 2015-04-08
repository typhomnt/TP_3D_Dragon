# version 120


// Infos sur le matériel
struct material_info {
    vec4 ka;
    vec4 kd;
    vec4 ks;
    vec4 emission;
    float shininess;
};

// Infos sur la lumière
struct light_info {
    vec4 position;
    vec4 la      ;
    vec4 ld      ;
    vec4 ls      ;
};


// Matériel de l'objet passé via OpenGL
uniform material_info material;

// Source de lumière passée via OpenGL
uniform light_info light;

// Couleur de la lumière qui sera transmise au fragment shader
varying vec4 light_color;

// Coordonnées de la texture passées via OpenGL
attribute vec2 texcoord0;

// Coordonnées de la texture passées au fragment shader
varying vec2 frag_texcoord0;


// Illumination de Phong
vec4 phongWithMaterial( vec4 position, vec3 norm )
{
    vec3 L = normalize( light.position.xyz - position.xyz );
    vec3 V = normalize( -position.xyz ); //camera is at (0,0,0) in this referential
    vec3 R = normalize( -reflect( L, norm ) );

    vec4 i_emission = material.emission; 
    vec4 i_ambient  = light.la * material.ka;
    vec4 i_diffuse  = light.ld * material.kd * max( dot( norm, L ), 0.0 );
    vec4 i_specular = light.ls * material.ks * pow( max( dot(R, V ), 0.0 ), material.shininess );

    return i_emission + i_ambient + clamp( i_diffuse, 0, 1 ) + clamp( i_specular, 0, 1 );
}


void main( void )
{
    vec4 position = gl_ModelViewMatrix * gl_Vertex; 
    vec3 normal   = mat3(gl_ModelViewMatrixInverseTranspose) * gl_Normal;

    light_color = phongWithMaterial( position, normal );
    frag_texcoord0 = texcoord0;

    gl_Position = gl_ProjectionMatrix * position;
}