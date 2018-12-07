package views

import (
	"fmt"
	"github.com/ryjen/imgui-go"
)

type DemoView struct {
	showDemo bool
	counter int
}

func NewDemoView() *DemoView {
	return &DemoView{ false, 0}
}

func (view *DemoView) Hello() {
	if !imgui.Begin("Hello") {
		return
	}

	imgui.Text("Hello, world!")

	imgui.Checkbox("Demo Window", &view.showDemo)

	if imgui.Button("Button") {
		view.counter++
	}
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("counter = %d", view.counter))

	imgui.End()
}

func (view *DemoView) Render() {

	view.Hello()

	if view.showDemo {
		imgui.ShowDemoWindow(&view.showDemo)
	}
}

func (view *DemoView) Update() {

}
