package views

import (
	"fmt"
	"github.com/ryjen/imgui-go"
	"micrantha.com/yahtsee/internal/events"
	"micrantha.com/yahtsee/internal/states"
	"micrantha.com/yahtsee/internal/yahtsee"
	"strconv"
)

var sheetViewSize = imgui.Vec2{X: float32(650), Y: float32(480)}

var sheetViewPos = imgui.Vec2{X: 20, Y: 40}

type history = []*yahtsee.Score

// SheetView A view of the score sheet
type SheetView struct {
	history history
	state *states.State
}

// NewSheetView Creates a new board view
func NewSheetView(state *states.State) *SheetView {
	view := &SheetView{
		history: history{
			yahtsee.NewScore(),
		},
		state: state,
	}

	return view
}

// CurrentScore gets the current score from the history
func (view *SheetView) CurrentScore() *yahtsee.Score {
	return view.history[0]
}

// Render displays the board view
func (view *SheetView) Render() {

	imgui.SetNextWindowPos(sheetViewPos)
	imgui.SetNextWindowSize(sheetViewSize)

	sheetViewID := fmt.Sprintf("Game #%d", len(view.history))

	if imgui.BeginV(sheetViewID, nil, imgui.WindowFlagsMenuBar) {

		if imgui.BeginMenuBar() {
			if imgui.BeginMenu("You") {
				imgui.EndMenu()
			}
			imgui.EndMenuBar()
		}
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

			imgui.PushID(string(i))

			score := view.CurrentScore().Get(i)

			if score > -1 {
				imgui.Text(strconv.FormatInt(int64(score), 10))
			} else {
				if imgui.SelectableV("",
					false, 0, imgui.Vec2{}) {
					selected = i
				}
			}

			imgui.PopID()

			imgui.NextColumn()
		}

		if selected < yahtsee.Max {
			events.Bus().Emit(events.ScoreEventID, events.NewScoreEvent(selected))
			view.CurrentScore().Set(selected, view.state.CurrentRoll)
		}

		imgui.PopStyleVar()
	}

	imgui.End()
}

// Update updates logic in the board view
func (view *SheetView) Update() {

}
