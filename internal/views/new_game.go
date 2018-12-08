package views

import "github.com/ryjen/imgui-go"

type NewGameView struct {

}

func NewNewGameView() *NewGameView {
	return &NewGameView{}
}

func (view *NewGameView) Render() {

	if imgui.BeginPopupModal("New Game") {

		imgui.LabelText("What is your name?", "")

		imgui.Button("Create")
	}
	imgui.EndPopup()
}

func (view *NewGameView) Update() {

}