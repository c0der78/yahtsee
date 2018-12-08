package views

import (
	"fmt"
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

		imgui.PushFont(graphics.Fonts.Get(graphics.FontAwesomeLarge))

		imgui.Text(fmt.Sprintf("Ace %c", graphics.FontAwesomeDie1))
		imgui.Text(string(graphics.FontAwesomeDie2))
		imgui.Text(string(graphics.FontAwesomeDie3))
		imgui.Text(string(graphics.FontAwesomeDie4))
		imgui.Text(string(graphics.FontAwesomeDie5))
		imgui.Text(string(graphics.FontAwesomeDie6))

		imgui.PopFont()

		//texture := graphics.Textures.Get(graphics.Die1)
		//
		//size := imgui.Vec2{X: float32(texture.Width), Y: float32(texture.Height)}
		//
		//imgui.Image(texture.Id, size)
	}
	imgui.EndChild()
}

func (view *DiceView) Update() {

}
