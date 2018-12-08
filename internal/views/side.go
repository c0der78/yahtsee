package views

import "github.com/ryjen/imgui-go"

const SideViewID = "SideView"

var sideViewSize = imgui.Vec2{
	X: 200, Y: 500,
}

type SideView struct {
	dice *DiceView
}

func NewSideView() *SideView {
	return &SideView{
		dice: NewDiceView(),
	}
}

func (view *SideView) Render() {

	imgui.SameLine()

	if imgui.BeginChildV(SideViewID, sideViewSize, true, imgui.WindowFlagsAlwaysAutoResize) {

		view.dice.Render()
	}

	imgui.EndChild()
}

func (view *SideView) Update() {}
