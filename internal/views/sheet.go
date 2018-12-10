package views

import (
	"fmt"
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/yahtsee"
	"strconv"
)

const sheetViewID = "SheetView"

var sheetViewSize = imgui.Vec2{ X:float32(650), Y:float32(440) }

// SheetView A view of the score sheet
type SheetView struct {
	history []*yahtsee.Score
}

// NewSheetView Creates a new board view
func NewSheetView() *SheetView {
	view := &SheetView{nil}

	view.history = append(view.history, &yahtsee.Score{})

	return view
}

// CurrentScore gets the current score from the history
func (view *SheetView) CurrentScore() *yahtsee.Score {
	return view.history[0]
}

// Render displays the board view
func (view *SheetView) Render() {

	if imgui.BeginChildV(sheetViewID, sheetViewSize, false, imgui.WindowFlagsAlwaysAutoResize) {

		imgui.ColumnsV(3, "Upper", true)

		imgui.PushStyleVarVec2(imgui.StyleVarItemSpacing, imgui.Vec2{X: float32(10), Y: float32(10)})

		imgui.Text(fmt.Sprintf("Game #%d", len(view.history)))

		imgui.NextColumn()

		imgui.Text("How to score")

		imgui.NextColumn()

		imgui.Text("Score")

		imgui.NextColumn()

		selected := yahtsee.Max

		for i := yahtsee.Aces; i <= yahtsee.Yahtsee; i++ {

			imgui.Separator()

			imgui.Text(yahtsee.ScoreNames[i])

			imgui.NextColumn()

			imgui.Text(yahtsee.ScoreHelp[i])

			imgui.NextColumn()

			if imgui.SelectableV(
				strconv.FormatInt(int64(view.CurrentScore().Get(i)), 10),
				selected == i, 0, imgui.Vec2{}) {
			}

			imgui.NextColumn()
		}

		imgui.PopStyleVar()
	}

	imgui.EndChild()
}

// Update updates logic in the board view
func (view *SheetView) Update() {

}
