
//
// Function declarations
//

vec4 pixelMain( in vec4 UV );

//
// Function definitions
//

vec4 pixelMain( in vec4 UV ) {

    return UV;
}


//
// User varying
//
varying vec4 xlat_varying_COLOR0;

//
// Translator's entry point
//
void main() {
    vec4 xlat_retVal;

    xlat_retVal = pixelMain( vec4(xlat_varying_COLOR0));

    gl_FragData[0] = vec4( xlat_retVal);
}


 