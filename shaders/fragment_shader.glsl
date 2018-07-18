#version 430
in highp vec2 texCoord;
out highp vec4 fragColor;
layout(binding = 0) uniform highp sampler2D outTex0;
layout(binding = 1) uniform highp sampler2D outTex1;
uniform vec2 cursorPos;
uniform vec2 rectExt; // also used as vec2(1.0f) / vec2(width,height) when using rm1
uniform vec2 tex0_size;
uniform int mode; // mode contains an integer dependent on the _renderingMethod
// check wheather a given point is in the rectangle
// rectPos is bottom left point of rect
bool checkPointInRectangle(vec2 rectPos, vec2 rectExtends, vec2 point) {
	if (point.x < rectPos.x || point.y < rectPos.y) return false;    // left and bottom edge
	if (point.x > rectPos.x + rectExtends.x || point.y > rectPos.y + rectExtends.y) return false;    // top and right edge
	return true;
}
bool rhgv(vec4 leftColor, vec4 rightColor) {
	if (leftColor.x < rightColor.x || leftColor.y < rightColor.y || leftColor.z < rightColor.z) return true;
	return false;
}

void renderingMethod0(){ // Standard
	fragColor = texture(outTex0, texCoord);
}

void renderingMethod1(){ // DISTANCE_DC
	vec4 cur = texture(outTex0, texCoord);
	vec2 windows_coord = tex0_size * texCoord;
    
	int x = int(windows_coord.x);
	int y = int(windows_coord.y);

	vec4 red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vec4 blue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	if(x % 2== 1 || y % 2 == 1){ // blacked out, take another texel
        // determine which texel to take
		if(x % 2== 1 && y % 2 == 1){
			// take top left
			//fragColor = texture(outTex0, texCoord - rectExt);
			fragColor = red;
		}else{
			if(x % 2== 0 && y % 2 == 1){
				// top
				//fragColor = texture(outTex0, texCoord - vec2(0, rectExt.y));
				fragColor = green;
			}else{
				if(x % 2== 1 && y % 2 == 0){
					// left
					//fragColor = texture(outTex0, texCoord - vec2(rectExt.x, 0));
					fragColor = blue;
				}
			}
		}
    }else{
		fragColor = texture(outTex0, texCoord);
		// same texel
	}
	return;
}

void renderingMethod2(){ // SQUARE_DC
	vec4 fragColor0 = texture(outTex0, texCoord);
	vec4 fragColor1 = texture(outTex1, texCoord);
	if(checkPointInRectangle(cursorPos - 0.5 * rectExt, rectExt, texCoord)){
		//if(rhgv(vec4(0,0,0,0), fragColor1)){
			fragColor = fragColor1;
	}else{
			fragColor = fragColor0;
	}
	return;
}

void renderingMethod3(){ // SINUS_RESOLUTION
	fragColor = texture(outTex0, texCoord);
}

void main() {
	switch(mode){
		case 0:
			renderingMethod0();
			break;
		case 1:
			renderingMethod1();
			break;
		case 2:
			renderingMethod2();
			break;
		case 3:
			renderingMethod3();
			break;
		default:
			renderingMethod0();
			break;
	}
}