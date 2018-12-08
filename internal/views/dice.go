package views

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
)

const DiceViewID = "DiceView"

type DiceView struct {}

func NewDiceView() *DiceView {
	return &DiceView{}
}

func (view *DiceView) Render() {

	if imgui.BeginChild(DiceViewID) {

		texture := graphics.Textures.Get(graphics.Die1)

		size := imgui.Vec2{X: float32(texture.Width), Y: float32(texture.Height)}

		imgui.Image(texture.Id, size)
	}
	imgui.EndChild()
}

func (view *DiceView) Update() {

}
