package views

import (
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/events"
	"micrantha.com/yahtsee/internal/graphics"
	"micrantha.com/yahtsee/internal/states"
	"micrantha.com/yahtsee/internal/yahtsee"
	"time"
)

const (
	diceViewID     = "Shaker"
	diceTexturesID = "DiceTextures"
	alertMaxRoll   = "No More Rolls"
)

const maxRollPerTurn = 3

var diceViewSize = imgui.Vec2{X: 200, Y: 210}

var diceViewPos = imgui.Vec2{X: 700, Y: 140}

var dieSize = imgui.Vec2{X:38, Y:38}

type DiceView struct {
	state *states.State
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

func NewDiceView(state *states.State) *DiceView {

	view := &DiceView{
		currentRoll: [yahtsee.DiePerRoll]*Die{},
		rollCount:   1,
		state: state,
	}

	view.Shake()

	channel := events.NewChannel(func(event events.Event) {
		view.rollCount = 1
		view.ShakeAnimation()
	})

	channel.StartListening()

	events.Bus().AddListener(events.ScoreEventID, channel)

	return view
}

func (view *DiceView) Shake() {

	view.state.CurrentRoll = yahtsee.Shake()

	// update views a shake of the dice
	for i, roll := range view.state.CurrentRoll {

		// skip selected dice
		if view.currentRoll[i] != nil && view.currentRoll[i].selected {
			view.state.CurrentRoll[i] = view.currentRoll[i].value
			continue
		}

		view.currentRoll[i] = &Die{
			value:   roll,
			texture: diceTextures[roll-1],
		}
	}
}

func (view *DiceView) ShakeAnimation() {
	go func() {
		for i := 0; i < 15; i++ {
			view.Shake()
			time.Sleep(time.Millisecond * 100)
		}
	}()
}

func (view *DiceView) Render() {

	imgui.SetNextWindowPos(diceViewPos)
	imgui.SetNextWindowSize(diceViewSize)

	if imgui.Begin(diceViewID) {

		imgui.SetNextWindowPos(imgui.Vec2{X: 717, Y: 170})

		if imgui.BeginChildV("View", imgui.Vec2{X: 155, Y: 130}, false, 0) {

			imgui.Dummy(imgui.Vec2{X:0,Y:0})
			imgui.SameLineV(15, 0)
			imgui.BeginGroup()

			imgui.ColumnsV(3, diceTexturesID, false)

			for i, die := range view.currentRoll {

				texture := graphics.Textures.Get(die.texture)

				imgui.PushID(string(i))

				var color imgui.Vec4

				if die.selected {
					color = imgui.Vec4{X: 0, Y: 230, Z: 0, W: 230}
				} else {
					color = imgui.Vec4{X: 0, Y: 0, Z: 0, W: 0}
				}

				if imgui.ImageButtonV(texture.Id, dieSize, imgui.Vec2{X: 0, Y: 0}, imgui.Vec2{X: 1, Y: 1},
					0, color, imgui.Vec4{X: 1, Y: 1, Z: 1, W: 1}) {
					die.selected = !die.selected
				}

				imgui.PopID()

				imgui.NextColumn()
				imgui.NextColumn()
			}

			imgui.EndGroup()
		}
		imgui.EndChild()

		if imgui.BeginChild("Actions") {

			imgui.SameLineV(40, 0)

			if imgui.ButtonV("Roll", imgui.Vec2{X: 100, Y: 30}) {
				if view.rollCount >= maxRollPerTurn {
					imgui.OpenPopup(alertMaxRoll)
				} else {
					view.rollCount++

					view.ShakeAnimation()
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
