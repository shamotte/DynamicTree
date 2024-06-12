#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
layout (location=5) in vec2 treeTexture; 
layout (location=4) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=3) in vec4 c3;
layout (location=2) in vec4 c2;
layout (location=1) in vec4 c1;

//Zmienne interpolowane
out vec4 l1;
out vec4 l2;
out vec4 v;
out vec2 iTreeTexture;

void main(void) {
    mat4 invTBN = mat4(c1, c2, c3, vec4(0, 0, 0, 1));

    vec4 lp1 = vec4(2, 3, -2, 1); //pozcyja światła 1, przestrzeń świata
    vec4 lp2 = vec4(-1, 3, 2, 1);//pozcyja światła 2, przestrzeń świata

    l1 = normalize(invTBN * inverse(M) * lp1 - invTBN * vertex); //znormalizowany wektor do światła 1 w przestrzeni stycznej
    l2 = normalize(invTBN * inverse(M) * lp2 - invTBN * vertex); //znormalizowany wektor do światła 2 w przestrzeni stycznej
    v = normalize(invTBN * inverse(V * M) * vec4(0, 0, 0, 1) - invTBN * vertex); //Wektor do obserwatora w przestrzeni stycznej
    iTreeTexture = treeTexture;

    gl_Position = P * V * M * vertex;
}