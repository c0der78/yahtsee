package views

import "github.com/ryjen/imgui-go"

const playersViewId = "Players"

var playersViewSize = imgui.Vec2{X: 200, Y: 80}

type PlayersView struct {
	OnSelect func()
}

func NewPlayersView() *PlayersView {
	return &PlayersView{
		OnSelect: func() {},
	}
}

func (view *PlayersView) Render() {

	imgui.SetNextWindowPos(imgui.Vec2{X: 700, Y: 40})
	imgui.SetNextWindowSize(playersViewSize)

	if imgui.Begin(playersViewId) {

		if imgui.Selectable("You") {
			view.OnSelect()
		}
	}

	imgui.End()
}

func (view *PlayersView) Update() {

}
