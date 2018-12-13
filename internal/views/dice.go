package views

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/yahtsee"
	"time"
)

const diceViewID = "DiceView"

var diceViewSize = imgui.Vec2{X: 200, Y: 170}

type DiceView struct {
	currentRoll [yahtsee.DiePerRoll]*Die
	rollCount int
}

var diceTextures = [6]graphics.TextureTypes {
	graphics.Die1,
	graphics.Die2,
	graphics.Die3,
	graphics.Die4,
	graphics.Die5,
	graphics.Die6,
}

type Die struct {
	texture graphics.TextureTypes
	value int
	selected bool
}

func NewDiceView() *DiceView {

	view := &DiceView{
		currentRoll: [yahtsee.DiePerRoll]*Die{},
		rollCount: 0,
	}

	view.Shake()

	return view
}

func (view *DiceView) Shake() {
	for i, roll := range yahtsee.Shake() {
		view.currentRoll[i] = &Die{
			value: roll,
			texture: diceTextures[roll-1],
		}
	}
}

func (view *DiceView) Render() {
	if imgui.BeginChildV(diceViewID, diceViewSize, true, 0) {

		imgui.ColumnsV(3, "DiceTextures", false)

		for _, die := range view.currentRoll {

			size := imgui.Vec2{X: float32(48), Y: float32(48)}

			texture := graphics.Textures.Get(die.texture)

			if imgui.ImageButtonV(texture.Id, size, imgui.Vec2{X: 0, Y: 0}, imgui.Vec2{X: 1, Y: 1},
				0, imgui.Vec4{X: 0, Y: 0, Z: 0, W: 0}, imgui.Vec4{X: 1, Y: 1, Z: 1, W: 1}) {
				die.selected = true
			}

			imgui.NextColumn()
			imgui.NextColumn()
		}
	}
	imgui.EndChild()

	if imgui.BeginChildV("DiceActions", imgui.Vec2{X: 200, Y: 20 }, false, 0) {

		imgui.SameLineV(50, 0)

		if imgui.ButtonV("Roll", imgui.Vec2{X: 100, Y: 20}) {
			if view.rollCount < 3 {

				view.rollCount++

				go func() {
					for i := 0; i < 15; i++ {
						view.Shake()
						time.Sleep(time.Millisecond * 100)
					}
				}()
			}
		}
		if view.rollCount >= 3 {

			if imgui.BeginPopupModal("Alert") {

				imgui.Text("A turn is allowed only 3 rolls to score")

				imgui.EndPopup()
			}
		}
	}
	imgui.EndChild()
}

func (view *DiceView) Update() {

}
