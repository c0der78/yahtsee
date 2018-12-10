package views

import "github.com/ryjen/imgui-go"

const playersViewId = "PlayersView"

var playersViewSize = imgui.Vec2{X: 200, Y: 80 }

type PlayersView struct {
	OnSelect func()
}

func NewPlayersView() *PlayersView {
	return &PlayersView{
		OnSelect: func(){},
	}
}

func (view *PlayersView) Render() {

	if imgui.BeginChildV(playersViewId, playersViewSize, true, imgui.WindowFlagsMenuBar) {

		if imgui.BeginMenuBar() {
			if imgui.BeginMenu("Players") {
				imgui.EndMenu()
			}
			imgui.EndMenuBar()
		}

		if imgui.Selectable("You") {
			view.OnSelect()
		}
	}

	imgui.EndChild()
}

func (view *PlayersView) Update() {

}
