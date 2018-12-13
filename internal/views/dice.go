package views

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/yahtsee"
	"time"
)

const (
	diceViewID     = "Shaker"
	diceRollID     = "DiceRoll"
	diceTexturesID = "DiceTextures"
	alertMaxRoll   = "No More Rolls"
)

const maxRollPerTurn = 3

var diceViewSize = imgui.Vec2{X: 200, Y: 250}

var diceViewPos = imgui.Vec2{X: 700, Y: 140}

type DiceView struct {
	currentRoll [yahtsee.DiePerRoll]*Die
	rollCount   int
}

var diceTextures = [6]graphics.TextureTypes{
	graphics.Die1,
	graphics.Die2,
	graphics.Die3,
	graphics.Die4,
	graphics.Die5,
	graphics.Die6,
}

type Die struct {
	texture  graphics.TextureTypes
	value    int
	selected bool
}

func NewDiceView() *DiceView {

	view := &DiceView{
		currentRoll: [yahtsee.DiePerRoll]*Die{},
		rollCount:   1,
	}

	view.Shake()

	return view
}

func (view *DiceView) Shake() {

	// update views a shake of the dice
	for i, roll := range yahtsee.Shake() {

		// skip selected dice
		if view.currentRoll[i] != nil && view.currentRoll[i].selected {
			continue
		}

		view.currentRoll[i] = &Die{
			value:   roll,
			texture: diceTextures[roll-1],
		}
	}
}

func (view *DiceView) Render() {

	imgui.SetNextWindowPos(diceViewPos)
	imgui.SetNextWindowSize(diceViewSize)

	if imgui.Begin(diceViewID) {

		imgui.SetNextWindowPos(imgui.Vec2{X: 717, Y: 170})

		if imgui.BeginChildV("View", imgui.Vec2{X: 170, Y: 170}, false, 0) {

			imgui.ColumnsV(3, diceTexturesID, false)

			for i, die := range view.currentRoll {

				size := imgui.Vec2{X: float32(48), Y: float32(48)}

				texture := graphics.Textures.Get(die.texture)

				imgui.PushID(string(i))

				if imgui.ImageButtonV(texture.Id, size, imgui.Vec2{X: 0, Y: 0}, imgui.Vec2{X: 1, Y: 1},
					0, imgui.Vec4{X: 0, Y: 0, Z: 0, W: 0}, imgui.Vec4{X: 1, Y: 1, Z: 1, W: 1}) {
					die.selected = true
				}

				imgui.PopID()

				imgui.NextColumn()
				imgui.NextColumn()
			}
		}
		imgui.EndChild()

		if imgui.BeginChild("Actions") {

			imgui.SameLineV(45, 0)

			if imgui.ButtonV("Roll", imgui.Vec2{X: 100, Y: 30}) {
				if view.rollCount >= maxRollPerTurn {
					imgui.OpenPopup(alertMaxRoll)
				} else {
					view.rollCount++

					go func() {
						for i := 0; i < 15; i++ {
							view.Shake()
							time.Sleep(time.Millisecond * 100)
						}
					}()
				}
			}

			if imgui.BeginPopupModalV(alertMaxRoll, nil,
				imgui.WindowFlagsNoMove|imgui.WindowFlagsNoResize|imgui.WindowFlagsNoCollapse) {

				imgui.Spacing()
				imgui.Spacing()

				imgui.Text("A turn is allowed only 3 rolls.")

				imgui.Spacing()
				imgui.Spacing()
				imgui.Spacing()

				imgui.SameLineV(50, 0)

				if imgui.ButtonV("OK", imgui.Vec2{X: 100, Y: 30}) {
					imgui.CloseCurrentPopup()
				}

				imgui.EndPopup()
			}
		}

		imgui.EndChild()
	}
	imgui.End()

}

func (view *DiceView) Update() {

}
