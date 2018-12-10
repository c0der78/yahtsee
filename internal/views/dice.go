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
	currentRoll yahtsee.Roll
	selectedDice yahtsee.Roll
}

var diceTextures = [6]graphics.TextureTypes {
	graphics.Die1,
	graphics.Die2,
	graphics.Die3,
	graphics.Die4,
	graphics.Die5,
	graphics.Die6,
}

var selectedTextures = [6]graphics.TextureTypes {
	graphics.Die1,
	graphics.Die2,
	graphics.Die3,
	graphics.Die4,
	graphics.Die5,
	graphics.Die6,
}

func NewDiceView() *DiceView {
	return &DiceView{
		currentRoll: yahtsee.Shake(),
		selectedDice: yahtsee.Roll{},
	}
}

func (view *DiceView) showDie(pos int) {

	var texture *graphics.Texture

	die := view.selectedDice[pos]

	if die == 0 {

		die = view.currentRoll[pos]

		texture = graphics.Textures.Get(diceTextures[die-1])

	} else {
		texture = graphics.Textures.Get(selectedTextures[die-1])
	}

	size := imgui.Vec2{X: float32(48), Y: float32(48)}

	imgui.Image(texture.Id, size)

	//if imgui.ImageButton(texture.Id, size) {
	//	view.selectedDice[pos] = die
	//}
}

func (view *DiceView) Render() {
	if imgui.BeginChildV(diceViewID, diceViewSize, true, 0) {

		imgui.SameLineV(12, 0)

		imgui.BeginGroup()

		imgui.ColumnsV(3, "DiceTextures", false)

		for i := 0; i < len(view.currentRoll); i++ {
			view.showDie(i)

			imgui.NextColumn()
			imgui.NextColumn()
		}

		imgui.EndGroup()
	}
	imgui.EndChild()

	if imgui.BeginChildV("DiceActions", imgui.Vec2{X: 200, Y: 20 }, false, 0) {

		imgui.SameLineV(50, 0)

		if imgui.ButtonV("Roll", imgui.Vec2{X: 100, Y: 20}) {
			go func() {
				for i := 0; i < 15; i++ {
					view.currentRoll = yahtsee.Shake()
					time.Sleep(time.Millisecond * 100)
				}
			}()
		}
	}
	imgui.EndChild()
}

func (view *DiceView) Update() {

}
