#version 330


out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

uniform vec4 color=vec4(88/256.0,59/256.0,39/256.0,1);

void main(void) {
	pixelColor=color;
}
