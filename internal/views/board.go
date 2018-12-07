package views

import (
	"fmt"
	"github.com/ryjen/imgui-go"
)

const BoardViewId = "BoardView"

type Score struct {}

type BoardView struct {
	history []*Score
}

func NewBoardView() *BoardView {
	view := &BoardView{nil}

	view.history = append(view.history, &Score{})

	return view
}

func (view *BoardView) Render() {
	if !imgui.Begin(BoardViewId) {
		return
	}

	if imgui.BeginChild("Upper") {
		imgui.Columns(3)

		imgui.Text(fmt.Sprintf("Game #%d", len(view.history)))

		imgui.NextColumn()

		imgui.Text("How to score")

		imgui.NextColumn()

		imgui.Text("Score")

		imgui.NextColumn()

		imgui.SameLine()

		imgui.Text("Aces = 1")

		imgui.NextColumn()

		imgui.Text("Count and add only aces")

		imgui.NextColumn()

		imgui.NextColumn()

		imgui.Text("Twos ⚁ = 2")

		imgui.NextColumn()

		imgui.Text("Count and add only twos")

		imgui.NextColumn()

		imgui.EndChild()
	}

	if imgui.BeginChild("Lower") {

		//imgui.Columns(3)

		imgui.EndChild()
	}

	imgui.End()
}

func (view *BoardView) Update() {

}
