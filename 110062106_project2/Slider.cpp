#include <algorithm>
#include <string>

#include "Point.hpp"
#include "Slider.hpp"

Slider::Slider(float x, float y, float w, float h) :
	ImageButton("stage-select/slider.png", "stage-select/slider-blue.png", x, y),
	Bar("stage-select/bar.png", x, y, w, h),
	End1("stage-select/end.png", x, y + h / 2, 0, 0, 0.5, 0.5),
	End2("stage-select/end.png", x + w, y + h / 2, 0, 0, 0.5, 0.5) {
	Position.x += w;
	Position.y += h / 2;
	Anchor = Engine::Point(0.5, 0.5);
}
void Slider::Draw() const {
	Bar.Draw();
	End1.Draw();
	End2.Draw();
    ImageButton::Draw();
}
void Slider::SetOnValueChangedCallback(std::function<void(float value)> onValueChangedCallback) {
	OnValueChangedCallback = onValueChangedCallback;

}
void Slider::SetValue(float value) {
	this->value = value;
	Position.x = this->value * (End2.Position.x - End1.Position.x) + End1.Position.x ;
	if (OnValueChangedCallback)
				OnValueChangedCallback(this->value);
}
void Slider::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && mouseIn && Enabled){
        Down = true;
	}

}
void Slider::OnMouseUp(int button, int mx, int my) {
	Down = false;
}
void Slider::OnMouseMove(int mx, int my) {
    ImageButton::OnMouseMove(mx,my);
    if(Down){
        if(mx <= End2.Position.x  and mx >= End1.Position.x){
          SetValue( (mx-End1.Position.x) / (End2.Position.x - End1.Position.x) );
        }
    }
}
