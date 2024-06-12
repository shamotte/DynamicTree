#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

out vec4 pixelColor; 

in vec4 l1;
in vec4 l2;
in vec4 v;
in vec2 iTreeTexture;

void main(void) {
    //Znormalizowane interpolowane wektory
    vec4 ml1 = normalize(l1);
    vec4 ml2 = normalize(l2);
    vec4 mn = normalize(vec4(texture(textureMap1, iTreeTexture).rgb * 2 - 1, 0));
    vec4 mv = normalize(v);
    vec4 mr1 = reflect(-ml1, mn); //Wektor odbity dla światła 1
    vec4 mr2 = reflect(-ml2, mn); //Wektor odbity dla światła 2

    //Parametr powierzchni
    vec4 kd = texture(textureMap0, iTreeTexture);
    vec4 ks = texture(textureMap0, iTreeTexture); //vec4(1,1,1,1);

    //Model Phonga dla pierwszego światła
    float nl1 = clamp(dot(mn, ml1), 0, 1); //Kosinus kąta pomiędzy wektorami n i l1.
    float rv1 = pow(clamp(dot(mr1, mv), 0, 1), 25); // Kosinus kąta pomiędzy wektorami r1 i v podniesiony do 25 potęgi

    //Model Phonga dla drugiego światła
    float nl2 = clamp(dot(mn, ml2), 0, 1); //Kosinus kąta pomiędzy wektorami n i l2.
    float rv2 = pow(clamp(dot(mr2, mv), 0, 1), 25); // Kosinus kąta pomiędzy wektorami r2 i v podniesiony do 25 potęgi

   
    vec4 color1 = vec4(kd.rgb * nl1, kd.a) + vec4(ks.rgb * rv1, 0);
    vec4 color2 = vec4(kd.rgb * nl2, kd.a) + vec4(ks.rgb * rv2, 0);

    pixelColor = color1 + color2;
}