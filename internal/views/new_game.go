package views

import "github.com/ryjen/imgui-go"

type NewGameView struct {

}

func NewNewGameView() *NewGameView {
	return &NewGameView{}
}

func (view *NewGameView) Render() {

	if !imgui.Begin("New Game") {
		return
	}

	imgui.LabelText("What is your name?", "")

	imgui.Button("Create")

	imgui.End()
}

func (view *NewGameView) Update() {

}