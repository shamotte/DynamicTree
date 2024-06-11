#version 330


//uniform sampler2D textureMap1;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 l;
in vec4 v;
in vec2 iTreeTexture;

void main(void) {

    //Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 mn = normalize(vec4(texture(textureMap1, iTreeTexture).rgb*2-1,0));         
	vec4 mv = normalize(v);
	vec4 mr=reflect(-ml,mn); //Wektor odbity

	//Parametr powierzchni
	//vec4 kd = texture(textureMap0, iTexCoord0);
	//vec4 ks = vec4(1,1,1,1);   //texture(textureMap1, iTexCoord0);

	//Model Phonga
	float nl = clamp(dot(mn, ml), 0, 1); //Kosinus k�ta pomi�dzy wektorami n i l.
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25); // Kosinus k�ta pomi�dzy wektorami r i v podniesiony do 25 pot�gi
    pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);
}
